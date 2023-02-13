/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved. **/
/*****************************************************************/
#ifndef _CCLOG
#define _CCLOG

// Note: ensure caller guid is never 0 ...



#endif

#if defined(_CCLOG) && defined(_CCLOG_IMPL)

#ifdef _HARD_DEBUG
ccfunc ccinle cccaller_t
cccaller(int guid, const char *file, int line, const char *func)
{ cccaller_t t;
  t.guid=guid;
  t.file=file;
  t.line=line;
  t.func=func;
  return t;
}
#endif

#ifdef _HARD_DEBUG



ccfunc void
ccpushcaller(cccaller_t caller)
{
	ccassert(!cchascaller);

  int stacksize=sizeof(cccallstack)/sizeof(cccallstack[0]);
  if(cccallindex>=stacksize)
  { cccallindex=stacksize-1;

    for(int i=0;i<stacksize-1;++i)
    {
      cccallstack[i]=cccallstack[i+1];
    }
  }

  cccallstack[cccallindex++]=caller;
  cchascaller=cctrue;
}

ccfunc ccinle cccaller_t
ccpullcaller()
{
	ccassert(cchascaller);
  cchascaller=ccfalse;

  ccassert(cccallindex>=1);

  return cccallstack[--cccallindex];
}

ccfunc ccdebug_sentry_t *
ccdebug_()
{
	cccaller_t caller=ccpullcaller();

  ccglobal ccdebug_sentry_t dummy={"dummy"};
	if(ccdebugnone) return &dummy;

	if(ccdebugthis) return ccdebugthis;

	if(!ccdebugroot.label)
	{ ccdebugroot.caller=caller;
    ccdebugroot.label="$root";
    ccdebugroot.event_count=1;
	  ccdebugroot.start_event_ticks=ccclocktick();
	}

	return &ccdebugroot;
}

ccfunc ccinle ccdebug_event_t *
ccevent_()
{
	// Note: call ccdebug_ so that it consumes our caller ...
  return &ccdebug_()->event;
}

ccfunc ccinle void
cctimed_bubble(ccdebug_sentry_t *timed)
{ if(!timed->super) return;
	cci64_t *a,*c,*s;
	a=timed->event.e;
	c=timed->last_event.e;
	s=timed->super->event.e;
	for(int i=0; i<5; ++i)
	{ *s+++=*a-*c;
		*c++=*a++;
	}
}

ccfunc ccinle ccf64_t
ccclockperc(ccclocktime_t total, ccclocktime_t local)
{
	return 100.0 * ( cccast(ccf64_t,local) /
								   cccast(ccf64_t,total) );
}

ccfunc const char *
ccbytecountreadable(cci64_t b, ccf64_t *f)
{
	if(b>(1024*1024*1024))
	{ *f=b/(1024.0*1024.0*1024.0);
		return "Gb";
	} else
	if(b>(1024*1024))
	{ *f=b/(1024.0*1024.0);
		return "Mb";
	} else
	if(b>(1024))
	{ *f=b/(1024.0);
		return "Kb";
	} else
	{
		*f=(ccf64_t)b;
		return "b";
	}
}

// 0        (no-leaks) (sealed)
//  ++5
//    ++10
//    ++10
//  --15
// --5 (sealed)

// ++10
//  ++10
//   ++10
//   ++10
//  --10 (+10)
//  ++10
//   ++10
//   ++10
//  --10 (+10)
// --10 (+10)
#include "cccolor.c"

ccfunc void
ccdebugdump_(
	ccdebug_sentry_t  * r,
	ccdebug_sentry_t  * h,
	ccdebug_sentry_t  * t)
{
	ccf64_t seconds_used,percent_used;
  seconds_used=ccclocksecs(t->total_event_ticks);
  percent_used=ccclockperc(r? r->total_event_ticks :t->total_event_ticks,t->total_event_ticks);

  size_t heap_blocks=t->event.heap_block_count;
 	const char *leak_string="deferred";
 	if(!r) leak_string="leaked";

  ccf64_t memory;
  const char *suffix;
  suffix=ccbytecountreadable(t->event.memory,&memory);

	for(int i=0;i<t->level*1;++i) printf(" ");
  ccprintf("<!%i#%i in %s[%i] %s(): %i <!6'%s'!> event(s), took %f(s) <!9(%%%f)!> with [%lli-%lli,%lli] allocations <!%i%s %f%s!> in %lli block(s), %lli collision(s)!>\n",
   	h==t?4:h?5:7,
    t->caller.guid,ccfilename(t->caller.file),t->caller.line,t->caller.func,
      t->event_count,t->label,seconds_used,percent_used,
        t->event.allocations,t->event.deallocations,t->event.reallocations,
        	t->event.memory>0?14:7,leak_string,memory,suffix,heap_blocks,t->event.collisions);
  if(h==t && t!=&ccdebugroot) cctextreset();


  ccdebug_sentry_t *c;

  ccdebug_sentry_t *n=ccnil;

  if(h==t)
  { ccarrfor(t->child,c)
	  	if(!n||c->total_event_ticks>n->total_event_ticks) n=c;
  }

  ccarrfor(t->child,c)
    ccdebugdump_(t,n,c);
}




ccfunc ccinle void
ccdebugdump()
{
	cctracelog("Debug Dump: ");
	ccdebugdump_(ccnil,&ccdebugroot,&ccdebugroot);

	ccdebugnone=cctrue;


	int freed_count=0;

	ccheap_block_t *f;
	for(ccheap_block_t *i=ccdebugroot.last_heap_block;i;freed_count++)
	{
		f=i;
		i=i->prev;

		ccfree(f+1);
	}

	cctracelog("freed %i block(s) for you", freed_count);
}

ccfunc void
ccenter_callme(const char *label)
{ cccaller_t caller=ccpullcaller();
  if(ccdebugnone) return;
 	ccassert(!ccnopushrob);
	ccnopushrob=cctrue;

	ccu64_t tick=ccclocktick();

	ccdebug_sentry_t *super=ccdebug();

	ccallocator_t *a=ccallocator;
	int d=ccdebugnone;

	ccdebugnone=cctrue;
	ccallocator=ccinternalallocator_;

  ccdebugthis=cctblsetP(super->child,cccast(cci64_t,1+caller.guid));

  ccdebugnone=d;
  ccallocator=a;

  if(ccdebugthis->event_count==0)
  { ccdebugthis->caller=caller;
    ccdebugthis->super=super;
    ccdebugthis->label=label;
    ccdebugthis->level=super->level+1;
  }
  ccdebugthis->event_count++;
  ccdebugthis->start_event_ticks=tick;
  ccnopushrob=ccfalse;
}

ccfunc void
ccleave_callme(const char *label)
{ cccaller_t caller=ccpullcaller();
  if(ccdebugnone) return;
  ccassert(!ccnopushrob);
	ccnopushrob=cctrue;
  ccassert(ccdebugthis!=0);
  ccassert(strcmp(ccdebugthis->label,label)==0);
  ccassert(strcmp(ccdebugthis->caller.file,caller.file)==0);
  ccassert(strcmp(ccdebugthis->caller.func,caller.func)==0);
	ccu64_t tick=ccclocktick();
  ccclocktime_t span=tick-ccdebugthis->start_event_ticks;
  ccdebugthis->total_event_ticks+=span;
  cctimed_bubble(ccdebugthis);
  ccdebugthis=ccdebugthis->super;
  ccnopushrob=ccfalse;
}
#endif

ccfunc void
ccdebugend()
{
#ifdef _HARD_DEBUG
	ccassert(ccdebugthis==&ccdebugroot);
	cccallas(ccdebugroot.caller,ccleave_callme("$root"));
	ccdebugdump();
#endif
}


ccfunc void
ccout(const char *string)
{
#if defined(_DEBUG) && defined(_WIN32)
  OutputDebugStringA(string);
#endif
  printf(string);
}

ccfunc void
ccoutnl(const char *string)
{
  ccout(string);
  ccout("\r\n");
}

ccfunc void
cctrace_(const char *label, const char *format, ...)
{
	cccaller_t caller=ccpullcaller();

  va_list vli;
  va_start(vli,format);

  ccglobal ccthread_local char buf[0xfff];
  int len=1+ccformatvex(buf,0xff,format,vli);

  int rem=sizeof(buf)-len;
  ccformatex(buf+len,rem,"%s: %s[%i] %s() %s\n",
  	label,caller.file,caller.line,caller.func,buf);

  ccout(buf+len);

  va_end(vli);
}

#endif