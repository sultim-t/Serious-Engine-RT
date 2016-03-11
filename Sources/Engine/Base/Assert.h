/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_ASSERT_H
#define SE_INCL_ASSERT_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#ifdef _MSC_VER  /* rcg10042001 */

#ifdef __cplusplus
  extern "C" {
#endif

  /* original VC definition of _assert */
  _CRTIMP void __cdecl _assert(void *, void *, unsigned);

#ifdef __cplusplus
  }
#endif

#endif


#ifdef PLATFORM_UNIX  /* rcg10042001 */
#include <assert.h>
#include <signal.h>
#define _assert(x, y, z) assert(0)
#endif


#ifdef NDEBUG
  #ifndef ASSERT
    #define ASSERT(__ignore) ((void)0)
  #endif
  #define ASSERTMSG(__ignore1, __ignore2) ((void)0)
  #define ASSERTALWAYS(__ignore) ((void)0)
  #define BREAKPOINT ((void)0)
  #define SAFEBREAKPOINT ((void)0)
  #define IFDEBUG(__ignore) ((void)0)
  #define DEBUGSTRING(__ignore) ("")
#else

/* From VisualC assert.h: Define _CRTIMP */
  #ifndef _CRTIMP
  #ifdef	_NTSDK
  /* definition compatible with NT SDK */
  #define _CRTIMP
  #else	/* ndef _NTSDK */
  /* current definition */
  #ifdef	_DLL
  #define _CRTIMP __declspec(dllimport)
  #else	/* ndef _DLL */
  #define _CRTIMP
  #endif	/* _DLL */
  #endif	/* _NTSDK */
  #endif	/* _CRTIMP */
/* End: Define _CRTIMP */

  // this breakpoint doesn't cause exceptions if not in debugger
  #ifdef _MSC_VER /* rcg10042001 */
    #define SAFEBREAKPOINT try { _asm { int 3 }; } catch(...) {;}
  #endif

  #ifdef PLATFORM_UNIX /* rcg10042001 */
    #define SAFEBREAKPOINT raise(SIGTRAP)
  #endif

  /* CT break point macro */
  #define BREAKPOINT                                                \
    if (1) {					                                              \
      SAFEBREAKPOINT;                                               \
      _assert("hard-coded breakpoint (CroTeam)",__FILE__,__LINE__); \
    } else NOTHING

  /* CT DEBUG macro -- the expression is executed only in debug version */
  #ifndef IFDEBUG
    #define IFDEBUG(expr) 				\
      expr
  #endif

  /* CT assertion macros */
  #ifndef ASSERT
    #define ASSERT(expr) 				            \
      if(!(expr)) {				                  \
        /*SAFEBREAKPOINT;*/                     \
        _assert(#expr,__FILE__,__LINE__);		\
      } else NOTHING
  #endif

  #define ASSERTALWAYS(msg)			                \
    if (1) {					                          \
      /*SAFEBREAKPOINT;*/                           \
      _assert(msg,__FILE__,__LINE__); 	      	\
    } else NOTHING

  #define ASSERTMSG(expr, msg) 			            \
    if(!(expr)) {				                        \
      /*SAFEBREAKPOINT;*/                           \
      _assert(msg,__FILE__,__LINE__); 	        \
    } else NOTHING
  #define DEBUGSTRING(str) (str)
#endif



#endif  /* include-once check. */

