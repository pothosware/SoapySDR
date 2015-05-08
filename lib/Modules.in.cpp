// Copyright (c) 2014-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Modules.hpp>
#include <vector>
#include <string>
#include <cstdlib> //getenv
#include <sstream>
#include <iostream>

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
    return "@CMAKE_INSTALL_PREFIX@";
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
    else std::cerr << "SoapySDR::listModules() glob(" << pattern << ") error " << ret << std::endl;

    globfree(&globResults);

#endif

    return modulePaths;
}

std::vector<std::string> SoapySDR::listModules(void)
{
    //the default search path
    std::vector<std::string> searchPaths;
    searchPaths.push_back(SoapySDR::getRootPath() + "/lib@LIB_SUFFIX@/SoapySDR/modules");

    //support /usr/local module installs when the install prefix is /usr
    if (SoapySDR::getRootPath() == "/usr")
    {
        searchPaths.push_back("/usr/local/lib@LIB_SUFFIX@/SoapySDR/modules");
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
        const std::string &path = searchPaths.at(i) + "/"; //requires trailing slash
        const std::vector<std::string> subModules = searchModulePath(path);
        modules.insert(modules.end(), subModules.begin(), subModules.end());
    }
    return modules;
}

/***********************************************************************
 * load module API call
 **********************************************************************/
void SoapySDR::loadModule(const std::string &path)
{
#ifdef _MSC_VER
    HMODULE handle = LoadLibrary(path.c_str());
    if (handle == NULL) std::cerr << "SoapySDR::loadModules() LoadLibrary(" << path << ") failed: " << GetLastError() << std::endl;
#else
    void *handle = dlopen(path.c_str(), RTLD_LAZY);
    if (handle == NULL) std::cerr << "SoapySDR::loadModules() dlopen(" << path << ") failed: " << dlerror() << std::endl;
#endif
}

/***********************************************************************
 * load modules API call
 **********************************************************************/
void SoapySDR::loadModules(void)
{
    static bool loaded = false;
    if (loaded) return;
    loaded = true;

    const std::vector<std::string> paths = listModules();
    for (size_t i = 0; i < paths.size(); i++)
    {
        loadModule(paths[i]);
    }
}
