#pragma once

/**
 * Copyright 2023 Matthew Peter Smith
 *
 * This code is provided under an MIT License. 
 * See LICENSE.txt at the root of this Git repository. 
 */

#include<dlfcn.h>
#include<stdio.h>
#include<stdbool.h>

#ifdef __cplusplus
#include<iostream>
#endif

#ifdef EAC_DEBUG
#define EAC_FUNC(fhandle, ret, arg_names)\
    {\
        static void* lib = NULL;\
        static void* func = NULL;\
        static bool reload = true;\
        if(reload) {\
            if(lib) dlclose(lib);\
            lib  = dlopen(EAC_LIB, 1);\
            func = dlsym(lib, EAC_STR(fhandle##_export));\
        }\
        reload = false;\
        ret res = ((typeof(&fhandle))func)(arg_names);\
        return res;\
    }
#define EAC_VOID_FUNC(fhandle, arg_names)\
    {\
        static void* lib = NULL;\
        static void* func = NULL;\
        static bool reload = true;\
        if(reload) {\
            if(lib) dlclose(lib);\
            lib  = dlopen(EAC_LIB, 1);\
            func = dlsym(lib, EAC_STR(fhandle##_export));\
        }\
        reload = false;\
        ((typeof(&fhandle))func)(arg_names);\
        return;\
    }
#define EAC_METHOD(cname, fname, ret, arg_names)\
{\
    static void* lib = NULL;\
    static void* func = NULL;\
    static bool reload = true;\
    if(reload) {\
        if(lib) dlclose(lib);\
        lib  = dlopen(EAC_LIB, 1);\
        func = dlsym(lib, EAC_STR(cname##_##fname##_export));\
    }\
    reload = false;\
    ret res = ((typeof(&cname##_##fname##_export))func)(this /*,*/ arg_names);\
    return res;\
}
#define EAC_VOID_METHOD(cname, fname, arg_names)\
{\
    static void* lib = NULL;\
    static void* func = NULL;\
    static bool reload = true;\
    if(reload) {\
        if(lib) dlclose(lib);\
        lib  = dlopen(EAC_LIB, 1);\
        func = dlsym(lib, EAC_STR(cname##_##fname##_export));\
    }\
    reload = false;\
    ((typeof(&cname##_##fname##_export))func)(this /*,*/ arg_names);\
    return;\
}

#else
#define EAC_FUNC(fname, ret, arg_names)
#define EAC_VOID_FUNC(fname, arg_names) 
#define EAC_METHOD(cname, fname, ret, arg_names) 
#define EAC_VOID_METHOD(cname, fname, arg_names) 
#endif

#ifdef __cplusplus 
#define EAC_EXPORT(fname, ret, args, arg_names)\
    extern "C" ret fname##_export(args) {\
        return fname(arg_names);\
    }
#define EAC_VOID_EXPORT(fname, args, arg_names)\
    extern "C" void fname##_export(args) {\
        fname(arg_names);\
    }
#else
#define EAC_EXPORT(fname, ret, args, arg_names)\
    ret fname##_export(args) {\
        return fname(arg_names);\
    }
#define EAC_VOID_EXPORT(fname, args, arg_names)\
    void fname##_export(args) {\
        fname(arg_names);\
    }
#endif

#define EAC_METHOD_EXPORT(cname, fname, ret, args, arg_names)\
    extern "C" ret cname##_##fname##_export(cname* cname##_instance /*,*/ args) {\
        return cname##_instance->fname(arg_names);\
    }

#define EAC_VOID_METHOD_EXPORT(cname, fname, args, arg_names)\
    extern "C" void cname##_##fname##_export(cname* cname##_instance /*,*/ args) {\
        cname##_instance->fname(arg_names);\
    }

#define EAC_METHOD_EXPORT_DEC(cname, fname, ret, args)\
    class cname;\
    extern "C" ret cname##_##fname##_export(cname* /*,*/ args);

#define EAC_VOID_METHOD_EXPORT_DEC(cname, fname, args)\
    class cname;\
    extern "C" void cname##_##fname##_export(cname* /*,*/ args);

#define EAC
#define EAC_CLASS
#define EAC_WRAP(...) __VA_ARGS__
#define EAC_STR(x) #x

#ifdef EAC_DEBUG
static void eacy_reload(const char* func, bool* reload) { }
#endif



