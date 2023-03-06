// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CC_C
#define _CC_C

#define _CCVERSION 1000

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

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

#if defined(_DEBUG) || defined(DEBUG) || defined(DBG)
# define _CCDEBUG
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define _CCGUID __COUNTER__
#define _CCFILE __FILE__
#define _CCLINE __LINE__
#define _CCFUNC __func__

#ifdef _MSC_VER
# ifndef _DEVELOPER
#  pragma warning(push)
# endif
#endif

// Note: suppress warnings ...
#ifdef _MSC_VER
# pragma warning(disable:4018) // signed unsigned mismatch
# pragma warning(disable:4101) // not referenced local variable
# pragma warning(disable:4311) // cast pointer to int
# pragma warning(disable:4312) // cast int to pointer
# pragma warning(disable:4302) // w'type cast': truncation from 'char *' to 'ccu32_t'
# pragma warning(disable:4706)
#ifdef _DEVELOPER
# pragma warning(disable:4189)
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

// Todo:
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

#define cccast(T,mem) ((T)(mem))
#define ccaddr(mem)   (&(mem))
#define ccdref(mem)   (*(mem))

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

// Note:
typedef ccu64_t ccclocktime_t;

typedef char *ccstr_t;

// Note: used for enhancing debug data ...
typedef struct cccaller_t cccaller_t;
typedef struct cccaller_t
{ int           guid;
  const char   *file;
  int           line;
  const char   *func;
} cccaller_t;

// Note: simple allocator function, must-have for switching the allocator of debug routines ...
typedef void *(ccallocator_t)(size_t,void *);

// Note: entry status, this also must be a global ...
typedef enum ccentry_k ccentry_k;
typedef enum ccentry_k
{
  ccent_kNIT=1,
  ccent_kAIT=2,
} ccentry_k;

typedef struct ccentry_t ccentry_t;
typedef struct ccentry_t
{ cccaller_t  caller;
  ccentry_t * nex;
  cci32_t     len;
  char      * key;
  size_t      val;
} ccentry_t;

// Note: dynamic length buffer ...
// Todo: At the moment, not very well integrated ...
// Note: val is in item count!
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
{ const char  * marker;
  ccsentry_t  * master;

  ccsentry_t  * slaves;

  cci32_t       level;
  cccaller_t    caller;

  // Note: this is the last block ...
  ccsentry_block_t * block_list;

  // Todo: it would be interesting if we'd also record child enter and leave count, this way
  // we'd also detect when a child hasn't properly left...
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

typedef struct cccolorstate_t
{ cci16_t  s[0x10];
  cci16_t  i;
  cci16_t  v;
} cccolorstate_t;

ccfunc void *ccuserallocator_(size_t,void*);
ccfunc void *ccinternalallocator_(size_t,void*);

// Note: global error and entry
ccglobal ccthread_local ccerr_k    ccerr;
ccglobal ccthread_local ccentry_t *ccentry;

#define ccerrstr()    (ccerr_s[ccerr])
#define ccerrset(val) ((ccerr=val),0)
#define ccerrnon()    ((ccerr)==ccerr_kNON)
#define ccerrsom()    ((ccerr)!=ccerr_kNON)
#define ccerrnit()    ((ccerr)==ccerr_kNIT)
#define ccerrait()    ((ccerr)==ccerr_kAIT)

#define cckeyset(val) (ccentry->key=val)
#define cckeyget()    (ccentry->key)

// Note:
ccfunc ccinle cccaller_t cccaller(int guid, const char *file, int line, const char *func);

// ccfunc ccinle cccaller_t cccaller(int guid, const char *file, int line, const char *func);

// Todo: remove this!
ccglobal ccthread_local cccaller_t cclastcaller;

#define cccall() (cclastcaller=cccaller(__COUNTER__,_CCFILE,_CCLINE,_CCFUNC))

// Todo: implement
ccglobal ccthread_local ccclocktime_t ccclockfreqc;


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

// Note: default allocator
#define ccmalloc(size)       (cccall(),ccuserallocator_(size,ccnull))
#define ccrealloc(data,size) (cccall(),ccuserallocator_(size,data))
#define ccfree(data)         (cccall(),ccuserallocator_(ccnil,data))

#define cclva(lv) cccast(void**,ccaddr(lv))


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

#define ccarrmax(arr) ((arr)?ccdlbmax_(arr):0)
#define ccarrmin(arr) ((arr)?ccdlbmin_(arr):0)

#define ccarrend_max(arr) ((arr)+ccarrmax(arr))
#define ccarrend_min(arr) ((arr)+ccarrmin(arr))

#define ccarrend ccarrend_min
#define ccarrlen(arr) cccast(ccu32_t,ccarrmin(arr))

#define ccarrresi(ccm,num) ccdlbadd(cclva(ccm),sizeof(*(ccm)),num,0)
#define ccarrres(ccm,num) ((ccm)+ccarrresi(ccm,num))

#define ccarraddi(ccm,num) ccdlbadd(cclva(ccm),sizeof(*(ccm)),num,num)
#define ccarradd(ccm,num) ((ccm)+ccarraddi(ccm,num))

#define ccarrone(ccm) ccarradd(ccm,1)
#define ccarrzro(ccm) memset(ccm,ccnil,ccarrmax(ccm)*sizeof(*ccm))

#define ccarrfor(arr,itr) for(itr=arr;itr<ccarrend(arr);++itr)

// Todo: remove?
#define ccarrfix(ccm) ((ccm)?ccdlb_(ccm)->rem_rze=cctrue:(ccnil))

// Note: table
#define cclithsh(lit) ccCstrlenL(lit),cccast(char*,lit)
#define ccntshsh(nts) ccCstrlenS(nts),cccast(char*,nts)
#define ccinthsh(ptr) -cccast(cci32_t,sizeof(ptr)),cccast(char*,ptr)

// Note: these use the global error code _ccerr_
ccfunc cci64_t cctblgeti(void **, cci32_t, cci32_t, char *);
ccfunc cci64_t cctblputi(void **, cci32_t, cci32_t, char *);
ccfunc cci64_t cctblseti(void **, cci32_t, cci32_t, char *);


#define cctblgeti(...) cctblgeti_(cccaller(__COUNTER__,_CCFILE,_CCLINE,_CCFUNC),__VA_ARGS__)
#define cctblputi(...) cctblputi_(cccaller(__COUNTER__,_CCFILE,_CCLINE,_CCFUNC),__VA_ARGS__)
#define cctblseti(...) cctblseti_(cccaller(__COUNTER__,_CCFILE,_CCLINE,_CCFUNC),__VA_ARGS__)

#define cctblputi_ex(caller,...) cctblputi_(caller,__VA_ARGS__)


// Todo: these is to be reworked ...
#define cctblgetL(ccm,lit) ((ccm)+cctblgeti(cclva(ccm),sizeof(*(ccm)),cclithsh(lit)))
#define cctblgetS(ccm,nts) ((ccm)+cctblgeti(cclva(ccm),sizeof(*(ccm)),ccntshsh(nts)))
#define cctblgetP(ccm,ptr) ((ccm)+cctblgeti(cclva(ccm),sizeof(*(ccm)),ccinthsh(ptr)))

#define cctblsetN(ccm,len,key) ((ccm)+cctblseti(cclva(ccm),sizeof(*ccm),len,cccast(char*,key)))
#define cctblsetL(ccm,lit) ((ccm)+cctblseti(cclva(ccm),sizeof(*ccm),cclithsh(lit)))
#define cctblsetS(ccm,nts) ((ccm)+cctblseti(cclva(ccm),sizeof(*ccm),ccntshsh(nts)))
#define cctblsetP(ccm,ptr) ((ccm)+cctblseti(cclva(ccm),sizeof(*ccm),ccinthsh(ptr)))

#define cctblputN(ccm,len,key) ((ccm)+cctblputi(cclva(ccm),sizeof(*ccm),len,cccast(char*,key)))
#define cctblputL(ccm,lit) ((ccm)+cctblputi(cclva(ccm),sizeof(*ccm),cclithsh(lit)))
#define cctblputS(ccm,nts) ((ccm)+cctblputi(cclva(ccm),sizeof(*ccm),ccntshsh(nts)))
#define cctblputP(ccm,ptr) ((ccm)+cctblputi(cclva(ccm),sizeof(*ccm),ccinthsh(ptr)))

#define cctblputP_ex(caller,ccm,ptr) ((ccm)+cctblputi_ex(caller,cclva(ccm),sizeof(*ccm),ccinthsh(ptr)))


// Note: string builder
#define ccstrdel ccarrdel
#define ccstrmax ccarrmax
#define ccstrmin ccarrmin
#define ccstrlen ccarrlen
#define ccstraddN(ccm,res,com) ((ccm)+ccdlbadd(cclva(ccm),sizeof(*(ccm)),res,com))

#define ccstrcatN(ccm,len,str) ((ccm)+ccstradd(&(ccm),len+1,len+0,str))
#define ccstrcatL(ccm,str) ccstrcatN(ccm,ccCstrlenL(str),str)
#define ccstrcatS(ccm,str) ccstrcatN(ccm,ccCstrlenS(str),str)

#define ccstrputN(ccm,len,str) ((ccm)+ccstradd(&(ccm),len+1,len+1,str))
#define ccstrputL(ccm,str) ccstrputN(ccm,ccCstrlenL(str),str)
#define ccstrputS(ccm,str) ccstrputN(ccm,ccCstrlenS(str),str)

#define ccstrcatF(ccm,fmt,...) ccstr_catf(&ccm,fmt,__VA_ARGS__)

// Todo: to be removed !
ccfunc ccinle ccsentry_t *ccdebug_();

// Todo: remove!
#define ccdebug() ccdebug_()

ccfunc ccsentry_t *
ccsentry_enter(cccaller_t caller, ccsentry_t *master, const char *marker);
ccfunc ccsentry_t *
ccsentry_leave(cccaller_t caller, ccsentry_t *sentry, const char *marker);

#define ccdbenter(marker) (ccdebugthis=ccsentry_enter(cccall(),ccdebugthis,marker))
#define ccdbleave(marker) (ccdebugthis=ccsentry_leave(cccall(),ccdebugthis,marker))

ccfunc const char *ccfilename(const char *name);

// Note: 'ccsys.c'
ccfunc ccinle ccclocktime_t ccclocktick();
ccfunc ccinle ccf64_t       ccclocksecs(ccu64_t);

// Todo: this file api leaves much to be desired ...
ccfunc void *   ccopenfile (const char *, const char *);
ccfunc int      ccrealfile ( void * );
ccfunc void     ccclosefile( void * );
ccfunc void   * ccpullfile ( void *, ccu32_t, ccu32_t *     );
ccfunc ccu32_t  ccpushfile ( void *, ccu32_t, ccu32_t,void *);
ccfunc ccu32_t  ccfilesize ( void * );

// Note: 'ccdbg.c'
ccfunc ccinle int   ccformatvex (char *,int,const char *,va_list);
ccfunc ccinle char *ccformatv   (const char *, va_list);
ccfunc        int   ccformatex  (char *, int, const char *, ...);
ccfunc        char *ccformat    (const char *, ...);

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
#include "ccdlb.c"
#include "ccdbg.c"

#ifdef _DEVELOPER
#undef malloc
#define malloc DO_NOT_USE_MALLOC
#undef realloc
#define realloc DO_NOT_USE_REALLOC
#undef free
#define free DO_NOT_USE_FREE
#endif

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