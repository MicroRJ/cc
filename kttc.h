/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
/*****************************************************************/
#ifndef KTT_HEADER_FILE_C
#define KTT_HEADER_FILE_C

/**
 * Things to improve on are definitely speed, worst case scenario, every time you parse some expression you end up calling
 * 16 other functions.
 *
 * Also memory management, trees shouldn't have direct pointers to other trees, instead use a handle, or some offset into
 * some dynamic memory, since trees are all the same size we could employ free lists with no de-fragmentation, or reduce the size
 * of each tree by only allocating the necessary data?
 *
 * I'd also like to make the lexer as fast as possible, although it is not a priority right now.
 * #define haszero(v) (((v) - 0x01010101LL) & ~(v) & 0x80808080LL)
 * #define hasvalue(x,n) (haszero((x) ^ (~0LL/255 * (n))))
 **/


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

#ifndef ccfunc
# define ccfunc static
#endif

#ifndef ccinle
// Todo:
#ifdef __forceinline
# define ccinle __forceinline
#else
# define ccinle inline
#endif
#endif


#ifndef cccast
# define cccast(type,mem) ((type)(mem))
#endif

#ifndef ktt__global
# define ktt__global static
#endif

#ifndef ktt__nullptr
# define ktt__nullptr (0)
#endif

#ifndef cclit
# define cclit(lit) cccast(unsigned int,sizeof(lit)-1),(lit)
#endif

#ifndef ktt__true
# define ktt__true cccast(ktt_i32, 1)
#endif

#ifndef ktt__false
# define ktt__false cccast(ktt_i32, 0)
#endif

#ifndef ccbreak
# define ccbreak __debugbreak
#endif

#ifdef _DEBUG
# define ccassert(is_true) if(!(is_true)) ccbreak()
#else
# define ccassert(is_true) 0
#endif

#ifndef ccmalloc
# define ccmalloc(len) malloc(len)
#endif

#ifndef ccrealloc
# define ccrealloc(mem,len) realloc(mem,len)
#endif

#ifndef ccfree
# define ccfree(mem) free(mem)
#endif

ccfunc int ccisfile(void *);
ccfunc void ccclosefile(void *);
ccfunc void *ccopenfile(const char *);
ccfunc void *ccpullfile(void *,unsigned long int,unsigned long int *);
ccfunc unsigned long int ccpushfile(void *,unsigned long int,unsigned long int,void*);
ccfunc unsigned long int ccfilesize(void *);

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

#include "ccmem.h"
#include "cclog.h"
#include "ccfile.c"

#include "kttc-read.h"
#include "kttc-read.c"

#include "cclex.c"

#include "kttc-read-expression.c"
#include "kttc-read-declaration.c"
#include "kttc-read-statement.c"

#include "ktt-s.c"

}
#endif