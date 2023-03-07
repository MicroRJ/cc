//
// Copyright (c) 2022 Dayan Rodriguez
//
// cc | c-cures v1.0 | 2022,2023
// https://github.com/MicroRJ/cc
//
// Public domain stb style c utilities:
// - dynamic length buffer with builtin hash-table
// - memory checker with leak detection
// - timing utilities
// - introspection utilities
//
// Still piecing everything together, trying to not add
// too many things here, I don't want this ever to become a huge file.
#ifndef _CC_C
#define _CC_C 1

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <malloc.h>
#include <memory.h>
#include <string.h>

#include <immintrin.h>
#include <emmintrin.h>
#include <intrin.h>

#include <math.h>

// -- Todo:
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

// -- Todo:
#ifdef _MSC_VER
# ifndef _DEVELOPER
#  pragma warning(push)
# endif
#endif

#ifdef _MSC_VER
# pragma warning(disable:4018)
# pragma warning(disable:4101)
# pragma warning(disable:4201)
# pragma warning(disable:4311)
# pragma warning(disable:4312)
# pragma warning(disable:4302)
# pragma warning(disable:4706)
#ifdef _DEVELOPER
# pragma warning(disable:4189)
# pragma warning(disable:4100)
# pragma warning(disable:4505)
# pragma warning(disable:4702)
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_DEBUG) || defined(DEBUG) || defined(DBG)
# define _CCDEBUG
#endif

#define _CCGUID __COUNTER__
#define _CCFILE __FILE__
#define _CCLINE __LINE__
#define _CCFUNC __func__

// Todo:
#define ccthread_local __declspec(thread)
#define ccglobal       static
#define ccfunc         static

// Todo:
#ifdef __forceinline
# define ccinle __forceinline
#else
# define ccinle inline
#endif

// Todo: remove!
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
#ifdef _WIN32
typedef signed   long int cci32_t;
typedef unsigned long int ccu32_t;
#else
typedef signed   int cci32_t;
typedef unsigned int ccu32_t;
#endif

// Todo:
#if _MSC_VER
typedef signed   __int64 cci64_t;
typedef unsigned __int64 ccu64_t;

typedef signed   __int16 cci16_t;
typedef unsigned __int16 ccu16_t;
typedef signed   __int8  cci8_t;
typedef unsigned __int8  ccu8_t;
#else
// Todo:
typedef signed   long long int cci64_t;
typedef unsigned long long int ccu64_t;

typedef signed   short cci16_t;
typedef unsigned short ccu16_t;
typedef signed   char  cci8_t;
typedef unsigned char  ccu8_t;
#endif

#ifndef _CCASSERT
# define _CCASSERT(x) typedef char __CCASSERT__[((x)?1:-1)]
#endif

#define CCWITHIN(x,l,r) (((x)>=(l))&&((x)<=(r)))

_CCASSERT(sizeof(ccu64_t)==sizeof(void*));
_CCASSERT(sizeof(cci64_t)==sizeof(void*));
_CCASSERT(sizeof(ccu32_t)==4);
_CCASSERT(sizeof(cci32_t)==4);
_CCASSERT(sizeof(ccu16_t)==2);
_CCASSERT(sizeof(cci16_t)==2);

typedef void **ccvala_t;

#define cccast(T,mem) ((T)(mem))
#define ccvala(v) cccast(ccvala_t,&(v))

// Todo: remove these!
#define ccaddr(mem)   (&(mem))
#define ccdref(mem)   (*(mem))

// Note: c array literal length
#ifndef ccCarrlenL
# define ccCarrlenL(carr) (sizeof(carr)/sizeof(carr[0]))
#endif

// Note: c string literal length
#ifndef ccCstrlenL
# define ccCstrlenL(clit) cccast(ccu32_t,ccCarrlenL(clit)-1)
#endif

// Note: c string length
#ifndef ccCstrlenS
# define ccCstrlenS(cstr) cccast(ccu32_t,strlen(cstr)-0)
#endif

// Note: some necessary error codes ...
typedef enum ccerr_k
{
  ccerr_kNON=0,
  ccerr_kNIT,
  ccerr_kAIT,
} ccerr_k;

ccglobal const char *ccerr_s[]=
{ "none",
  "not in table",
  "already in table",
};

typedef ccu64_t ccclocktick_t;

typedef struct cccaller_t cccaller_t;
typedef struct cccaller_t
{ int           guid;
  const char   *file;
  int           line;
  const char   *func;
} cccaller_t;

typedef void *(ccallocator_t)(cccaller_t caller, size_t,void *);
ccfunc void *ccuserallocator_(cccaller_t,size_t,void*);
ccfunc void *ccinternalallocator_(cccaller_t,size_t,void*);

typedef struct ccentry_t ccentry_t;
typedef struct ccentry_t
{ cccaller_t  caller;
  ccentry_t * nex;
  cci32_t     len;
  char      * key;
  cci64_t     val;
} ccentry_t;

#define ccarray(T) T *
#define cctable(T) T *

typedef char *ccstr_t;

typedef struct ccdlb_t ccdlb_t;
typedef struct ccdlb_t
{ unsigned        rem_add: 1;
  unsigned        rem_rze: 1;
  ccallocator_t * allocator;
  ccentry_t     * entries;
  cci64_t         sze_max;
  cci64_t         sze_min;
} ccdlb_t;

typedef struct ccward_t ccward_t;
typedef struct ccward_t
{ char head_guard[4];
  cccaller_t         caller;
  struct ccsentry_t *master;
  struct ccsentry_t *sentry;
  ccallocator_t    * allocator;
  ccward_t * prev, * next;
  cci64_t            size;
  char tail_guard[4];
} ccward_t;

typedef struct ccsentry_t ccsentry_t;
typedef struct ccsentry_t
{ const char   * marker;
  ccsentry_t   * master;
  ccsentry_t   * slaves;
  cci32_t        level;
  cccaller_t     caller;
  ccward_t     * block_list;
  cci32_t        enter_count;
  cci32_t        leave_count;
  ccclocktick_t  start_time_in_ticks;
  ccclocktick_t  total_time_in_ticks;
  struct
  { cci64_t pma;
    cci64_t nma;
    cci64_t pm;
    cci64_t nm;
    cci32_t pbc;
    cci32_t nbc;
  } last_metrics, metrics;
} ccsentry_t;

// -- Todo: broaden this, should be console state or something!
typedef struct cccolorstate_t cccolorstate_t;
typedef struct cccolorstate_t
{ cci16_t  s[0x10];
  cci16_t  i;
  cci16_t  v;
} cccolorstate_t;

#define cctrue  cccast(cci32_t,1)
#define ccfalse cccast(cci32_t,0)

// Todo: hmm...
#define ccnil  0
#define ccnull 0

#ifndef ccbreak
# define ccbreak __debugbreak
#endif

// Todo: enhance these assertions? ...
#ifndef ccassert
# ifdef _DEBUG
#  define ccassert(is_true,...) do{ if(!(is_true)) { cctraceerr("assertion triggered",0); ccbreak(); } } while(0)
# else
#  define ccassert(is_true,...) ccnil
# endif
#endif

// Note:
ccfunc ccinle cccaller_t cccaller(int guid, const char *file, int line, const char *func);
#define cccall() (cccaller(__COUNTER__,_CCFILE,_CCLINE,_CCFUNC))

// Todo:
ccglobal ccthread_local cccolorstate_t cccolorstate;
#define cccolormove( x )  cccolorstate.v=x
#define cccolorpush(   )  cccolorstate.s[cccolorstate.i++]=cccolorstate.v
#define cccolorload(   )  cccolorstate.v=cccolorstate.s[--cccolorstate.i]

// Todo: allocator stack or remove ...
ccglobal ccthread_local ccallocator_t *ccallocator=ccuserallocator_;

// Todo:
ccglobal ccthread_local ccsentry_t   ccdebugroot;
ccglobal ccthread_local ccsentry_t  *ccdebugthis;
ccglobal ccthread_local cci32_t      ccdebugnone;

// Note: default allocator
#define ccmalloc(size)       (ccuserallocator_(cccall(),size,ccnull))
#define ccrealloc(data,size) (ccuserallocator_(cccall(),size,data))
#define ccfree(data)         (ccuserallocator_(cccall(),ccnil,data))
#define ccmalloc_T(T) cccast(T*,ccmalloc(sizeof(T)))

// -- Note: dynamic length buffer and hash-table utilities

ccglobal ccthread_local ccerr_k    ccerr;
ccglobal ccthread_local ccentry_t *ccentry;

#define ccerrstr()    (ccerr_s[ccerr])
#define ccerrset(val) ((ccerr=val),0)
#define ccerrnon()    ((ccerr)==ccerr_kNON)
#define cckeyget()    ((ccentry)?(ccentry->key):ccnull)

// -- Todo: figure out how we're going to do the caller thing...

ccfunc void    ccdlbdel_(void **ccm);
// -- Note:
ccfunc cci64_t ccdlbadd_(void **, cci32_t item_size, cci64_t reserve_size, cci64_t commit_size);
// -- Note:
ccfunc int     cctblhsh_(cccaller_t, void **, cci32_t bit, int len, char *key, int create_always);
// -- Note:
ccfunc cci64_t cctblgeti_(cccaller_t, void **, cci32_t item_size, cci32_t key_length, char *key);
// -- Note:
ccfunc cci64_t cctblputi_(cccaller_t, void **, cci32_t item_size, cci32_t key_length, char *key);
// -- Note:
ccfunc cci64_t cctblseti_(cccaller_t, void **, cci32_t item_size, cci32_t key_length, char *key);
// -- Note:
ccfunc cci64_t ccstraddi_(char **, cci64_t reserve_size, cci64_t commit_size, const char *);
// -- Note:
ccfunc cci64_t ccstrcatiV_(char **, const char *, va_list);
// -- Note:
ccfunc cci64_t ccstrcatiF_(char **, const char *, ...);
// -- Note:

#define ccdlb_(ccm) (cccast(ccdlb_t*,ccm)-1)
#define ccdlbmax_(ccm) (ccdlb_(ccm)->sze_max)
#define ccdlbmin_(ccm) (ccdlb_(ccm)->sze_min)

#define ccdlb(ccm) ((ccm)?ccdlb_(ccm):(ccnull))
#define ccdlbmax(ccm) ((ccm)?ccdlbmax_(ccm):(ccnull))
#define ccdlbmin(ccm) ((ccm)?ccdlbmin_(ccm):(ccnull))

#define ccdlbdel(ccm) ccdlbdel_(ccvala(ccm))

#define ccarrdel ccdlbdel

#define ccarrmax(arr) ((arr)?ccdlbmax_(arr):0)
#define ccarrmin(arr) ((arr)?ccdlbmin_(arr):0)
#define ccarrlen(arr) cccast(ccu32_t,ccarrmin(arr))

#define ccarrend_max(arr) ((arr)+ccarrmax(arr))
#define ccarrend_min(arr) ((arr)+ccarrmin(arr))
#define ccarrend ccarrend_min

#define ccarrzro_max(arr) memset(arr,0,ccarrmax(arr)*sizeof(*arr))
#define ccarrzro_min(arr) memset(arr,0,ccarrmin(arr)*sizeof(*arr))
#define ccarrzro ccarrzro_min

#define ccarrresi(arr,num) ccdlbadd_(ccvala(arr),sizeof(*(arr)),num,0)
#define ccarrres(arr,num) ((arr)+ccarrresi(arr,num))

#define ccarraddi(arr,num) ccdlbadd_(ccvala(arr),sizeof(*(arr)),num,num)
#define ccarradd(arr,num) ((arr)+ccarraddi(arr,num))

#define ccarrfor(arr,itr) for(itr=arr;itr<ccarrend(arr);++itr)

// Todo: remove?
#define ccarrfix(arr) ((arr)?ccdlb_(arr)->rem_rze=cctrue:(ccnull))

// Note: table
#define cclithsh(lit) ccCstrlenL(lit),cccast(char*,lit)
#define ccntshsh(nts) ccCstrlenS(nts),cccast(char*,nts)
#define ccinthsh(ptr) -cccast(cci32_t,sizeof(ptr)),cccast(char*,ptr)

// Note: these use the global error code _ccerr_
ccfunc cci64_t cctblgeti(void **, cci32_t, cci32_t, char *);
ccfunc cci64_t cctblputi(void **, cci32_t, cci32_t, char *);
ccfunc cci64_t cctblseti(void **, cci32_t, cci32_t, char *);

#define cctblgeti(...) cctblgeti_(cccall(),__VA_ARGS__)
#define cctblputi(...) cctblputi_(cccall(),__VA_ARGS__)
#define cctblseti(...) cctblseti_(cccall(),__VA_ARGS__)

// -- Todo: these are to be reworked?
#define cctblgetL(tbl,lit) ((tbl)+cctblgeti(ccvala(tbl),sizeof(*(tbl)),cclithsh(lit)))
#define cctblgetS(tbl,nts) ((tbl)+cctblgeti(ccvala(tbl),sizeof(*(tbl)),ccntshsh(nts)))
#define cctblgetP(tbl,ptr) ((tbl)+cctblgeti(ccvala(tbl),sizeof(*(tbl)),ccinthsh(ptr)))

#define cctblsetN(tbl,len,key) ((tbl)+cctblseti(ccvala(tbl),sizeof(*tbl),len,cccast(char*,key)))
#define cctblsetL(tbl,lit) ((tbl)+cctblseti(ccvala(tbl),sizeof(*tbl),cclithsh(lit)))
#define cctblsetS(tbl,nts) ((tbl)+cctblseti(ccvala(tbl),sizeof(*tbl),ccntshsh(nts)))
#define cctblsetP(tbl,ptr) ((tbl)+cctblseti(ccvala(tbl),sizeof(*tbl),ccinthsh(ptr)))

#define cctblputN(tbl,len,key) ((tbl)+cctblputi(ccvala(tbl),sizeof(*tbl),len,cccast(char*,key)))
#define cctblputL(tbl,lit) ((tbl)+cctblputi(ccvala(tbl),sizeof(*tbl),cclithsh(lit)))
#define cctblputS(tbl,nts) ((tbl)+cctblputi(ccvala(tbl),sizeof(*tbl),ccntshsh(nts)))
#define cctblputP(tbl,ptr) ((tbl)+cctblputi(ccvala(tbl),sizeof(*tbl),ccinthsh(ptr)))

// Note: string builder
#define ccstrdel ccarrdel
#define ccstrmax ccarrmax
#define ccstrmin ccarrmin
#define ccstrlen ccarrlen

#define ccstraddN(arr,res,com) ((arr)+ccdlbadd_(ccvala(arr),sizeof(*(arr)),res,com))

#define ccstrcatN(arr,len,str) ((arr)+ccstraddi_(&(arr),len+1,len+0,str))
#define ccstrcatL(arr,str) ccstrcatN(arr,ccCstrlenL(str),str)
#define ccstrcatS(arr,str) ccstrcatN(arr,ccCstrlenS(str),str)

#define ccstrputN(arr,len,str) ((arr)+ccstraddi_(&(arr),len+1,len+1,str))
#define ccstrputL(arr,str) ccstrputN(arr,ccCstrlenL(str),str)
#define ccstrputS(arr,str) ccstrputN(arr,ccCstrlenS(str),str)

#define ccstrcatF(arr,fmt,...) ccstrcatiF_(&arr,fmt,__VA_ARGS__)

ccfunc ccinle int   ccformatvex (char *,int,const char *,va_list);
ccfunc ccinle char *ccformatv   (const char *, va_list);
ccfunc        int   ccformatex  (char *, int, const char *, ...);
ccfunc        char *ccformat    (const char *, ...);

ccfunc ccinle ccclocktick_t ccclocktick();
ccfunc ccinle ccf64_t       ccclocksecs(ccu64_t);

ccfunc void *   ccopenfile (const char *, const char *);
ccfunc int      ccrealfile ( void * );
ccfunc void     ccclosefile( void * );
ccfunc void   * ccpullfile ( void *, ccu32_t, ccu32_t *     );
ccfunc ccu32_t  ccpushfile ( void *, ccu32_t, ccu32_t,void *);
ccfunc ccu32_t  ccfilesize ( void * );

// Todo: to be removed !
ccfunc ccinle ccsentry_t *ccdebug_();

// Todo: remove!
#define ccdebug() ccdebug_()

ccfunc ccsentry_t * ccsentry_enter(cccaller_t caller, ccsentry_t *master, const char *marker);
ccfunc ccsentry_t * ccsentry_leave(cccaller_t caller, ccsentry_t *sentry, const char *marker);

#define ccdbenter(marker) (ccdebugthis=ccsentry_enter(cccall(),ccdebugthis,marker))
#define ccdbleave(marker) (ccdebugthis=ccsentry_leave(cccall(),ccdebugthis,marker))

ccfunc const char *ccfilename(const char *name);

ccfunc void cctrace_(cccaller_t caller, const char *label, const char *format, ...);
#define cctrace(label,fmt,...) (cctrace_(cccaller(__COUNTER__,_CCFILE,_CCLINE,_CCFUNC),label,fmt,__VA_ARGS__),0)
#define cctracelog(fmt,...) (cctrace("log",fmt,__VA_ARGS__),0)
#define cctracewar(fmt,...) (cctrace("war",fmt,__VA_ARGS__),0)
#define cctraceerr(fmt,...) (cctrace("err",fmt,__VA_ARGS__),0)
#define ccdebuglog(fmt,...) (cctrace("dbg",fmt,__VA_ARGS__),0)

#ifdef _CCEXECLOG
# define ccexeclog(fmt,...) (cctrace("exec",fmt,__VA_ARGS__),0)
#else
# define ccexeclog(fmt,...) (0)
#endif

// Note:
#include "ccsys.c"
#include "ccdbg.c"

#ifdef _DEVELOPER
#undef malloc
#define malloc DO_NOT_USE_MALLOC
#undef realloc
#define realloc DO_NOT_USE_REALLOC
#undef free
#define free DO_NOT_USE_FREE
#endif

// Todo:
ccfunc void
ccdlbdel_(ccvala_t vala)
{
  ccassert(vala!=0);

  ccdlb_t *dlb=ccdlb(*vala);
  if(dlb)
  {
    ccassert(dlb->allocator!=0);

    if(dlb->entries)
    {
      ccentry_t *i,*f,*e;
      ccarrfor(dlb->entries,e)
      { for(i=e->nex;i;)
        { f=i;
          i=i->nex;

          dlb->allocator(cccall(),0,f);
        }
      }

      ccarrdel(dlb->entries);
    }

    dlb->allocator(cccall(),0,dlb);
  }

  *vala=0;
}

ccfunc cci64_t
ccdlbadd_(ccvala_t vala, cci32_t isze, cci64_t rsze, cci64_t csze)
{
  ccdlb_t *dlb;
  ccallocator_t *allocator;
  int rem_rze,rem_add;
  cci64_t sze_max, sze_min;

  if(*vala)
  { dlb=ccdlb_(*vala);
    allocator=dlb->allocator;
    sze_max=dlb->sze_max;
    sze_min=dlb->sze_min;
    rem_rze=dlb->rem_rze;
    rem_add=dlb->rem_add;
    ccassert(!rem_add);
  } else
  { dlb=ccnull;
    allocator=ccallocator;
    sze_max=0;
    sze_min=0;
    rem_rze=0;
    rem_add=0;
  }

  ccassert(csze<=rsze+sze_max-sze_min);

  if(sze_max<sze_min+rsze)
  {
    ccassert(!rem_rze);

    sze_max<<=1;
    if(sze_max<sze_min+rsze)
      sze_max=sze_min+rsze;

    dlb=cccast(ccdlb_t*,allocator(cccall(),sizeof(*dlb)+sze_max*isze,dlb));

    if(!*vala)
    { dlb->rem_rze=rem_rze;
      dlb->rem_add=rem_add;
      dlb->allocator=allocator;
      dlb->entries=0;
    }

    *vala=dlb+1;

    dlb->sze_max=sze_max;
  }

  dlb->sze_min=sze_min+csze;
  return sze_min;
}

// Todo: does this use a good hashing function for strings?
// Todo: remove caller when not in debug mode!
ccfunc int
cctblhsh_(cccaller_t caller, void **ccm, cci32_t bit, int len, char *key, int create_always)
{
  ccassert(len!=0);
  ccassert(key!=0);

  ccdlb_t *dlb;

  if(!*ccm)
  {
    if(create_always)
    {
      ccdlbadd_(ccm,bit,1,0);

      dlb=ccdlb_(*ccm);

      // Todo: how should we size up the hash-table?
      ccarradd(dlb->entries,0xff);
      ccarrzro(dlb->entries);
      ccarrfix(dlb->entries);
    }
      else return ccfalse;
  }
    else dlb=ccdlb_(*ccm);

  ccu64_t hsh=5381;
  ccu64_t idx,max;

  if(len>0)
    for(idx=0;idx<len;hsh=hsh<<5,hsh=hsh+key[idx++]);
  else
    hsh=cccast(ccu64_t,key);

  max=ccarrmax(dlb->entries);

  idx=hsh%max;

  ccentry=dlb->entries+idx;

  while(ccentry->key)
  { if(ccentry->len==len)
    { if(len>0)
      { if(!memcmp(ccentry->key,key,len))
          return cctrue;
      } else
      { if(ccentry->key==key)
          return cctrue;
      }
    }
    if(!ccentry->nex)
      break;
    ccentry=ccentry->nex;
  }

  if(create_always)
  {
    if(ccentry->key)
      ccentry=ccentry->nex=ccmalloc_T(ccentry_t);

    ccentry->caller=caller;

    cci64_t val=ccdlbadd_(ccm,bit,1,1);

    // Todo: remove this from here when other code no longer depends on it!
    memset(cccast(char*,*ccm)+val*bit,ccnull,bit);

    // Todo:
    ccentry->nex=ccnull;
    ccentry->len=len;
    ccentry->key=key;
    ccentry->val=val;
  }

  return ccfalse;
}

// Todo: remove caller in release mode!
ccfunc cci64_t
cctblgeti_(cccaller_t caller, void **ccm, cci32_t isze, cci32_t len, char *key)
{ ccassert(ccm!=0);
  ccerrset(ccerr_kNIT);
  if(cctblhsh_(caller,ccm,isze,len,key,ccfalse))
    ccerrset(ccerr_kNON);
  return ccentry? ccentry->val :ccnull;
}

// Todo: remove caller in release mode!
ccfunc cci64_t
cctblputi_(cccaller_t caller, void **ccm, cci32_t isze, cci32_t len, char *key)
{ ccassert(ccm!=0);
  ccerrset(ccerr_kAIT);
  if(cctblhsh_(caller,ccm,isze,len,key,cctrue))
  { cctrace_(caller,"war","already in table, added by %s[%i]::%s()",
      ccfilename(ccentry->caller.file),ccentry->caller.line,ccentry->caller.func);
  } else
    ccerrset(ccerr_kNON);
  return ccentry? ccentry->val :ccnull;
}

// Todo: remove caller in release mode!
ccfunc cci64_t
cctblseti_(cccaller_t caller, void **ccm, cci32_t isze, cci32_t len, char *key)
{ ccassert(ccm!=0);
  ccerrset(ccerr_kNON);
  cctblhsh_(caller,ccm,isze,len,key,cctrue);
  return ccentry? ccentry->val :ccnull;
}

ccfunc cci64_t
ccstraddi_(char **ccm, cci64_t res, cci64_t com, const char *mem)
{
  // Note: assuming that you'll reserve at-least one more byte for the null terminator ...
  ccassert(res!=0);
  ccassert(com!=0);

  cci64_t result=ccdlbadd_(cccast(void**,ccm),sizeof(char),res,com);

  char *cur=*ccm+result;

  memcpy(cur,mem,res-1);
  cur[res-1]=0;

  return result;
}


// Todo: remove from here?
ccfunc cci64_t
ccstrcatiV_(char **ccm, const char *fmt, va_list vli)
{ int len=stbsp_vsnprintf(0,0,fmt,vli);
  cci64_t res=ccdlbadd_(cccast(void**,ccm),1,len+1,len);
  len=stbsp_vsnprintf((char*)*ccm+res,len+1,fmt,vli);
  return res;
}

// Todo: remove from here?
ccfunc cci64_t
ccstrcatiF_(char **ccm, const char *fmt, ...)
{ va_list vli;
  va_start(vli,fmt);
  cci64_t res=ccstrcatiV_(ccm,fmt,vli);
  va_end(vli);
  return res;
}

// Note:
typedef struct ccexec_t       ccexec_t;
typedef struct ccvalue_t      ccvalue_t;
typedef struct ccblock_t      ccblock_t;
typedef struct ccexec_value_t ccexec_value_t;

#include "ccread.h"
#include "cctree.h"
#include "cctype.h"
#include "ccseer.h"

#include "ccedict.h"

#include "ccseer.c"
#include "ccemit.h"
#include "ccexec.h"

#include "ccrune.c"
#include "ccread.c"
#include "ccemit.c"
#include "ccexec.c"
#include "cccore.c"

#ifdef _MSC_VER
# ifndef _DEVELOPER
#  pragma warning(pop)
# endif
#endif

#ifdef __cplusplus
}
#endif

#endif