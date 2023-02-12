/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
#ifndef _CC
#define _CC

// Todo: pending string arena ...
// Todo: custom allocator support, custom alignment support ...

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


// Note: universal error codes
typedef enum ccerr_k
{
  ccerr_kNON=0,
  ccerr_kNIT=1,
  ccerr_kAIT=2,
  ccerr_kOOM=3,
  ccerr_kIUA=4,
} ccerr_k;

ccglobal const char * const ccerr_s[]=
{ "none",
  "not in table",
  "already in table",
  "out of memory",
  "invalid user argument",
};

// Note: This is a helper type for when you want to make it abundantly clear
// that there's metadata associated with the string ..
typedef char *ccstr_t;

// Note: entity state, nit: not in table, ait: already in table ...
typedef enum ccent_k
{
	ccent_kNIT=0,
  ccent_kAIT=1,
} ccent_k;

// Note: debugging utilities
typedef struct cccaller_t cccaller_t;
typedef struct cccaller_t
{ int           guid;
  const char   *file;
  int           line;
  const char   *func;
} cccaller_t;

typedef void *(ccalloctr_t)(size_t,void *);

// Note: general table entry
typedef struct ccent_t ccent_t;
typedef struct ccent_t
{ ccent_t * nex;
  cci32_t   len;
  char    * key;
  size_t    val;
} ccent_t;

// Note: static length buffer, slb for short... slb's are use for regular heap allocations ...
typedef struct ccslb_t ccslb_t;
typedef struct ccslb_t
{ cccaller_t  caller;
	ccslb_t    *prev, *next;
  size_t      size;
  char        guard[0x04];
} ccslb_t;

// Note: dynamic length buffer, dlb for short ... each dlb will also support a hash-table ...
// Note: At the moment, slb's and dlb's are NOT integrated, for instance, a dlb is intended to be a
// superset of a slb, so you'd allocate a slb, and then add dlb metadata:
//
// ccslb_t
// ccdlb_t
// head_bytes
// user_bytes
// tail_bytes
//
// This is only if the allocator adds the slb metadata, (the default one does) however,
// I do not expect - you, the user to use the provided allocator, in which case, dlb's have a sze_max field.
//
// Point is, if we're going to fully commit to this whole slb dlb style of memory management and introspection,
// we could save some overhead memory by not storing a sze_max and making entries be part of slb, which would,
// enable hashing on a fixed sized buffer and dynamic ones as well.
//
// Note: It is important to note that in practice dlb's are entirely independent of slb's, it is simply
// functionality wise that their relation is meaningful... by that I mean that the only difference between an slb
// and dlb is the functionality that is related to each which in most cases overlaps.
//
// Todo: be a little more conservative with the size_t's
typedef struct ccdlb_t ccdlb_t;
typedef struct ccdlb_t
{ unsigned        rem_add: 1;
  unsigned        rem_rze: 1;
  ccalloctr_t * alloctr;
  ccent_t *       entries;
  size_t          sze_max;
  size_t          sze_min;
} ccdlb_t;

// Note: each debug event contains a set of metrics that can be used to diff the next event's metrics,
// this makes it relatively fast to bubble child metrics, although realistically, you'd do this at the end
// for no perfomance cost however comma we're not smart...
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
  };
} ccdebug_event_t;

typedef struct ccdebug_t ccdebug_t;
typedef struct ccdebug_t
{ const char * label;

  ccdebug_t  * super;
  ccdebug_t  * child; // hash-map

  cccaller_t   caller;
  cci32_t      level;

  ccclocktime_t start_event_ticks;
  ccclocktime_t total_event_ticks;

  ccdebug_event_t last_event;

  cci32_t         event_count;
  ccdebug_event_t event;

  // Note: only the root debug context may have heap blocks ...
  ccslb_t *last_heap_block;
} ccdebug_t;


// Todo: enhance these assertions ...
#ifndef ccassert
# ifdef _DEBUG
#  define ccassert(is_true,...) (!(is_true)?ccbreak():ccnil)
# else
#  define ccassert(is_true,...) ccnil
# endif
#endif

#ifndef ccnotnil
# ifdef _DEBUG
#  define ccnotnil(val) (ccassert((val)!=ccnil),val)
# else
#  define ccnotnil(val) (val)
# endif
#endif

ccfunc void *ccuseralloctr_(size_t,void*);
ccfunc void *ccinternalalloctr_(size_t,void*);

// Note:
ccglobal ccthread_local ccclocktime_t ccclockfreqc;

// Note: do not access these directly ...
ccglobal ccthread_local cccaller_t  cccallstack[0x04];
ccglobal ccthread_local cci32_t     cccallindex;
ccglobal ccthread_local cci32_t     cchascaller;

// Note: do not access these directly ... use the ccdebug() and ccevent() functions instead ...
ccglobal ccthread_local ccdebug_t   ccdebugroot;
ccglobal ccthread_local ccdebug_t  *ccdebugthis;

// Note: enable debug features, if you're going to use this, make sure you restore it properly by stacking the previous state ...
ccglobal ccthread_local cci32_t     ccdebug_disabled;

// Note: global error code
ccglobal ccthread_local ccerr_k ccerr;

// Note: global key
ccglobal ccthread_local ccstr_t cckey;

// Note: you have to stack this manually if you wish to change it
ccglobal ccthread_local ccalloctr_t *ccalloctr=ccuseralloctr_;

// Note: global error
#define ccerrset(err) ((ccerr=err),0)
#define ccerrnon()    ((ccerr)==ccerr_kNON)
#define ccerrsom()    ((ccerr)!=ccerr_kNON)
#define ccerrnit()    ((ccerr)==ccerr_kNIT)
#define ccerrait()    ((ccerr)==ccerr_kAIT)

// Note: global key
#define cckeyset(key) (cckey=key)
#define cckeyget()    (cckey)


// Note: C string utils ...
#ifndef ccstrlenS
# define ccstrlenS(cstr) cccast(ccu32_t,strlen(cstr))
#endif
#ifndef ccstrlenL
# define ccstrlenL(lstr) cccast(ccu32_t,sizeof(lstr)-1)
#endif

// Note: debug caller mechanism, this useful because it enables you to enhance debug data without
// having to modify your code too much ...
ccfunc ccinle cccaller_t cccaller(int guid, const char *file, int line, const char *func);
ccfunc void ccpushcaller(cccaller_t);


// Note: ensure that this is the first intruction in your function, before anything else ... pretty please ...
ccfunc ccinle cccaller_t ccpullcaller();

#ifndef cccallee
# define cccallee ccfunc
#endif
// Note: the function that you're calling must pull the caller ...
#ifndef cccallas
# define cccallas(caller,callee) (ccpushcaller(caller),(callee))
#endif
#ifndef cccall
# define cccall(callee) cccallas(cccaller(__COUNTER__,_CCFILE,_CCLINE,_CCFUNC),callee)
#endif

ccfunc ccinle ccdebug_event_t *ccevent_();
ccfunc ccinle ccdebug_t       *ccdebug_();

#ifndef ccdebug
# define ccdebug() cccall(ccdebug_())
#endif
#ifndef ccevent
# define ccevent() cccall(ccevent_())
#endif


// Note: simple trace logging ...
cccallee void cctrace_(const char *label, const char *format, ...);

#ifndef cctracelog
# define cctracelog(fmt,...) (cccall(0),cctrace_("log",fmt,__VA_ARGS__))
#endif
#ifndef cctracewar
# define cctracewar(fmt,...) (cccall(0),cctrace_("war",fmt,__VA_ARGS__))
#endif
#ifndef cctraceerr
# define cctraceerr(fmt,...) (cccall(0),cctrace_("err",fmt,__VA_ARGS__))
#endif

// Note: the idea is that the function you're calling knows that a caller will be put on the stack,
// and so that function must call ccpullcaller. Such functions shall end in '_', for instance 'ccmalloc_', there should be
// an accompanying macro 'ccmalloc' that pushes the caller onto the stack automatically.

// Note:

#ifndef ccmalloc
# define ccmalloc(size) cccall(ccuseralloctr_(size,ccnil))
#endif
#ifndef ccrealloc
# define ccrealloc(data,size) cccall(ccuseralloctr_(size,data))
#endif
#ifndef ccfree
# define ccfree(data) cccall(ccuseralloctr_(ccnil,data))
#endif

// Note:
// Todo: rename these
ccfunc void cctimedhead_(const char *label);
ccfunc void cctimedtail_(const char *label);

#ifndef cctimedhead
# define cctimedhead(label) cccall(cctimedhead_(label))
#endif
#ifndef cctimedtail
# define cctimedtail(label) cccall(cctimedtail_(label))
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

#ifdef _HARD_DEBUG

ccfunc void
ccdebug_checkblock(ccslb_t *block)
{
	ccassert(!block->prev||block->prev->next==block);
  ccassert(!block->next||block->next->prev==block);
  ccassert(!block->next||block->next!=block);
  ccassert(!block->prev||block->prev!=block);
}

ccfunc void *ccuseralloctr_(size_t size,void *data)
{
cccaller_t caller=ccpullcaller();

cctimedhead("user-allocator");
  ccdebug_t *debug=ccdebug();

	ccslb_t *block;
	if(!size)
	{ block=(ccslb_t*)data-1;

		ccdebug_checkblock(block);
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
	} else
	{ if(data)
		{ block=(ccslb_t*)data-1;

  		ccdebug_checkblock(block);

	    debug->event.memory+=size-block->size;

	    ccassert(ccdebugroot.last_heap_block!=0);
	    int is_last=block==ccdebugroot.last_heap_block;
	    block=(ccslb_t*)realloc(block,sizeof(*block)+size);
	    block->caller=caller;
	    block->size=size;
	    if(block->prev) block->prev->next=block;
	    if(block->next) block->next->prev=block;
	    if(is_last) ccdebugroot.last_heap_block=block;
		} else
		{ block=(ccslb_t*)malloc(sizeof(*block)+size);
  		memset(block,ccnil,sizeof(*block));

  		block->caller=caller;
  		block->size=size;

		  if(!ccdebugroot.last_heap_block)
		  { ccdebugroot.last_heap_block=block;
		  } else
		  { block->prev=ccdebugroot.last_heap_block;
		    ccdebugroot.last_heap_block->next=block;
		    ccdebugroot.last_heap_block=block;
		  }
  		ccdebug_checkblock(block);

  		debug->event.heap_block_count++;
  		debug->event.allocations++;
		  debug->event.memory+=size;
		}
	}
cctimedtail("user-allocator");
	return block+1;
}

ccfunc void *ccinternalalloctr_(size_t size,void *data)
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