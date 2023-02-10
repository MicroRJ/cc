/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
#ifndef _CC
#define _CC

#ifdef _WIN32
#ifndef CINTERFACE
# define CINTERFACE
#endif
#ifndef NOMINMAX
# define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
// #ifndef _NO_CRT_STDIO_INLINE
// # define _NO_CRT_STDIO_INLINE
// #endif
#include <windows.h>
#endif

// Note: we want to to use client blocks, don't define this ...
// #define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>

#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __FUNC__
# define __FUNC__ __func__
#endif

#ifndef __FUNC__
# error 'missing __FUNC__'
#endif
#ifndef __FILE__
# error 'missing __FILE__'
#endif
#ifndef __LINE__
# error 'missing __LINE__'
#endif
#ifndef __COUNTER__
# error 'missing __COUNTER__'
#endif


// Note: suppress warnings ...
#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable:4053) // void operand for ternary expression ...
# pragma warning(disable:4706) // assignment within conditional expression

#ifdef _DEVELOPER
# pragma warning(disable:4100)
# pragma warning(disable:4201)
# pragma warning(disable:4505)
#endif

#endif


// Todo:
#ifdef _MSC_VER
#define ccf64_t double
#define ccf32_t float

#define cci64_t __int64
#define ccu64_t unsigned cci64_t

#define cci32_t __int32
#define ccu32_t unsigned cci32_t

#define cci16_t __int16
#define ccu16_t unsigned cci16_t

#define cci8_t  __int8
#define ccu8_t  unsigned cci8_t
#endif

#ifndef ccthread_local
# define ccthread_local __declspec(thread)
#endif
#ifndef ccglobal
# define ccglobal static
#endif
#ifndef ccfunc
# define ccfunc static
#endif
#ifndef ccunion
# ifdef _HARD_DEBUG
#  define ccunion struct
# else
#  define ccunion union
# endif
#endif
// Todo:
#ifndef ccinle
# ifdef __forceinline
#  define ccinle __forceinline
# else
#  define ccinle inline
# endif
#endif
#ifndef ccaddr
# define ccaddr(mem) (&(mem))
#endif
#ifndef ccdref
# define ccdref(mem) (*(mem))
#endif
#ifndef cccast
# define cccast(type,mem) ((type)(mem))
#endif

#ifndef ccnil
# define ccnil 0
#endif
#ifndef ccnit
# define ccnit 0
#endif

#ifndef cctrue
# define cctrue cccast(int,1)
#endif
#ifndef ccfalse
# define ccfalse cccast(int,0)
#endif
#ifndef ccbreak
# define ccbreak __debugbreak
#endif
#ifndef cclit
# define cclit(lit) cccast(unsigned int,sizeof(lit)-1),(lit)
#endif

#ifdef _DEBUG
# define ccassert(is_true,...) (!(is_true)?ccbreak():ccnil)
#else
# define ccassert(is_true,...) ccnil
#endif
#ifndef _CCASSERT
#define _CCASSERT(x) typedef char __STATIC__ASSERT__[((x)?1:-1)]
#endif
#ifndef ccnotnil
# define ccnotnil(val) (ccassert((val)!=ccnil),val)
#endif

ccfunc void
cctrace_(int guid, const char  *file, int line, const char *func, const char *tag, const char *fmt, ...);

# ifndef cctrace
#  define cctrace(tag,fmt,...) cctrace_(__COUNTER__,__FILE__,__LINE__,__FUNC__,tag,fmt,__VA_ARGS__)
# endif
# ifndef cctracelog
#  define cctracelog(fmt,...) cctrace("log",fmt,__VA_ARGS__)
# endif
# ifndef cctracewar
#  define cctracewar(fmt,...) cctrace("war",fmt,__VA_ARGS__)
# endif
# ifdef _DEBUG
# ifndef cctraceerr
#  define cctraceerr(fmt,...) cctrace("err",fmt,__VA_ARGS__),ccbreak()
# endif
#else
# ifndef cctrace
#  define cctrace(tag,fmt,...) 0
# endif
# ifndef cctracelog
#  define cctracelog(fmt,...) ccnil
# endif
# ifndef cctracewar
#  define cctracewar(fmt,...) ccnil
# endif
# ifndef cctraceerr
#  define cctraceerr(fmt,...) ccnil
# endif
#endif


ccfunc void *ccmalloc_ (size_t,         const char *, const char *, int);
ccfunc void *ccrealloc_(void *, size_t, const char *, const char *, int);
ccfunc void  ccfree_   (void *,         const char *, const char *, int);


#ifdef _HARD_DEBUG
# ifndef ccmalloc
#  define ccmalloc(len) ccmalloc_(len,__FILE__,__FUNC__,__LINE__)
# endif
# ifndef ccrealloc
#  define ccrealloc(mem,len) ccrealloc_(mem,len,__FILE__,__FUNC__,__LINE__)
# endif
# ifndef ccfree
#  define ccfree(mem) ccfree_(mem,__FILE__,__FUNC__,__LINE__)
# endif
#else
# ifndef ccmalloc
#  define ccmalloc(len) malloc(len)
# endif
# ifndef ccrealloc
#  define ccrealloc(mem,len) realloc(mem,len)
# endif
# ifndef ccfree
#  define ccfree(mem) free(mem)
# endif
#endif // #ifdef _HARD_DEBUG

#ifndef ccmalloc_T
# define ccmalloc_T(type) cccast(type*,ccmalloc(sizeof(type)))
#endif
#ifndef ccrealloc_T
# define ccrealloc_T(mem,type) cccast(type*,ccrealloc(mem,sizeof(type)))
#endif

#ifdef _HARD_DEBUG
ccfunc void *ccmalloc_(size_t size, const char *file, const char *func, int line)
{
  // long   reqn;
  // char * file;
  // int    line;
  // ccassert(_CrtIsMemoryBlock(mem,sze,&reqn,&file,&line))

  void *mem=_malloc_dbg(size,_CLIENT_BLOCK,file,line);
  return mem;
}

ccfunc void *ccrealloc_(void *data, size_t size, const char *file, const char *func, int line)
{
  void *mem=_realloc_dbg(data,size,_CLIENT_BLOCK,file,line);
  return mem;
}
ccfunc void ccfree_(void *data, const char *file, const char *func, int line)
{
	ccassert(_CrtIsValidHeapPointer(data));

  _free_dbg(data,_CLIENT_BLOCK);
}
#ifdef _DEVELOPER
#undef malloc
# define malloc DO_NOT_USE_MALLOC
#undef realloc
# define realloc DO_NOT_USE_REALLOC
#undef free
# define free DO_NOT_USE_FREE
#endif
#endif

_CCASSERT(sizeof(ccu64_t)==sizeof(size_t));
_CCASSERT(sizeof(cci64_t)==sizeof(size_t));

ccfunc int ccrealfile(void *);
ccfunc void ccclosefile(void *);
ccfunc void *ccopenfile(const char *);
ccfunc void *ccpullfile(void *,unsigned long int,unsigned long int *);
ccfunc unsigned long int ccpushfile(void *,unsigned long int,unsigned long int,void*);
ccfunc unsigned long int ccfilesize(void *);

ccfunc ccinle ccu64_t ccclocktick();
ccfunc ccinle ccf64_t ccclocksecs(ccu64_t);

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

typedef struct ccemit_value_t ccemit_value_t;
typedef struct ccemit_block_t ccemit_block_t;
typedef struct ccemit_procd_t ccemit_procd_t;
typedef struct ccexec_value_t ccexec_value_t;

typedef struct cctree_t cctree_t;

#include "cclog.h"
#include "ccdlb.h"
#include "ccfio.c"
#include "ccread.h"
#include "cctree.h"
#include "ccedict.h"
#include "ccemit.h"
#include "ccexec.h"

#include "cclex.c"
#include "ccread.c"
#include "ccemit.c"
#include "ccexec.c"
#include "ccemit-c.c"


#ifdef _MSC_VER
#pragma warning(pop)
#endif

#ifdef __cplusplus
}
#endif
#endif