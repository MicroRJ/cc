/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
/*****************************************************************/
#ifndef KTT_HEADER_FILE_C
#define KTT_HEADER_FILE_C



// Todo: speed, as always ...

// annoying
#ifdef _DEBUG
# pragma warning(disable:4100)
#endif

#ifdef _DEBUG
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
# include <windows.h>
#endif
#endif

#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>

extern "C"
{

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


// Todo: MSVC specific
#define cci64 __int64
#define cci32 __int32
#define cci16 __int16
#define cci8  __int8

#define ccu64 unsigned cci64
#define ccu32 unsigned cci32
#define ccu16 unsigned cci16
#define ccu8  unsigned cci8


#ifdef _MSC_VER
# pragma warning(disable:4505) // un-referenced function with internal usage
# pragma warning(disable:4706) // assignment within conditional expression

# define ktt_i64  signed __int64
# define ktt_i32  signed __int32
# define ktt_i16  signed __int16
# define ktt_i8   signed __int8
# define ktt_u64  unsigned __int64
# define ktt_u32  unsigned __int32
# define ktt_u16  unsigned __int16
# define ktt_u8   unsigned __int8
# define ktt_f64  double
# define ktt_f32  float
# define ktt_c8   char
# define ktt_i64_32 ktt_i64
# define ktt_u64_32 ktt_u64
# define ktt_f64_32 ktt_f64
#endif

#ifndef ccglobal
# define ccglobal static
#endif
#ifndef ccfunc
# define ccfunc static
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
# define ccnil cccast(int,0)
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
# define ccassert(is_true) (!(is_true)?ccbreak():ccnil)
#else
# define ccassert(is_true) 0
#endif
#ifndef _CCASSERT
#define _CCASSERT(x) typedef char __STATIC__ASSERT__[((x)?1:-1)]
#endif
#ifndef ccnotnil
# define ccnotnil(val) (ccassert((val)!=ccnil),val)
#endif
#ifndef ccmalloc
# define ccmalloc(len) malloc(len)
#endif
#ifndef ccrealloc
# define ccrealloc(mem,len) realloc(mem,len)
#endif
#ifndef ccmalloc_T
# define ccmalloc_T(type) cccast(type*,ccmalloc(sizeof(type)))
#endif
#ifndef ccrealloc_T
# define ccrealloc_T(mem,type) cccast(type*,ccrealloc(mem,sizeof(type)))
#endif
#ifndef ccfree
# define ccfree(mem) free(mem)
#endif

// Todo: use size_t instead ...
_CCASSERT(sizeof(long long int)==sizeof(size_t));


ccfunc int  ccrealfile(void *);
ccfunc void ccclosefile(void *);
ccfunc void *ccopenfile(const char *);
ccfunc void *ccpullfile(void *,unsigned long int,unsigned long int *);
ccfunc unsigned long int ccpushfile(void *,unsigned long int,unsigned long int,void*);
ccfunc unsigned long int ccfilesize(void *);

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

#include "cclog.h"
#include "ccdlb.h"

#include "ccread.h"

#include "ccfio.c"
#include "cclex.c"
#include "ccreadexpr.c"
#include "ccreaddecl.c"
#include "ccreadstat.c"
#include "ccread.c"
#include "ktt-s.c"
#include "ccemit.c"
#include "ccemit-c.c"
#include "ccexec.c"
}
#endif