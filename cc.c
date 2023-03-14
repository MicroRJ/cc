//
// Copyright (c) 2022 Dayan Rodriguez
//
// cc | c-cures v1.0 | 2022,2023
// https://github.com/MicroRJ/cc
//
// Public domain stb style c utilities:
// - standardized basic types
// - dynamic length buffer with builtin hash-table
// - malloc wrapper with leak detection
// - timing utilities
// - file utilities
// - cc-lang
//
// Still piecing everything together, trying not to add too many things here,
// enough for just about every basic program.
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

#ifndef _CC_STATIC_MEMORY
# define _CC_STATIC_MEMORY 0x1000
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

typedef void **ccvala_t;

#define cccast(T,mem) ((T)(mem))
#define ccvala(v) cccast(ccvala_t,&(v))

// Todo: remove these!
#define ccaddr(mem)   (&(mem))
#define ccdref(mem)   (*(mem))


// Note: some necessary error codes ...
typedef enum ccerror_k
{
  ccerror_kNON=0,
  ccerror_kNIT,
  ccerror_kAIT,
} ccerror_k;

ccglobal const char *ccerror_ks[]=
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

struct
{ ccerror_k  error;
  ccentry_t *entry;

  ccward_t  *wards;

  ccallocator_t *allocator_stack[0x10];
  int            allocator_index;
  ccallocator_t *allocator;

  cci16_t color_stack[0x10];
  cci16_t color_index;
  cci16_t color_value;

  char static_memory[_CC_STATIC_MEMORY];
  int  static_cursor;

  void *console;

  ccclocktick_t clock_seconds_in_ticks;

  ccsentry_t   debug_root;
	ccsentry_t  *debug_this;
	cci32_t      debug_none;
} ccglobal ccthread_local cc;

ccfunc void ccinit();

#define ccerrset(x) ((cc.error=x),0)
#define ccerrnon()  (cc.error==ccerror_kNON)
#define ccerrstr()  (ccerror_ks[cc.error])
#define cckeyget()  ((cc.entry)?(cc.entry->key):ccnull)

// -- Todo: not sure of this is the best API for this sort of stuff!
#define ccpush_allocator( ) cc.allocator_stack[cc.allocator_index++]=cc.allocator
#define ccpull_allocator( ) cc.allocator=cc.allocator_stack[--cc.allocator_index]
#define ccmove_allocator(x) cc.allocator=x

// Todo:
#define ccmove_color( x )  cc.color_value=x
#define ccpush_color(   )  cc.color_stack[cc.color_index++]=cc.color_value
#define ccload_color(   )  cc.color_value=cc.color_stack[--cc.color_index]

#define cctrue  cccast(cci32_t,1)
#define ccfalse cccast(cci32_t,0)

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

// Note: default allocator
#define ccmalloc(size)       (ccuserallocator_(cccall(),size,ccnull))
#define ccrealloc(data,size) (ccuserallocator_(cccall(),size,data))
#define ccfree(data)         (ccuserallocator_(cccall(),ccnil,data))
#define ccmalloc_T(T) cccast(T*,ccmalloc(sizeof(T)))

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

// Note: these use the global error code _ccerror_k
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

ccfunc ccsentry_t * ccsentry_enter(cccaller_t caller, ccsentry_t *master, const char *marker);
ccfunc ccsentry_t * ccsentry_leave(cccaller_t caller, ccsentry_t *sentry, const char *marker);

#ifdef _CCDEBUG
# define ccdbenter(marker) (cc.debug_this=ccsentry_enter(cccall(),cc.debug_this,marker))
# define ccdbleave(marker) (cc.debug_this=ccsentry_leave(cccall(),cc.debug_this,marker))
#else
# define ccdbenter(marker) (0)
# define ccdbleave(marker) (0)
#endif

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

// Todo: this is not complete!
ccfunc void ccprintf(const char *fmt, ...);

// Note:
#include "ccsys.c"
#include "ccdbg.c"

#ifdef _DEVELOPER
# undef malloc
#  define malloc DO_NOT_USE_MALLOC
# undef realloc
#  define realloc DO_NOT_USE_REALLOC
# undef free
#  define free DO_NOT_USE_FREE
#endif

///////////////////////
// Implementation
///////////////////////

// Todo: can we get rid of this? or at-least make it optional?
ccfunc void
ccinit()
{
  cc.debug_this=&cc.debug_root;
  cc.console=(void*)GetStdHandle(STD_OUTPUT_HANDLE);

	ccmove_allocator(ccuserallocator_);
  ccmove_color(7);

#ifdef _WIN32
  LARGE_INTEGER l;
  QueryPerformanceFrequency(&l);
  cc.clock_seconds_in_ticks=l.QuadPart;
#endif
}

ccfunc ccinle ccclocktick_t
ccclocktick()
{
#ifdef _WIN32
	LARGE_INTEGER l;
  QueryPerformanceCounter(&l);
  return l.QuadPart;
#endif
}

ccfunc ccinle void
ccout(const char *string)
{
#if defined(_WIN32) && defined(_CCDEBUG)
  OutputDebugStringA(string);
#endif
  printf(string);
}

ccfunc ccinle ccf64_t
ccclocksecs(ccclocktick_t t)
{
	return t/(ccf64_t)cc.clock_seconds_in_ticks;
}

ccfunc ccinle int
ccformatvex(char *buf, int len, const char * fmt, va_list vli)
{
  return stbsp_vsnprintf(buf,len,fmt,vli);
}

ccfunc ccinle char *
ccformatv(const char *fmt, va_list vli)
{ int length=ccformatvex(ccnull,ccnull,fmt,vli);
  int static_length=ccCarrlenL(cc.static_memory);
  if(cc.static_cursor+length>=static_length)
    cc.static_cursor=0;
  char *string=cc.static_memory+cc.static_cursor;
  ccformatvex(string,static_length-cc.static_cursor,fmt,vli);
  cc.static_cursor+=length;
  return string;
}

ccfunc int
ccformatex(char *buf, int len, const char * fmt, ...)
{ va_list vli;
  va_start(vli,fmt);
  int res=ccformatvex(buf,len,fmt,vli);
  va_end(vli);
  return res;
}

ccfunc char *
ccformat(const char * fmt, ...)
{ va_list vli;
  va_start(vli,fmt);
  char *res=ccformatv(fmt,vli);
  va_end(vli);
  return res;
}

ccfunc void
cctrace_(cccaller_t caller, const char *label, const char *format, ...)
{
  va_list vli;
  va_start(vli,format);

  ccout(
	  ccformat("%s: %s[%i] %s() %s\n",
	    label,ccfilename(caller.file),caller.line,caller.func,ccformatv(format,vli)));

  va_end(vli);
}

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
    allocator=cc.allocator;
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

  cc.entry=dlb->entries+idx;

  while(cc.entry->key)
  { if(cc.entry->len==len)
    { if(len>0)
      { if(!memcmp(cc.entry->key,key,len))
          return cctrue;
      } else
      { if(cc.entry->key==key)
          return cctrue;
      }
    }
    if(!cc.entry->nex)
      break;
    cc.entry=cc.entry->nex;
  }

  if(create_always)
  {
    if(cc.entry->key)
      cc.entry=cc.entry->nex=ccmalloc_T(ccentry_t);

    cc.entry->caller=caller;

    cci64_t val=ccdlbadd_(ccm,bit,1,1);

    // Todo: remove this from here when other code no longer depends on it!
    memset(cccast(char*,*ccm)+val*bit,ccnull,bit);

    // Todo:
    cc.entry->nex=ccnull;
    cc.entry->len=len;
    cc.entry->key=key;
    cc.entry->val=val;
  }

  return ccfalse;
}

// Todo: remove caller in release mode!
ccfunc cci64_t
cctblgeti_(cccaller_t caller, void **ccm, cci32_t isze, cci32_t len, char *key)
{ ccassert(ccm!=0);
  ccerrset(ccerror_kNIT);
  if(cctblhsh_(caller,ccm,isze,len,key,ccfalse))
    ccerrset(ccerror_kNON);
  return cc.entry? cc.entry->val :ccnull;
}

// Todo: remove caller in release mode!
ccfunc cci64_t
cctblputi_(cccaller_t caller, void **ccm, cci32_t isze, cci32_t len, char *key)
{ ccassert(ccm!=0);
  ccerrset(ccerror_kAIT);
  if(cctblhsh_(caller,ccm,isze,len,key,cctrue))
  { cctrace_(caller,"war","already in table, added by %s[%i]::%s()",
      ccfilename(cc.entry->caller.file),cc.entry->caller.line,cc.entry->caller.func);
  } else
    ccerrset(ccerror_kNON);
  return cc.entry? cc.entry->val :ccnull;
}

// Todo: remove caller in release mode!
ccfunc cci64_t
cctblseti_(cccaller_t caller, void **ccm, cci32_t isze, cci32_t len, char *key)
{ ccassert(ccm!=0);
  ccerrset(ccerror_kNON);
  cctblhsh_(caller,ccm,isze,len,key,cctrue);

  return cc.entry? cc.entry->val :ccnull;
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

// Todo: this is not the best way to do things!
#define CCFG_BLACK        "<!0"
#define CCFG_BLUE         "<!1"
#define CCFG_GREEN        "<!2"
#define CCFG_CYAN         "<!3"
#define CCFG_RED          "<!4"
#define CCFG_MAGENTA      "<!5"
#define CCFG_BROWN        "<!6"
#define CCFG_LIGHTGRAY    "<!7"
#define CCFG_GRAY         "<!8"
#define CCFG_LIGHTBLUE    "<!9"
#define CCFG_LIGHTGREEN   "<!0"
#define CCFG_LIGHTCYAN    "<!1"
#define CCFG_LIGHTRED     "<!2"
#define CCFG_LIGHTMAGENTA "<!3"
#define CCFG_YELLOW       "<!4"
#define CCFG_WHITE        "<!F"
#define CCEND             "!>"

// Todo: actually setup the color table properly ...
// CONSOLE_SCREEN_BUFFER_INFOEX e={sizeof(e)};
// if(GetConsoleScreenBufferInfoEx(h,&e))
// { SetConsoleScreenBufferInfoEx(h,&e);
// }

// Todo: this is absolute crap! modify stb's...
ccfunc void
ccprintf(const char *r, ...)
{
  va_list vli;
  va_start(vli,r);

#define cccolornext( l )  r+=l,c=*r
#define cccolorpval(   )  (void  *) va_arg(vli,void  *)
#define cccolorsval(   )  (char  *) va_arg(vli,char  *)
#define cccolorlval(   )  (cci64_t) va_arg(vli,cci64_t)
#define cccolorival(   )  (cci32_t) va_arg(vli,cci32_t)
#define cccolorfval(   )  (ccf64_t) va_arg(vli,ccf64_t)
#define cccolorwval(   )  (cci16_t) va_arg(vli,cci32_t)
#define cccolorcval(   )  (char   ) va_arg(vli,char   )

  char c;
  for(;c=*r;c)
  {
    for(; (c!='\0')&&(c!='%') &&
         !(c=='<'&&r[1]=='!') &&
         !(c=='!'&&r[1]=='>'); cccolornext(1)) printf("%c",c);

    if(c=='<')
    { cccolornext(2);
      if(CCWITHIN(c,'0','9'))
        ccpush_color(),ccmove_color(0x00+c-'0'),cccolornext(1);
      else
      if(CCWITHIN(c,'A','F'))
        ccpush_color(),ccmove_color(0x0A+c-'A'),cccolornext(1);
      else
      if(CCWITHIN(c,'a','f'))
        ccpush_color(),ccmove_color(0x0A+c-'a'),cccolornext(1);
      else
      if(c=='%'&&r[1]=='i')
        ccpush_color(),ccmove_color(cccolorwval()),cccolornext(2);
      else
        ccassert(!"error");
    } else
    if(c=='!')
    { ccload_color(),cccolornext(2);
    } else
    if(c=='%')
    { cccolornext(1);
      if(c=='%')
        printf("%%"),cccolornext(1);
      else
      if(c=='i')
        printf("%i",cccolorival()),cccolornext(1);
      else
      if(c=='f')
        printf("%f",cccolorfval()),cccolornext(1);
      else
      if(c=='s')
        printf("%s",cccolorsval()),cccolornext(1);
      else
      if(c=='c')
        printf("%c",cccolorcval()),cccolornext(1);
      else
      if(c=='p')
        printf("%p",cccolorpval()),cccolornext(1);
      else
      { if(c=='l'&&r[1]=='l'&&r[2]=='i')
          printf("%lli",cccolorlval()),cccolornext(3);
        else
        if(c=='l'&&r[1]=='l'&&r[2]=='u')
          printf("%llu",cccolorlval()),cccolornext(3);
        else
          ccassert(!"error");
      }
    } else
    if(c=='\0')
      break;

#ifdef _WIN32
    SetConsoleTextAttribute((HANDLE)cc.console,cc.color_value);
#endif
  }
  va_end(vli);
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