// Copyright (c) 2014-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Logger.hpp>
#include <SoapySDR/Version.hpp>
#include <vector>
#include <string>
#include <cstdlib> //getenv
#include <sstream>
#include <map>

#ifdef _MSC_VER
#include <windows.h>
#else
#include <dlfcn.h>
#include <glob.h>
#endif

/***********************************************************************
 * root installation path
 **********************************************************************/
std::string getEnvImpl(const char *name)
{
    #ifdef _MSC_VER
    const DWORD len = GetEnvironmentVariableA(name, 0, 0);
    if (len == 0) return "";
    char* buffer = new char[len];
    GetEnvironmentVariableA(name, buffer, len);
    std::string result(buffer);
    delete [] buffer;
    return result;
    #else
    const char *result = getenv(name);
    if (result != NULL) return result;
    #endif
    return "";
}

std::string SoapySDR::getRootPath(void)
{
    const std::string rootPathEnv = getEnvImpl("SOAPY_SDR_ROOT");
    if (not rootPathEnv.empty()) return rootPathEnv;

    // Get the path to the current dynamic linked library.
    // The path to this library can be used to determine
    // the installation root without prior knowledge.
    #ifdef _MSC_VER
    char path[MAX_PATH];
    HMODULE hm = NULL;
    if (GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR) &SoapySDR::getRootPath, &hm))
    {
        const DWORD size = GetModuleFileNameA(hm, path, sizeof(path));
        if (size != 0)
        {
            const std::string libPath(path, size);
            const size_t slash0Pos = libPath.find_last_of("/\\");
            const size_t slash1Pos = libPath.substr(0, slash0Pos).find_last_of("/\\");
            if (slash0Pos != std::string::npos and slash1Pos != std::string::npos)
                return libPath.substr(0, slash1Pos);
        }
    }
    #endif

    return "@SOAPY_SDR_ROOT@";
}

/***********************************************************************
 * list modules API call
 **********************************************************************/
static std::vector<std::string> searchModulePath(const std::string &path)
{
    const std::string pattern = path + "*.*";
    std::vector<std::string> modulePaths;

#ifdef _MSC_VER

    //http://stackoverflow.com/questions/612097/how-can-i-get-a-list-of-files-in-a-directory-using-c-or-c
    WIN32_FIND_DATA fd; 
    HANDLE hFind = ::FindFirstFile(pattern.c_str(), &fd); 
    if(hFind != INVALID_HANDLE_VALUE) 
    { 
        do 
        { 
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) 
            {
                modulePaths.push_back(path + fd.cFileName);
            }
        }while(::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind); 
    }

#else

    glob_t globResults;

    const int ret = glob(pattern.c_str(), 0/*no flags*/, NULL, &globResults);
    if (ret == 0) for (size_t i = 0; i < globResults.gl_pathc; i++)
    {
        modulePaths.push_back(globResults.gl_pathv[i]);
    }
    else if (ret == GLOB_NOMATCH) {/* acceptable error condition, do not print error */}
    else SoapySDR::logf(SOAPY_SDR_ERROR, "SoapySDR::listModules(%s) glob(%s) error %d", path.c_str(), pattern.c_str(), ret);

    globfree(&globResults);

#endif

    return modulePaths;
}

std::vector<std::string> SoapySDR::listModules(void)
{
    //the default search path
    std::vector<std::string> searchPaths;
    searchPaths.push_back(SoapySDR::getRootPath() + "/lib@LIB_SUFFIX@/SoapySDR/modules" + SoapySDR::getABIVersion());

    //support /usr/local module installs when the install prefix is /usr
    if (SoapySDR::getRootPath() == "/usr")
    {
        searchPaths.push_back("/usr/local/lib@LIB_SUFFIX@/SoapySDR/modules" + SoapySDR::getABIVersion());
        //when using a multi-arch directory, support single-arch path as well
        static const std::string libsuffix("@LIB_SUFFIX@");
        if (not libsuffix.empty() and libsuffix.at(0) == '/')
            searchPaths.push_back("/usr/local/lib/SoapySDR/modules" + SoapySDR::getABIVersion());
    }

    //separator for search paths
    #ifdef _MSC_VER
    static const char sep = ';';
    #else
    static const char sep = ':';
    #endif

    //check the environment's search path
    std::stringstream pluginPaths(getEnvImpl("SOAPY_SDR_PLUGIN_PATH"));
    std::string pluginPath;
    while (std::getline(pluginPaths, pluginPath, sep))
    {
        if (pluginPath.empty()) continue;
        searchPaths.push_back(pluginPath);
    }

    //traverse the search paths
    std::vector<std::string> modules;
    for (size_t i = 0; i < searchPaths.size(); i++)
    {
        const std::vector<std::string> subModules = SoapySDR::listModules(searchPaths.at(i));
        modules.insert(modules.end(), subModules.begin(), subModules.end());
    }
    return modules;
}

std::vector<std::string> SoapySDR::listModules(const std::string &path)
{
    return searchModulePath(path + "/"); //requires trailing slash
}

/***********************************************************************
 * load module API call
 **********************************************************************/
std::map<std::string, void *> &getModuleHandles(void)
{
    static std::map<std::string, void *> handles;
    return handles;
}

//! share the module path during loadModule
std::string &getModuleLoading(void)
{
    static std::string moduleLoading;
    return moduleLoading;
}

//! share registration errors during loadModule
std::map<std::string, SoapySDR::Kwargs> &getLoaderResults(void)
{
    static std::map<std::string, SoapySDR::Kwargs> results;
    return results;
}

#ifdef _MSC_VER
static std::string GetLastErrorMessage(void)
{
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    std::string msg((char *)lpMsgBuf);
    LocalFree(lpMsgBuf);
    return msg;
}
#endif

std::string SoapySDR::loadModule(const std::string &path)
{
    //check if already loaded
    if (getModuleHandles().count(path) != 0) return path + " already loaded";

    //stash the path for registry access
    getModuleLoading().assign(path);

    //load the module
#ifdef _MSC_VER
    HMODULE handle = LoadLibrary(path.c_str());
    getModuleLoading().clear();
    if (handle == NULL) return "LoadLibrary() failed: " + GetLastErrorMessage();
#else
    void *handle = dlopen(path.c_str(), RTLD_LAZY);
    getModuleLoading().clear();
    if (handle == NULL) return "dlopen() failed: " + std::string(dlerror());
#endif

    //stash the handle
    getModuleHandles()[path] = handle;
    return "";
}

SoapySDR::Kwargs SoapySDR::getLoaderResult(const std::string &path)
{
    if (getLoaderResults().count(path) == 0) return SoapySDR::Kwargs();
    return getLoaderResults()[path];
}

std::string SoapySDR::unloadModule(const std::string &path)
{
    //check if already loaded
    if (getModuleHandles().count(path) == 0) return path + " never loaded";

    //stash the path for registry access
    getModuleLoading().assign(path);

    //unload the module
    void *handle = getModuleHandles()[path];
#ifdef _MSC_VER
    BOOL success = FreeLibrary((HMODULE)handle);
    getModuleLoading().clear();
    if (not success) return "FreeLibrary() failed: " + GetLastErrorMessage();
#else
    int status = dlclose(handle);
    getModuleLoading().clear();
    if (status != 0) return "dlclose() failed: " + std::string(dlerror());
#endif

    //clear the handle
    getLoaderResults().erase(path);
    getModuleHandles().erase(path);
    return "";
}

/***********************************************************************
 * load modules API call
 **********************************************************************/

void lateLoadNullDevice(void);

void SoapySDR::loadModules(void)
{
    static bool loaded = false;
    if (loaded) return;
    loaded = true;
    lateLoadNullDevice();

    const auto paths = listModules();
    for (size_t i = 0; i < paths.size(); i++)
    {
        if (getModuleHandles().count(paths[i]) != 0) continue; //was manually loaded
        const std::string errorMsg = loadModule(paths[i]);
        if (not errorMsg.empty()) SoapySDR::logf(SOAPY_SDR_ERROR, "SoapySDR::loadModule(%s)\n  %s", paths[i].c_str(), errorMsg.c_str());
        for (const auto &it : SoapySDR::getLoaderResult(paths[i]))
        {
            if (it.second.empty()) continue;
            SoapySDR::logf(SOAPY_SDR_ERROR, "SoapySDR::loadModule(%s)\n  %s", paths[i].c_str(), it.second.c_str());
        }
    }
}
