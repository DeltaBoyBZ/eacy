/**
 * Copyright 2023 Matthew Peter Smith
 *
 * This code is provided under an MIT License. 
 * See LICENSE.txt at the root of this Git repository. 
 */

#include<dlfcn.h>
#include<string.h>
#include<stdio.h>

#include<string>
#include<sstream>
#include<vector>
#include<iostream>

static std::vector<void*> lib_handles; 
static std::vector<std::string> lib_names;
static unsigned rev = 0;

extern "C" unsigned EACLoad(const char* name)
{
    //sprintf(fullname, "./.eac/%s", name);
    std::string fullname = std::string(".eac/") + name;
    void* lib = dlopen(fullname.c_str(), RTLD_LAZY | RTLD_LOCAL); 
    lib_handles.push_back(lib); 
    std::string libname;
    libname.assign(name);
    lib_names.push_back(libname); 
    return lib_handles.size() - 1;
}

extern "C" void* EACGet(unsigned index)
{
    return lib_handles[index];
}

extern "C" void EACReload(unsigned index)
{
    void* lib = lib_handles[index];
    dlclose(lib); 
    std::string rev_name = std::string(".eac.") 
        + std::to_string(rev) + "/"
        + lib_names[index]; 
    std::cout << rev_name << std::endl;
    lib_handles[index] = dlopen(rev_name.c_str(), RTLD_LAZY | RTLD_LOCAL); 
}

extern "C" void EACRevision(unsigned x)
{
    rev = x; 
}

extern "C" unsigned EACGetRevision()
{
    return rev;
}

extern "C" void EACRevise()
{
    rev += 1; 
}

