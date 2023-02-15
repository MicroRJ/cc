/** Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved. **/
#ifndef _CC
#define _CC
// Note: CC stands for 'c-cures' and it is meant to be a simple set of utilities that you'd
// use on almost any c project ...
// I don't feel too comfortable yet having everything in a single file, so definitions will be here,
// and implementations in their respective files.
//

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_DEBUG)||defined(DEBUG)||defined(DBG)
# define _CCDEBUG
#endif

#define _CCGUID __COUNTER__
#define _CCFILE __FILE__
#define _CCLINE __LINE__
#define _CCFUNC __func__


// Note: if we are in development mode keep the disabled warnings for faster prototyping,
// and when we build in release mode, enable warnings to neatify code ...
#ifdef _MSC_VER
# ifndef _DEVELOPER
#  pragma warning(push)
# endif
#endif

// Note: suppress warnings ...
#ifdef _MSC_VER
# pragma warning(disable:4053)
# pragma warning(disable:4706)
#ifdef _DEVELOPER
# pragma warning(disable:4100)
# pragma warning(disable:4201)
# pragma warning(disable:4505)
# pragma warning(disable:4702)
#endif
#endif

// Todo:
#define ccthread_local __declspec(thread)
#define ccglobal       static
#define ccfunc         static

#ifdef __forceinline
# define ccinle __forceinline
#else
# define ccinle inline
#endif

#ifndef ccunion
# ifdef _HARD_DEBUG
#  define ccunion struct
# else
#  define ccunion union
# endif
#endif

typedef double ccf64_t;
typedef float  ccf32_t;

// Todo:
#if _MSC_VER
typedef signed   __int64 cci64_t;
typedef unsigned __int64 ccu64_t;
typedef signed   __int32 cci32_t;
typedef unsigned __int32 ccu32_t;
typedef signed   __int16 cci16_t;
typedef unsigned __int16 ccu16_t;
typedef signed   __int8  cci8_t;
typedef unsigned __int8  ccu8_t;
#else
// Todo:
typedef signed   long long int cci64_t;
typedef unsigned long long int ccu64_t;
#ifdef _CCLONG32
typedef signed   long int cci32_t;
typedef unsigned long int ccu32_t;
#else
typedef signed   int cci32_t;
typedef unsigned int ccu32_t;
#endif
typedef signed   short cci16_t;
typedef unsigned short ccu16_t;
typedef signed   char  cci8_t;
typedef unsigned char  ccu8_t;
#endif

#define _CCASSERT(x) typedef char __STATIC__ASSERT__[((x)?1:-1)]

_CCASSERT(sizeof(ccu64_t)==sizeof(void*));
_CCASSERT(sizeof(cci64_t)==sizeof(void*));
_CCASSERT(sizeof(ccu32_t)==4);
_CCASSERT(sizeof(cci32_t)==4);
_CCASSERT(sizeof(ccu16_t)==2);
_CCASSERT(sizeof(cci16_t)==2);

// Note: this makes easier to regex
#define cccast(T,mem) ((T)(mem))
#define ccaddr(mem)   (&(mem))
#define ccdref(mem)   (*(mem))

#define cctrue  cccast(int,1)
#define ccfalse cccast(int,0)

// Todo: hmm...
#define ccnil  0
#define ccnull 0

#ifndef ccbreak
# define ccbreak __debugbreak
#endif

// Todo: enhance these assertions ...
#ifndef ccassert
# ifdef _DEBUG
#  define ccassert(is_true,...) (!(is_true)?ccbreak():ccnil)
# else
#  define ccassert(is_true,...) ccnil
# endif
#endif

// Todo: remove this ...
#ifndef ccnotnil
# ifdef _DEBUG
#  define ccnotnil(val) (ccassert((val)!=ccnil),val)
# else
#  define ccnotnil(val) (val)
# endif
#endif

// Note:
typedef ccu64_t ccclocktime_t;

// Note: This is a tag type for when you want to make it abundantly clear
// that there's metadata associated with the string ..
typedef char *ccstr_t;

// Note: used for enhancing debug data ...
typedef struct cccaller_t cccaller_t;
typedef struct cccaller_t
{ int           guid;
  const char   *file;
  int           line;
  const char   *func;
} cccaller_t;

// Note: common error codes ... let's try to keep this conservative, I don't want to keep promoting
// the use of global status codes ... these are the ones absolutely necessary as of now ...
typedef enum ccerr_k
{
  ccerr_kNON=0,
  ccerr_kNIT=1,
  ccerr_kAIT=2,
  ccerr_kOOM=3,
  ccerr_kIUA=4,
} ccerr_k;

// Note: simple allocator function, must-have for switching the allocator of debug routines ...
// Todo: Do not take cccaller_t on the stack, make it a global or something, much easier to
// turn off in release-mode...
typedef void *(ccallocator_t)(cccaller_t, size_t,void *);

// Note: entry status, this also must be a global ...
typedef enum ccent_k
{
  ccent_kNIT=1,
  ccent_kAIT=2,
} ccent_k;

// Note: general table entry, ideally allocated with no overhead ... but that is not the case yet ...
typedef struct ccentry_t ccentry_t;
typedef struct ccentry_t
{ ccentry_t * nex;
  cci32_t     len;
  char      * key;
  size_t      val;
} ccentry_t;

// Note: dynamic length buffer ...
// Todo: At the moment, not very well integrated ...
typedef struct ccdlb_t ccdlb_t;
typedef struct ccdlb_t
{ unsigned        rem_add: 1;
  unsigned        rem_rze: 1;
  ccallocator_t * allocator;
  ccentry_t     * entries;
  size_t          sze_max;
  size_t          sze_min;
} ccdlb_t;

typedef struct ccsentry_t ccsentry_t;

// Note: allocator is just to determine whether we're using the right allocator ...
typedef struct ccsentry_block_t ccsentry_block_t;
typedef struct ccsentry_block_t
{ char head_guard[4];

  // Note: the current sentry that has this block ...
  ccsentry_t *sentry;
  // Note: the original sentry that created this block ...
  ccsentry_t *master;

  ccallocator_t    * allocator;
  ccsentry_block_t * prev, * next;
  cccaller_t         caller;
  size_t             size;
  char tail_guard[4];
} ccsentry_block_t;

typedef struct ccsentry_t ccsentry_t;
typedef struct ccsentry_t
{
  // Note: some of these names will get me in trouble one day ...
  const char  * marker;
  ccsentry_t  * master;
  ccsentry_t  * slaves;

  cci32_t       level;

  cccaller_t   caller;

  // Note: this is the last block ...
  ccsentry_block_t * block_list;

  // Todo: it would be interesting if we'd also record child enter and leave count, this way
  // we would also detect when a child hasn't properly left...
  cci32_t enter_count;
  cci32_t leave_count;

  // Note: this has to be every time we enter ...
  ccclocktime_t start_time_in_ticks;
  ccclocktime_t total_time_in_ticks;

  struct
  { cci64_t pma;
    cci64_t nma;
    cci64_t pm;
    cci64_t nm;
    cci32_t pbc;
    cci32_t nbc;
  } last_metrics, metrics;
} ccsentry_t;


ccfunc void *ccuserallocator_(cccaller_t,size_t,void*);
ccfunc void *ccinternalallocator_(cccaller_t,size_t,void*);

// Note:
ccglobal ccthread_local ccclocktime_t ccclockfreqc;

ccglobal ccthread_local ccerr_k ccerr;
ccglobal ccthread_local ccstr_t cckey;

// Todo: allocator stack or remove ...
ccglobal ccthread_local ccallocator_t *ccallocator=ccuserallocator_;

#ifdef _HARD_DEBUG
ccglobal ccthread_local ccsentry_t   ccdebugroot;
ccglobal ccthread_local ccsentry_t  *ccdebugthis;
ccglobal ccthread_local cci32_t      ccdebugnone;
#endif

#define ccstrlenS(cstr) cccast(ccu32_t,strlen(cstr)-0)
#define ccstrlenL(lstr) cccast(ccu32_t,sizeof(lstr)-1)

// Note: global error
#define ccerrset(err) ((ccerr=err),0)
#define ccerrnon()    ((ccerr)==ccerr_kNON)
#define ccerrsom()    ((ccerr)!=ccerr_kNON)
#define ccerrnit()    ((ccerr)==ccerr_kNIT)
#define ccerrait()    ((ccerr)==ccerr_kAIT)

// Note: global key
#define cckeyset(key) (cckey=key)
#define cckeyget()    (cckey)

// Note: l-value address
#define cclva(lv) cccast(void**,ccaddr(lv))

// Note: default allocator

#define ccmalloc(size)       ccuserallocator_(cccall(),size,ccnil)
#define ccrealloc(data,size) ccuserallocator_(cccall(),size, data)
#define ccfree(data)         ccuserallocator_(cccall(),ccnil,data)

#define ccmalloc_T(T) cccast(T*,ccmalloc(sizeof(T)))

// Note: dynamic length buffer
#define ccdlb_(ccm) (cccast(ccdlb_t*,ccm)-1)
#define ccdlb(ccm) ((ccm)?ccdlb_(ccm):(ccnil))
#define ccdlbmax_(ccm) (ccdlb_(ccm)->sze_max)
#define ccdlbmin_(ccm) (ccdlb_(ccm)->sze_min)
#define ccdlbmax(ccm) ((ccm)?ccdlbmax_(ccm):(ccnil))
#define ccdlbmin(ccm) ((ccm)?ccdlbmin_(ccm):(ccnil))
#define ccdlbdel(ccm) ccdlbdel_(cclva(ccm))
// Note: array
#define ccarrdel ccdlbdel

#define ccarrmax(arr) ((arr)?(ccdlbmax_(arr)/sizeof(*(arr))):(ccnil))
#define ccarrmin(arr) ((arr)?(ccdlbmin_(arr)/sizeof(*(arr))):(ccnil))

#define ccarrend_max(arr) ((arr)+ccarrmax(arr))
#define ccarrend_min(arr) ((arr)+ccarrmin(arr))

#define ccarrend ccarrend_min
#define ccarrlen(arr) cccast(ccu32_t,ccarrmin(arr))

#define ccarrresi(ccm,num) ccdlb_arradd(cclva(ccm),sizeof(*(ccm)),num,0)
#define ccarrres(ccm,num) ((ccm)+ccarrresi(ccm,num))

#define ccarraddi(ccm,num) ccdlb_arradd(cclva(ccm),sizeof(*(ccm)),num,num)
#define ccarradd(ccm,num) ((ccm)+ccarraddi(ccm,num))

#define ccarrone(ccm) ccarradd(ccm,1)
#define ccarrzro(ccm) memset(ccm,ccnil,ccdlbmax(ccm))

#define ccarrfor(arr,itr) for(itr=arr;itr<ccarrend(arr);++itr)

// Todo:
#define ccarrfix(ccm) ((ccm)?ccdlb_(ccm)->rem_rze=cctrue:(ccnil))

// Note: table
#define cclithsh(lit) ccstrlenL(lit),lit
#define ccntshsh(nts) ccstrlenS(nts),nts
#define ccptrhsh(ptr) -cccast(cci32_t,sizeof(ptr)),cccast(char*,ptr)

// Note: these use the global error code _ccerr_
ccfunc cci64_t ccdlb_tblget(void **, cci32_t, cci32_t, const char *);
ccfunc cci64_t ccdlb_tblput(void **, cci32_t, cci32_t, const char *);
ccfunc cci64_t ccdlb_tblset(void **, cci32_t, cci32_t, const char *);

// Todo: this is to be reworked ...
#define cctblgetL(ccm,lit) ((ccm)+ccdlb_tblget(cclva(ccm),sizeof(*(ccm)),cclithsh(lit)))
#define cctblgetS(ccm,nts) ((ccm)+ccdlb_tblget(cclva(ccm),sizeof(*(ccm)),ccntshsh(nts)))
#define cctblgetP(ccm,ptr) ((ccm)+ccdlb_tblget(cclva(ccm),sizeof(*(ccm)),ccptrhsh(ptr)))

#define cctblset(ccm,len,key) ((ccm)+ccdlb_tblset(cclva(ccm),sizeof(*ccm),len,key))

#define cctblsetL(ccm,lit) ((ccm)+ccdlb_tblset(cclva(ccm),sizeof(*ccm),cclithsh(lit)))
#define cctblsetS(ccm,nts) ((ccm)+ccdlb_tblset(cclva(ccm),sizeof(*ccm),ccntshsh(nts)))
#define cctblsetP(ccm,ptr) ((ccm)+ccdlb_tblset(cclva(ccm),sizeof(*ccm),ccptrhsh(ptr)))

#define cctblputL(ccm,lit) ((ccm)+ccdlb_tblput(cclva(ccm),sizeof(*ccm),cclithsh(lit)))
#define cctblputS(ccm,nts) ((ccm)+ccdlb_tblput(cclva(ccm),sizeof(*ccm),ccntshsh(nts)))
#define cctblputP(ccm,ptr) ((ccm)+ccdlb_tblput(cclva(ccm),sizeof(*ccm),ccptrhsh(ptr)))

// Note: string builder
#define ccstrdel ccarrdel
#define ccstrmax ccarrmax
#define ccstrmin ccarrmin
#define ccstrlen ccarrlen
#define ccstradd(ccm,res,com)  ((ccm)+ccdlb_arradd(cclva(ccm),sizeof(*(ccm)),res,com))
#define ccstrcatN(ccm,len,str) ((ccm)+ccdlb_stradd(&(ccm),len+1,len+0,str))
#define ccstrputN(ccm,len,str) ((ccm)+ccdlb_stradd(&(ccm),len+1,len+1,str))
#define ccstrcatL(ccm,str) ccstrcatN(ccm,ccstrlenL(str),str)
#define ccstrputL(ccm,str) ccstrputN(ccm,ccstrlenL(str),str)
#define ccstrcatS(ccm,str) ccstrcatN(ccm,ccstrlenS(str),str)
#define ccstrputS(ccm,str) ccstrputN(ccm,ccstrlenS(str),str)
#define ccstrcatF(ccm,fmt,...) ccstr_catf(&ccm,fmt,__VA_ARGS__)

// Todo: to be removed !
ccfunc ccinle ccsentry_t *ccdebug_();
ccfunc ccinle cccaller_t cccaller(int guid, const char *file, int line, const char *func);

#define cccall()  cccaller(__COUNTER__,_CCFILE,_CCLINE,_CCFUNC)
#define ccdebug() ccdebug_()

ccfunc ccsentry_t *
ccsentry_enter(cccaller_t caller, ccsentry_t *master, const char *marker);
ccfunc ccsentry_t *
ccsentry_leave(cccaller_t caller, ccsentry_t *sentry, const char *marker);

#ifndef ccenter
# define ccenter(marker) (ccdebugthis=ccsentry_enter(cccall(),ccdebugthis,marker))
#endif
#ifndef ccleave
# define ccleave(marker) (ccdebugthis=ccsentry_leave(cccall(),ccdebugthis,marker))
#endif

#include <stdarg.h>
ccfunc ccinle ccclocktime_t ccclocktick();
ccfunc ccinle ccf64_t ccclocksecs(ccu64_t);

ccfunc const char *ccfilename(const char *name);

ccfunc int ccrealfile(void *);
ccfunc void ccclosefile(void *);
ccfunc void *ccopenfile(const char *);
ccfunc void *ccpullfile(void *,unsigned long int,unsigned long int *);
ccfunc unsigned long int ccpushfile(void *,unsigned long int,unsigned long int,void*);
ccfunc unsigned long int ccfilesize(void *);

ccfunc ccinle int ccformatvex(char *,int,const char *,va_list);
ccfunc ccinle char *ccformatv(const char *,va_list);
ccfunc int ccformatex(char *, int, const char *, ...);
ccfunc char *ccformat(const char *, ...);

ccfunc void cctrace_(cccaller_t caller, const char *label, const char *format, ...);
#define cctracelog(fmt,...) 0
#define cctracewar(fmt,...) 0
#define cctraceerr(fmt,...) 0

// cctrace_(cccall(),"log",fmt,__VA_ARGS__)
// cctrace_(cccall(),"war",fmt,__VA_ARGS__)
// cctrace_(cccall(),"err",fmt,__VA_ARGS__)

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

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

ccfunc ccinle int
ccformatvex(char *buf, int len, const char * fmt, va_list vli)
{
  return stbsp_vsnprintf(buf,len,fmt,vli);
}

ccfunc ccinle char *
ccformatv(const char * fmt, va_list vli)
{
  ccglobal ccthread_local char buf[0xff];

  ccformatvex(buf,0xff,fmt,vli);

  return buf;
}

ccfunc int
ccformatex(char *buf, int len, const char * fmt, ...)
{
  va_list vli;
  va_start(vli,fmt);
  int res=ccformatvex(buf,len,fmt,vli);
  va_end(vli);

  return res;
}

ccfunc char *
ccformat(const char * fmt, ...)
{
  va_list vli;
  va_start(vli,fmt);
  char *res=ccformatv(fmt,vli);
  va_end(vli);

  return res;
}

typedef struct ccedict_t ccedict_t;
typedef struct ccread_t  ccread_t;
typedef struct ccemit_t  ccemit_t;
typedef struct ccexec_t  ccexec_t;

typedef struct ccemit_value_t ccemit_value_t;
typedef struct ccemit_block_t ccemit_block_t;
typedef struct ccemit_procd_t ccemit_procd_t;
typedef struct ccexec_value_t ccexec_value_t;

typedef struct ccjump_point_t ccjump_point_t;

typedef struct cctype_t cctype_t;
typedef struct cctree_t cctree_t;

#include "ccfio.c"

ccfunc void
ccdebug_checkblock(ccallocator_t *allocator, ccsentry_block_t *block);

#include "cclog.h"
#include "ccdlb.h"
#define _CCLOG_IMPL
#include "cclog.h"

ccfunc void *ccinternalallocator_(cccaller_t caller, size_t size,void *data)
{ if(size)
  { if(data)
      return realloc(data,size);
    else
      return malloc(size);
  }
  free(data);
  return ccnil;
}

#ifdef _HARD_DEBUG

ccfunc void
ccdebug_checkblock(ccallocator_t *allocator, ccsentry_block_t *block)
{ for(int i=0; i<4; ++i)
    ccassert(block->head_guard[i]==ccnil);
  for(int i=0; i<4; ++i)
    ccassert(block->tail_guard[i]==ccnil);

  ccassert(block->master!=ccnil);
  ccassert(block->sentry!=ccnil);

  ccassert(block->allocator==allocator);

  ccassert(!block->prev||block->prev->sentry==block->sentry);
  ccassert(!block->next||block->next->sentry==block->sentry);

  ccassert(!block->prev||block->prev->next==block);
  ccassert(!block->next||block->next->prev==block);

  ccassert(!block->next||block->next!=block);
  ccassert(!block->prev||block->prev!=block);
}

ccfunc void *ccuserallocator_(cccaller_t caller, size_t size,void *data)
{
ccenter("user-allocator");
  ccsentry_block_t *block;
  if(!size)
  {
ccenter("free");
    ccsentry_t *debug=ccdebug();
    block=(ccsentry_block_t*)data-1;
    ccdebug_checkblock(ccuserallocator_,block);

    if(block->prev) block->prev->next=block->next;
    if(block->next) block->next->prev=block->prev;
    else block->sentry->block_list=block->prev;

    debug->metrics.nbc++;

    debug->metrics.nma++;
    debug->metrics.nm+=block->size;

    // Note: just in case?
    block->next=ccnil;
    block->prev=ccnil;

    free(block);
    block=ccnil;
ccleave("free");
  } else
  { if(data)
    {
ccenter("ccrealloc");
      ccsentry_t *debug=ccdebug();
      block=(ccsentry_block_t*)data-1;
      ccdebug_checkblock(ccuserallocator_,block);

      debug->metrics.nm+=block->size;

      block=(ccsentry_block_t*)realloc(block,sizeof(*block)+size);
      block->caller=caller;
      block->size=size;

      if(block->prev) block->prev->next=block;
      if(block->next) block->next->prev=block;
      else block->sentry->block_list=block;

      debug->metrics.pma++;
      debug->metrics.nma++;
      debug->metrics.pm+=size;

ccleave("ccrealloc");
    } else
    {
ccenter("malloc");
      ccsentry_t *debug=ccdebug();
      block=(ccsentry_block_t*)malloc(sizeof(*block)+size);
      memset(block,ccnil,sizeof(*block));
      block->allocator=ccuserallocator_;
      block->caller=caller;
      block->master=debug;
      block->sentry=debug;
      block->size=size;
      ccsentry_addblock(debug,block);

      ccdebug_checkblock(ccuserallocator_,block);

      debug->metrics.pbc++;
      debug->metrics.pma++;
      debug->metrics.pm+=size;
ccleave("malloc");
    }
  }
ccleave("user-allocator");
  return block+1;
}
#ifdef _DEVELOPER
#undef malloc
# define malloc DO_NOT_USE_MALLOC
#undef realloc
# define realloc DO_NOT_USE_REALLOC
#undef free
# define free DO_NOT_USE_FREE
#endif
#else
ccfunc void *ccuserallocator_(size_t size,void *data)
{
  return ccinternalallocator_(size,data);
}
#endif

// Note:
#include "ccread.h"
#include "cctree.h"
#include "ccseek.h"

#include "ccedict.h"

#include "ccseek.c"
#include "ccemit.h"
#include "ccexec.h"

#include "cclex.c"
#include "ccread.c"
#include "ccemit.c"
#include "ccexec.c"
// #include "ccemit-c.c"

#ifdef _MSC_VER
# ifndef _DEVELOPER
#  pragma warning(pop)
# endif
#endif

#ifdef __cplusplus
}
#endif
#endif