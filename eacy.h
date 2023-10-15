#pragma once

/**
 * Copyright 2023 Matthew Peter Smith
 * This code is provided under an MIT License. 
 * See LICENSE.txt at the root of this Git repository. 
 */

#include<dlfcn.h>

#include"load.h"

#ifdef __cplusplus
#include<iostream>
#else
#include<stdbool.h>
#include<stdio.h>
#endif

#define EAC_LIB_INIT \
    static int lib_index = -1;\
    static void* func = NULL;\
    static int rev = 0;\
    if(lib_index < 0) lib_index = EACLoad(EAC_LIB);

#define EAC_LIB_MAYBE_RELOAD(func_str) \
    if(EACGetRevision() > rev) {\
        EACReload(lib_index);\
        void* lib = EACGet(lib_index);\
        func = dlsym(lib, func_str);\
        rev = EACGetRevision();\
    }

#ifdef EAC_DEBUG
#define EAC_FUNC(fhandle, ret, arg_names) \
    {\
        EAC_LIB_INIT;\
        if(!func) func = dlsym(EACGet(lib_index), EAC_STR(fhandle##_export));\
        EAC_LIB_MAYBE_RELOAD(EAC_STR(fhandle##_export));\
        ret res = ((typeof(&fhandle))func)(arg_names);\
        return res;\
    }

#define EAC_VOID_FUNC(fhandle, arg_names) \
    {\
        EAC_LIB_INIT;\
        if(!func) func = dlsym(EACGet(lib_index), EAC_STR(fhandle##_export));\
        EAC_LIB_MAYBE_RELOAD(EAC_STR(fhandle##_export));\
        ((typeof(&fhandle))func)(arg_names);\
        return;\
    }

#define EAC_STATIC_FUNC(fhandle, ret, arg_names) \
    {\
        EAC_LIB_INIT;\
        if(!func) func = dlsym(EACGet(lib_index), EAC_STR(fhandle##_export));\
        EAC_LIB_MAYBE_RELOAD(EAC_STR(fhandle##_export));\
        ret res = ((typeof(&fhandle))func)(arg_names);\
        return res;\
    }

#define EAC_STATIC_VOID_FUNC(fhandle, arg_names) \
    {\
        EAC_LIB_INIT;\
        if(!func) func = dlsym(EACGet(lib_index), EAC_STR(fhandle##_export));\
        EAC_LIB_MAYBE_RELOAD(EAC_STR(fhandle##_export));\
        ((typeof(&fhandle))func)(arg_names);\
        return;\
    }

#define EAC_METHOD(cname, fname, ret, arg_names) \
    {\
        EAC_LIB_INIT;\
        if(!func) func = dlsym(EACGet(lib_index), EAC_STR(cname##_##fname##_export));\
        EAC_LIB_MAYBE_RELOAD(EAC_STR(cname##_##fname##_export));\
        ret res = ((typeof(&cname##_##fname##_export))func)(this /*,*/ arg_names);\
        return res;\
    }

#define EAC_VOID_METHOD(cname, fname, arg_names) \
    {\
        EAC_LIB_INIT;\
        if(!func) func = dlsym(EACGet(lib_index), EAC_STR(cname##_##fname##_export));\
        EAC_LIB_MAYBE_RELOAD(EAC_STR(cname##_##fname##_export));\
        ((typeof(&cname##_##fname##_export))func)(this /*,*/ arg_names);\
        return;\
    }

#else
#define EAC_FUNC(fname, ret, arg_names)
#define EAC_VOID_FUNC(fname, arg_names) 
#define EAC_STATIC_FUNC(fname, ret, arg_names)
#define EAC_STATIC_VOID_FUNC(fname, arg_names) 
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



