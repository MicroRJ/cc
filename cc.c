/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
#ifndef _CC
#define _CC
// Note: CC stands for (C)ommon (C)ode and it was originally meant
// to be a simple set of structures and functionality that you'd
// use on almost any project ...
// It is still meant to be as such ...

#ifdef __cplusplus
extern "C" {
#endif

#define _CCGUID __COUNTER__
#define _CCFILE __FILE__
#define _CCLINE __LINE__
#define _CCFUNC __func__

// Note: suppress warnings ...
#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable:4053)
# pragma warning(disable:4706)
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

// Note: this makes easier to regex
#ifndef ccaddr
# define ccaddr(mem) (&(mem))
#endif
#ifndef ccdref
# define ccdref(mem) (*(mem))
#endif
#ifndef cccast
# define cccast(type,mem) ((type)(mem))
#endif

#ifndef cctrue
# define cctrue cccast(int,1)
#endif
#ifndef ccfalse
# define ccfalse cccast(int,0)
#endif

#ifndef ccnil
# define ccnil 0
#endif

// Note: compile time assert ...
#ifndef _CCASSERT
# define _CCASSERT(x) typedef char __STATIC__ASSERT__[((x)?1:-1)]
#endif

// Note:
#ifndef ccthread_local
# define ccthread_local __declspec(thread)
#endif
#ifndef ccglobal
# define ccglobal static
#endif

// Note:
#ifndef ccfunc
# define ccfunc static
#endif
#ifndef ccinle
# ifdef __forceinline
#  define ccinle __forceinline
# else
#  define ccinle inline
# endif
#endif

// Note: this makes it easier to spot problems when the fields of a struct are not properly or accessed through the right tag ...
#ifndef ccunion
# ifdef _HARD_DEBUG
#  define ccunion struct
# else
#  define ccunion union
# endif
#endif

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

// Todo: ensure proper size ...
_CCASSERT(sizeof(ccu64_t)==sizeof(size_t));
_CCASSERT(sizeof(cci64_t)==sizeof(size_t));


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

// Note: common error codes ... let's try to keep this conservative as I don't want to keep promoting
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
typedef void *(ccallocator_t)(size_t,void *);

// Note: allocator is just to determine whether we're using the right allocator ...
typedef struct ccheap_block_t ccheap_block_t;
typedef struct ccheap_block_t
{ char head_guard[4];
  ccallocator_t   * allocator;
  ccheap_block_t  * prev, * next;
  cccaller_t        caller;
  size_t            size;
  char tail_guard[4];
} ccheap_block_t;

// Note: entry status, this also must be a global ...
typedef enum ccent_k
{
  ccent_kNIT=1,
  ccent_kAIT=2,
} ccent_k;

// Note: general table entry
typedef struct ccentry_t ccentry_t;
typedef struct ccentry_t
{ ccentry_t * nex;
  cci32_t     len;
  char      * key;
  size_t      val;
} ccentry_t;

// Note: dynamic length buffer ...
// Todo: At the moment, not very well integrated with heap blocks ...
typedef struct ccdlb_t ccdlb_t;
typedef struct ccdlb_t
{ unsigned        rem_add: 1;
  unsigned        rem_rze: 1;
  ccallocator_t * allocator;
  ccentry_t     * entries;
  size_t          sze_max;
  size_t          sze_min;
} ccdlb_t;

// Note: A debug event collect a snapshot of certain metrics in between the enter and leave function calls...
typedef union ccdebug_event_t ccdebug_event_t;
typedef union ccdebug_event_t
{ cci64_t e[6];
  struct
  { cci64_t allocations;
    cci64_t deallocations;
    cci64_t reallocations;
    cci64_t heap_block_count;
    cci64_t memory;

    cci64_t collisions;
  };
} ccdebug_event_t;

// Note: A debug context tracks a specific debug event, since the debug event could occur multiple times
// two versions must be stored, this to differentiate the event's metrics, this is what I call "tracking"
// or keeping track of the change over a set of occurrences.
// This difference is then simply be added up to the super context and thus constituting the "tracking"
// bit.
// Debug contexts also collect child debug contexts by means of dlb ...
typedef struct ccdebug_sentry_t ccdebug_sentry_t;
typedef struct ccdebug_sentry_t
{ const char * label;

  ccdebug_sentry_t  * super;
  ccdebug_sentry_t  * child;

  cccaller_t   caller;
  cci32_t      level;

  ccclocktime_t start_event_ticks;
  ccclocktime_t total_event_ticks;

  ccdebug_event_t last_event;

  cci32_t         event_count;
  ccdebug_event_t event;

  // Note: only the root debug context may have heap blocks ...
  ccheap_block_t *last_heap_block;
} ccdebug_sentry_t;


ccfunc void *ccuserallocator_(size_t,void*);
ccfunc void *ccinternalallocator_(size_t,void*);

// Note:
ccglobal ccthread_local ccclocktime_t ccclockfreqc;

// Note: global error code
ccglobal ccthread_local ccerr_k ccerr;

// Note: global key
ccglobal ccthread_local ccstr_t cckey;

// Note: you have to stack this manually if you wish to change it
ccglobal ccthread_local ccallocator_t *ccallocator=ccuserallocator_;

#ifdef _HARD_DEBUG
// Note: do not access these directly ...
ccglobal ccthread_local cccaller_t  cccallstack[0x04];
ccglobal ccthread_local cci32_t     cccallindex;
ccglobal ccthread_local cci32_t     cchascaller;

// Note: do not access these directly ... use the ccdebug() and ccevent() functions instead ...
ccglobal ccthread_local ccdebug_sentry_t   ccdebugroot;
ccglobal ccthread_local ccdebug_sentry_t  *ccdebugthis;
ccglobal ccthread_local cci32_t     ccdebugnone;
ccglobal ccthread_local cci32_t     ccnopushrob;
// #ccnopushrob
// Set this when you want to ensure the next function you call won't rob your push
// #ccdebugnone
// Dynamically choose an execution path to solve infinite recursion and the inclusion of
// misleading debug metrics and push steal so long as the function exits immediately and accordingly.
//
// This happens when a 'push' function calls some other 'push' function before actually doing the 'pushing' part.
// For instance:
//
// Context c; =0
// Boolean d; =0
//
// ccpush():
//   ccenter()
//   actually_push()
//   ccleave();
// ccenter():
//   ccpush();
// ccleave():
//   ccassert(c);
// ccmain():
//   ccenter()
//   ccleave();
//
// To solve:
//
// ccpush():
//   ccenter()
//   actually_push()
//   ccleave();
// ccenter():
//   d ?^        // If 'd' or 'disabled' don't collect anything
//   p=d,d=true  // Store what 'd' was previously, set 'd' or 'disabled' to true
//   c=ccpush();
//   d=p         // Restore 'd'
// ccleave():
//   d ?^
//   ccassert(c);
// ccmain():
//   ccenter()
//   ccleave();


#endif

// Note: global error
#define ccerrset(err) ((ccerr=err),0)
#define ccerrnon()    ((ccerr)==ccerr_kNON)
#define ccerrsom()    ((ccerr)!=ccerr_kNON)
#define ccerrnit()    ((ccerr)==ccerr_kNIT)
#define ccerrait()    ((ccerr)==ccerr_kAIT)

// Note: global key
#define cckeyset(key) (cckey=key)
#define cckeyget()    (cckey)

ccfunc ccinle ccdebug_event_t *ccevent_();
ccfunc ccinle ccdebug_sentry_t       *ccdebug_();

#ifdef _HARD_DEBUG
ccfunc ccinle cccaller_t cccaller(int guid, const char *file, int line, const char *func);
ccfunc void ccpushcaller(cccaller_t);
ccfunc ccinle cccaller_t ccpullcaller();
ccfunc void ccenter_callme(const char *label);
ccfunc void ccleave_callme(const char *label);
// Todo: this is deprecated ... how are you going to expand this to zero later on?
# ifndef ccdebug
#  define ccdebug() cccall(ccdebug_())
# endif
# ifndef ccevent
#  define ccevent() cccall(ccevent_())
# endif
# ifndef cccallas
#  define cccallas(caller,callee) (ccpushcaller(caller),(callee))
# endif
# ifndef cccall
#  define cccall(callee) cccallas(cccaller(__COUNTER__,_CCFILE,_CCLINE,_CCFUNC),callee)
# endif
#else
# define ccenter_callme(x) 0
# define ccleave_callme(x) 0
# define ccpullcaller() {}
ccglobal ccdebug_sentry_t dummy;
# ifndef ccdebug
#  define ccdebug() (&dummy)
# endif
# ifndef ccevent
#  define ccevent() (&dummy.event)
# endif
# ifndef cccallas
#  define cccallas(caller,callee) (callee)
# endif
# ifndef cccall
#  define cccall(callee) (callee)
# endif
#endif

// Note: C string ...
#ifndef cclit
# define cclit(lit) cccast(unsigned int,sizeof(lit)-1),(lit)
#endif
#ifndef ccstrlenS
# define ccstrlenS(cstr) cccast(ccu32_t,strlen(cstr))
#endif
#ifndef ccstrlenL
# define ccstrlenL(lstr) cccast(ccu32_t,sizeof(lstr)-1)
#endif


// Note: simple trace logging ...
ccfunc void cctrace_(const char *label, const char *format, ...);

#ifndef cctracelog
# define cctracelog(fmt,...) (cccall(0),cctrace_("log",fmt,__VA_ARGS__))
#endif
#ifndef cctracewar
# define cctracewar(fmt,...) (cccall(0),cctrace_("war",fmt,__VA_ARGS__))
#endif
#ifndef cctraceerr
# define cctraceerr(fmt,...) (cccall(0),cctrace_("err",fmt,__VA_ARGS__))
#endif

#ifndef ccmalloc
# define ccmalloc(size) cccall(ccuserallocator_(size,ccnil))
#endif
#ifndef ccrealloc
# define ccrealloc(data,size) cccall(ccuserallocator_(size,data))
#endif
#ifndef ccfree
# define ccfree(data) cccall(ccuserallocator_(ccnil,data))
#endif



#ifndef ccenter
# define ccenter(label) cccall(ccenter_callme(label))
#endif
#ifndef ccleave
# define ccleave(label) cccall(ccleave_callme(label))
#endif

ccfunc const char *ccfilename(const char *name);

// Todo: cache clock frequency
// Note: some platform functions...
ccfunc ccinle ccclocktime_t ccclocktick();
ccfunc ccinle ccf64_t ccclocksecs(ccu64_t);

ccfunc int ccrealfile(void *);
ccfunc void ccclosefile(void *);
ccfunc void *ccopenfile(const char *);
ccfunc void *ccpullfile(void *,unsigned long int,unsigned long int *);
ccfunc unsigned long int ccpushfile(void *,unsigned long int,unsigned long int,void*);
ccfunc unsigned long int ccfilesize(void *);

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






// #ifdef _HARD_DEBUG
// # ifndef ccmalloc
// #  define ccmalloc(len) cccall(ccmalloc_(len))
// # endif
// # ifndef ccrealloc
// #  define ccrealloc(mem,len) cccall(ccrealloc_(mem,len))
// # endif
// # ifndef ccfree
// #  define ccfree(mem) cccall(ccfree_(mem))
// # endif
// #else
// # ifndef ccmalloc
// #  define ccmalloc(len) malloc(len)
// # endif
// # ifndef ccrealloc
// #  define ccrealloc(mem,len) realloc(mem,len)
// # endif
// # ifndef ccfree
// #  define ccfree(mem) free(mem)
// # endif
// #endif // #ifdef _HARD_DEBUG

#ifndef ccmalloc_T
# define ccmalloc_T(type) cccast(type*,ccmalloc(sizeof(type)))
#endif
#ifndef ccrealloc_T
# define ccrealloc_T(mem,type) cccast(type*,ccrealloc(mem,sizeof(type)))
#endif




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
typedef struct ccread_t ccread_t;
typedef struct ccemit_t ccemit_t;
typedef struct ccexec_t ccexec_t;


typedef struct ccemit_value_t ccemit_value_t;
typedef struct ccemit_block_t ccemit_block_t;
typedef struct ccemit_procd_t ccemit_procd_t;
typedef struct ccexec_value_t ccexec_value_t;

typedef struct ccjump_point_t ccjump_point_t;

typedef struct cctype_t cctype_t;
typedef struct cctree_t cctree_t;

#include "ccfio.c"

#include "cclog.h"
#include "ccdlb.h"
#define _CCLOG_IMPL
#include "cclog.h"

ccfunc void *ccinternalallocator_(size_t size,void *data)
{ ccpullcaller();
  if(size)
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
ccdebug_checkblock(ccallocator_t *allocator, ccheap_block_t *block)
{ for(int i=0; i<4; ++i)
    ccassert(block->head_guard[i]==ccnil);
  for(int i=0; i<4; ++i)
    ccassert(block->tail_guard[i]==ccnil);
  ccassert(block->allocator==allocator);
  ccassert(!block->prev||block->prev->next==block);
  ccassert(!block->next||block->next->prev==block);
  ccassert(!block->next||block->next!=block);
  ccassert(!block->prev||block->prev!=block);
}

ccfunc void *ccuserallocator_(size_t size,void *data)
{
	cccaller_t caller=ccpullcaller();

ccenter("user-allocator");
  ccdebug_sentry_t *debug=ccdebug();
  ccheap_block_t *block;
  if(!size)
  {
ccenter("free");
  	block=(ccheap_block_t*)data-1;
    ccdebug_checkblock(ccuserallocator_,block);
    if(block->prev) block->prev->next=block->next;
    if(block->next) block->next->prev=block->prev;
    else ccdebugroot.last_heap_block=block->prev;
    block->next=ccnil;
    block->prev=ccnil;
    block->caller=caller;
    debug->event.heap_block_count--;
    debug->event.deallocations++;
    debug->event.memory-=block->size;
    free(block);
    block=ccnil;
ccleave("free");
  } else
  { if(data)
    {
ccenter("ccrealloc");
    	block=(ccheap_block_t*)data-1;
      ccdebug_checkblock(ccuserallocator_,block);

      debug->event.memory+=size-block->size;
      debug->event.reallocations++;

      ccassert(ccdebugroot.last_heap_block!=0);
      int is_last=block==ccdebugroot.last_heap_block;
      block=(ccheap_block_t*)realloc(block,sizeof(*block)+size);
      block->caller=caller;
      block->size=size;
      if(block->prev) block->prev->next=block;
      if(block->next) block->next->prev=block;
      if(is_last) ccdebugroot.last_heap_block=block;
ccleave("ccrealloc");
    } else
    {
ccenter("malloc");
    	block=(ccheap_block_t*)malloc(sizeof(*block)+size);
      memset(block,ccnil,sizeof(*block));
      block->allocator=ccuserallocator_;
      block->caller=caller;
      block->size=size;

      if(!ccdebugroot.last_heap_block)
      { ccdebugroot.last_heap_block=block;
      } else
      { block->prev=ccdebugroot.last_heap_block;
        ccdebugroot.last_heap_block->next=block;
        ccdebugroot.last_heap_block=block;
      }

      ccdebug_checkblock(ccuserallocator_,block);

      debug->event.heap_block_count++;
      debug->event.allocations++;
      debug->event.memory+=size;
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
#include "cccheck.c"
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