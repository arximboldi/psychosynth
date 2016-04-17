/*
 *  File:       warnings.hpp
 *  Author:     Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  Date:       Fri Apr 15 15:01:04 2016
 *
 *  Tools to disable warnings for external headers.
 */

#pragma once

#if defined(__clang__)
#  define PSYNTH_DISABLE_WARNINGS                                       \
    _Pragma("clang diagnostic push")                                    \
    _Pragma("clang diagnostic ignored \"-Wconversion\"")                \
    _Pragma("clang diagnostic ignored \"-Wcovered-switch-default\"")    \
    _Pragma("clang diagnostic ignored \"-Wdeprecated\"")                \
    _Pragma("clang diagnostic ignored \"-Wdisabled-macro-expansion\"")  \
    _Pragma("clang diagnostic ignored \"-Wdocumentation-unknown-command\"") \
    _Pragma("clang diagnostic ignored \"-Wdocumentation\"")             \
    _Pragma("clang diagnostic ignored \"-Wexit-time-destructors\"")     \
    _Pragma("clang diagnostic ignored \"-Wextra-semi\"")                \
    _Pragma("clang diagnostic ignored \"-Wfloat-equal\"")               \
    _Pragma("clang diagnostic ignored \"-Wglobal-constructors\"")       \
    _Pragma("clang diagnostic ignored \"-Wheader-hygiene\"")            \
    _Pragma("clang diagnostic ignored \"-Wkeyword-macro\"")             \
    _Pragma("clang diagnostic ignored \"-Wmissing-noreturn\"")          \
    _Pragma("clang diagnostic ignored \"-Wmissing-prototypes\"")        \
    _Pragma("clang diagnostic ignored \"-Wold-style-cast\"")            \
    _Pragma("clang diagnostic ignored \"-Wpadded\"")                    \
    _Pragma("clang diagnostic ignored \"-Wreserved-id-macro\"")         \
    _Pragma("clang diagnostic ignored \"-Wshadow\"")                    \
    _Pragma("clang diagnostic ignored \"-Wshift-sign-overflow\"")       \
    _Pragma("clang diagnostic ignored \"-Wshorten-64-to-32\"")          \
    _Pragma("clang diagnostic ignored \"-Wsign-compare\"")              \
    _Pragma("clang diagnostic ignored \"-Wsign-conversion\"")           \
    _Pragma("clang diagnostic ignored \"-Wswitch-enum\"")               \
    _Pragma("clang diagnostic ignored \"-Wundef\"")                     \
    _Pragma("clang diagnostic ignored \"-Wunreachable-code\"")          \
    _Pragma("clang diagnostic ignored \"-Wunused-local-typedef\"")      \
    _Pragma("clang diagnostic ignored \"-Wunused-parameter\"")          \
    _Pragma("clang diagnostic ignored \"-Wused-but-marked-unused\"")    \
    _Pragma("clang diagnostic ignored \"-Wuser-defined-literals\"")     \
    _Pragma("clang diagnostic ignored \"-Wweak-vtables\"")              \
    //
#  define PSYNTH_RESTORE_WARNINGS                    \
    _Pragma("clang diagnostic pop")                  \
    //
#elif defined(_MSC_VER)
/**
 * C4100: 'identifier' : unreferenced formal parameter
 * C4127: conditional expression is constant
 * C4244: 'conversion' conversion from 'type1' to 'type2',
          possible loss of data
 * C4251: 'identifier' : class 'type' needs to have dll-interface
          to be used by clients of class 'type2'
 * C4800: 'type' : forcing value to bool 'true' or 'false'
          (performance warning)
 */
#  define PSYNTH_DISABLE_WARNINGS               \
    __pragma(warning(push))                     \
    __pragma(warning(disable: 4100))            \
    __pragma(warning(disable: 4127))            \
    __pragma(warning(disable: 4244))            \
    __pragma(warning(disable: 4251))            \
    __pragma(warning(disable: 4800))            \
    //
#  define PSYNTH_RESTORE_WARNINGS               \
    __pragma(warning(pop))                      \
    //
#elif defined(__GNUC__)
#  define PSYNTH_DISABLE_WARNINGS                                       \
    _Pragma("GCC diagnostic push")                                      \
    _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")     \
    //
#  define PSYNTH_RESTORE_WARNINGS               \
    _Pragma("GCC diagnostic pop")               \
    //
#else
#  define PSYNTH_DISABLE_WARNINGS
#  define PSYNTH_RESTORE_WARNINGS
#endif
