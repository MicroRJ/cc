/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
#ifndef _CC
#define _CC

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

// Note: ensure proper size ...
_CCASSERT(sizeof(ccu64_t)==sizeof(size_t));
_CCASSERT(sizeof(cci64_t)==sizeof(size_t));

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

#ifndef ccbreak
# define ccbreak __debugbreak
#endif

// Note: this makes it easier to spot problems when the fields of a struct are not properly or accessed through the right tag ...
#ifndef ccunion
# ifdef _HARD_DEBUG
#  define ccunion struct
# else
#  define ccunion union
# endif
#endif

#ifndef cclit
# define cclit(lit) cccast(unsigned int,sizeof(lit)-1),(lit)
#endif

// Note:
typedef ccu64_t ccclocktime_t;

// Note: debugging utilities
typedef struct cccaller_t cccaller_t;
typedef struct cccaller_t
{ int           guid;
  const char   *file;
  int           line;
  const char   *func;
} cccaller_t;

typedef struct ccdebug_block_t ccdebug_block_t;
typedef struct ccdebug_block_t
{ ccdebug_block_t *prev;
  ccdebug_block_t *next;
  cccaller_t      caller;
  size_t          size;
  int             removed;
  char underwrite[4];
} ccdebug_block_t;

// Note: each debug event contains a set of metrics that can be used to diff the next event's metrics,
// this makes it relatively fast to bubble child metrics, although realistically, you'd do this at the end ...
typedef union ccdebug_event_t ccdebug_event_t;
typedef union ccdebug_event_t
{ size_t e[6];
  struct
  { size_t allocations;
    size_t deallocations;
    size_t reallocations;
    size_t memory;
    size_t heap_block_count;

    size_t collisions;
    size_t overhead_clock_ticks;
  };
} ccdebug_event_t;

typedef struct ccdebug_t ccdebug_t;
typedef struct ccdebug_t
{
  ccdebug_t  * super;
  ccdebug_t  * child; // hash-map

  cccaller_t   caller;
  const char * label;
  cci32_t      level;

  ccclocktime_t start_event_ticks;
  ccclocktime_t total_event_ticks;

  ccdebug_event_t last_event;

  cci32_t         event_count;
  ccdebug_event_t event;

  ccdebug_block_t *last_heap_block;
} ccdebug_t;


// Note: do not access these directly ...
ccglobal ccthread_local cccaller_t  cccallstack[0x04];
ccglobal ccthread_local cci32_t     cccallindex;

// Note:
ccglobal ccthread_local ccdebug_t   ccdebugroot;
ccglobal ccthread_local ccdebug_t  *ccdebugthis;

// Note: enable debug features
ccglobal ccthread_local cci32_t     cctimed_enabled=cctrue;

// Note:
ccfunc void *ccmalloc_ (size_t);
ccfunc void *ccrealloc_(void *,size_t);
ccfunc void  ccfree_   (void *);



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

ccfunc ccinle ccdebug_event_t *ccstats();
ccfunc ccinle ccdebug_t *cctimed();



// Note: the idea is that the function you're calling knows that a caller will be put on the stack,
// and so that function must call ccpullcaller. Such functions shall end in '_', for instance 'ccmalloc_', there should be
// an accompanying macro 'ccmalloc' that pushes the caller onto the stack automatically.

ccfunc void ccpushcaller_(cccaller_t);
ccfunc ccinle cccaller_t ccpullcaller();

#ifndef ccpushcaller
# define ccpushcaller() ccpushcaller_(cccaller(1+__COUNTER__,ccfilename(_CCFILE),_CCLINE,_CCFUNC))
#endif

#ifndef cccall
# define cccall(call) (ccpushcaller(),call)
#endif

#ifndef cctimedhead
# define cctimedhead(label) cccall(cctimedhead_(label))
#endif

#ifndef cctimedtail
# define cctimedtail(label) (ccpushcaller(),cctimedtail_(label))
#endif

ccfunc ccinle cccaller_t cccaller(int guid, const char *file, int line, const char *func);

ccfunc void cctimedhead_(const char *label);
ccfunc void cctimedtail_(const char *label);


#ifdef _DEBUG
# define ccassert(is_true,...) (!(is_true)?ccbreak():ccnil)
#else
# define ccassert(is_true,...) ccnil
#endif

#ifndef ccnotnil
# define ccnotnil(val) (ccassert((val)!=ccnil),val)
#endif

ccfunc void
cctrace_(int guid, const char  *file, int line, const char *func, const char *tag, const char *fmt, ...);

#ifdef _DEBUG
# ifndef cctrace
#  define cctrace(tag,fmt,...) cctrace_(__COUNTER__,__FILE__,__LINE__,_CCFUNC,tag,fmt,__VA_ARGS__)
# endif
# ifndef cctracelog
#  define cctracelog(fmt,...) cctrace("log",fmt,__VA_ARGS__)
# endif
# ifndef cctracewar
#  define cctracewar(fmt,...) cctrace("war",fmt,__VA_ARGS__)
# endif
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

#ifdef _HARD_DEBUG
# ifndef ccmalloc
#  define ccmalloc(len) cccall(ccmalloc_(len))
# endif
# ifndef ccrealloc
#  define ccrealloc(mem,len) cccall(ccrealloc_(mem,len))
# endif
# ifndef ccfree
#  define ccfree(mem) cccall(ccfree_(mem))
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




ccfunc int ccrealfile(void *);
ccfunc void ccclosefile(void *);
ccfunc void *ccopenfile(const char *);
ccfunc void *ccpullfile(void *,unsigned long int,unsigned long int *);
ccfunc unsigned long int ccpushfile(void *,unsigned long int,unsigned long int,void*);
ccfunc unsigned long int ccfilesize(void *);

ccfunc ccinle ccclocktime_t ccclocktick();
ccfunc ccinle ccf64_t ccclocksecs(ccu64_t);

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



#ifdef _HARD_DEBUG

ccfunc void
cctimed_checkheapblock(ccdebug_t *timed, ccdebug_block_t *block)
{
  ccassert(!block->removed);

  ccassert(!block->prev||block->prev->next==block);
  ccassert(!block->next||block->next->prev==block);
  ccassert(!block->next||block->next!=block);
  ccassert(!block->prev||block->prev!=block);

  ccassert(block->next||block==timed->last_heap_block);
}


ccfunc void *
cctimed_addheapblock(ccdebug_block_t *block, cccaller_t caller, size_t size)
{
  ccdebug_t *timed=cctimed();

  memset(block,ccnil,sizeof(*block));

  block->prev=ccnil;
  block->next=ccnil;
  block->caller=caller;
  block->size=size;

  if(!timed->last_heap_block)
  { timed->last_heap_block=block;
  } else
  { block->prev=timed->last_heap_block;
    timed->last_heap_block->next=block;
    timed->last_heap_block=block;
  }

  cctimed_checkheapblock(timed,block);
  timed->event.heap_block_count++;

  return block+1;
}

ccfunc void
cctimed_remheapblock(ccdebug_block_t *block, cccaller_t caller)
{
  ccdebug_t *timed=cctimed();

  cctimed_checkheapblock(timed,block);
  timed->event.heap_block_count--;


  block->removed=cctrue;

  if(block->prev) block->prev->next=block->next;
  if(block->next) block->next->prev=block->prev;
  else timed->last_heap_block=block->prev;

  block->next=ccnil;
  block->prev=ccnil;
  block->caller=caller;
}

// Todo: have two sets of allocations functions, one for the user, and one for the debug info ...
// This way we can measure precisely how much time we've spend allocating ...

ccfunc void *ccmalloc_(size_t size)
{
  cccaller_t caller=ccpullcaller();

  ccclocktime_t tick=ccclocktick();
  ccdebug_t *timed=cctimed();

  ccstats()->allocations++;
  ccstats()->memory+=size;

  ccdebug_block_t *block=
    (ccdebug_block_t*)malloc(sizeof(*block)+size);

  cctimed_addheapblock(block,caller,size);

  cctimed_addoverhead(timed,tick);
  return block+1;
}

ccfunc void *ccrealloc_(void *data, size_t size)
{
  cccaller_t caller=ccpullcaller();

  ccclocktime_t tick=ccclocktick();
  ccdebug_t *timed=cctimed();

  ccstats()->reallocations++;

  ccdebug_block_t *block;
  if(data)
  { block=(ccdebug_block_t*)data-1;
    ccstats()->memory+=size-block->size;

    // Note: if we're reallocating, we're implying that there's at-least one block already in memory ...
    ccassert(timed->last_heap_block!=0);

    int is_last=block==timed->last_heap_block;

    block=(ccdebug_block_t*)realloc(block,sizeof(*block)+size);
    block->caller=caller;
    block->size=size;

    if(block->prev) block->prev->next=block;
    if(block->next) block->next->prev=block;

    if(is_last)
      timed->last_heap_block=block;
  } else
  { ccstats()->memory+=size;
    block=(ccdebug_block_t*)realloc(ccnil,sizeof(*block)+size);
    cctimed_addheapblock(block,caller,size);
  }

  cctimed_addoverhead(timed,tick);
  return block+1;
}

ccfunc void ccfree_(void *data)
{
  cccaller_t caller=ccpullcaller();

  ccclocktime_t tick=ccclocktick();
  ccdebug_t *timed=cctimed();

  ccdebug_block_t *block=
    (ccdebug_block_t*)data-1;

  ccstats()->deallocations++;
  ccstats()->memory-=block->size;

  cctimed_remheapblock(block,caller);

  free(block);

  cctimed_addoverhead(timed,tick);
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