// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Modules.hpp>
#include <vector>
#include <string>
#include <cstdlib> //getenv
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
std::string SoapySDR::getRootPath(void)
{
    const char *rootPathEnv = getenv("SOAPY_SDR_ROOT");
    if (rootPathEnv != NULL) return rootPathEnv;
    return "@CMAKE_INSTALL_PREFIX@";
}

/***********************************************************************
 * list modules API call
 **********************************************************************/
std::vector<std::string> SoapySDR::listModules(void)
{
    std::vector<std::string> modulePaths;

#ifdef _MSC_VER

    const std::string pattern = SoapySDR::getRootPath() + "\\lib@LIB_SUFFIX@\\SoapySDR\\modules\\*.*";

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
                modulePaths.push_back(fd.cFileName);
            }
        }while(::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind); 
    }

#else

    const std::string pattern = SoapySDR::getRootPath() + "/lib@LIB_SUFFIX@/SoapySDR/modules/*.*";
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

/***********************************************************************
 * load module API call
 **********************************************************************/
void SoapySDR::loadModule(const std::string &path)
{
#ifdef _MSC_VER
    HMODULE handle = LoadLibrary(path.c_str());
    if (handle == NULL) std::cerr << "SoapySDR::loadModules() LoadLibrary(" << path << ") failed" << std::endl;
#else
    void *handle = dlopen(path.c_str(), RTLD_LAZY);
    if (handle == NULL) std::cerr << "SoapySDR::loadModules() dlopen(" << path << ") failed" << std::endl;
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