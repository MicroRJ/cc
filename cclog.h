/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved. **/
/*****************************************************************/
#ifndef _CCLOG
#define _CCLOG

// Note: ensure caller guid is never 0 ...



#endif

#if defined(_CCLOG) && defined(_CCLOG_IMPL)

ccfunc ccinle cccaller_t
cccaller(int guid, const char *file, int line, const char *func)
{
  cccaller_t t;
  t.guid=guid;
  t.file=file;
  t.line=line;
  t.func=func;

  return t;
}

ccfunc void
ccpushcaller_(cccaller_t caller)
{
  int stacksize=sizeof(cccallstack)/sizeof(cccallstack[0]);
  if(cccallindex>=stacksize)
  { cccallindex=stacksize-1;

    for(int i=0;i<stacksize-1;++i)
    {
      cccallstack[i]=cccallstack[i+1];
    }
  }

  cccallstack[cccallindex++]=caller;
}

ccfunc ccinle cccaller_t
ccpullcaller()
{
  ccassert(cccallindex>=1);

  return cccallstack[--cccallindex];
}

ccfunc ccinle ccdebug_t *
cctimed()
{
  ccglobal ccdebug_t dummy;
  return cctimed_enabled && ccdebugthis? ccdebugthis :&dummy;
}

ccfunc ccinle ccdebug_event_t *
ccstats()
{
  return &cctimed()->event;
}


ccfunc ccinle void
cctimed_addoverhead(ccdebug_t *timed, ccclocktime_t start)
{
	timed->event.overhead_clock_ticks += ccclocktick()-start;
}

ccfunc ccinle void
cctimed_bubble(ccdebug_t *timed)
{ if(!timed->super)
		return;

	size_t *a,*c,*s;
	a=timed->event.e;
	c=timed->last_event.e;
	s=timed->super->event.e;

	for(int i=0; i<5; ++i)
	{ // Note: calculate difference between actual and cached, add to super ...
		*s++ += *a - *c;
		// Note: update cached with actual
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
ccbytecountreadable(ccu64_t b, ccf64_t *f)
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

ccfunc void
cctimeddump_(ccdebug_t *r, ccdebug_t *t)
{

  ccclocktime_t overhead_clock_ticks;
  overhead_clock_ticks=t->event.overhead_clock_ticks;

  ccf64_t seconds_used,percent_used;
  seconds_used=ccclocksecs(t->total_event_ticks-overhead_clock_ticks);
  percent_used=ccclockperc(r? r->total_event_ticks :t->total_event_ticks,t->total_event_ticks);

  size_t heap_blocks=t->event.heap_block_count;

  // ccdebug_block_t *block;
 	// for(block=t->last_heap_block;block;block=block->prev)
 	// {
 	//    heap_blocks++;
 	// }

 	const char *leak_string;
 	if(!r) leak_string="leaked";
 	else leak_string="deferred";

  ccf64_t memory;
  const char *suffix;
  suffix=ccbytecountreadable(t->event.memory,&memory);

	for(int i=0;i<t->level*2;++i)
    printf(" ");
  printf("#%i in %s[%i] %s(): %i '%s' event(s), took %f(s)(%%%.2f) also [%lli-%lli,%lli]=%.2f%s %s in %lli block(s), %lli collision(s)\n",
    t->caller.guid,t->caller.file,t->caller.line,t->caller.func,
      t->event_count,t->label,seconds_used,percent_used,
        t->event.allocations,t->event.deallocations,t->event.reallocations,
        	memory,suffix,leak_string,heap_blocks,t->event.collisions);

  ccdebug_t *c;
  ccarrfor(t->child,c)
    cctimeddump_(t,c);
}

ccfunc void
cctimeddump(ccdebug_t *r, ccdebug_t *t)
{
	cctimeddump_(r,t);
}

ccfunc void
cctimedhead_(const char *label)
{
  if(!cctimed_enabled)
    return;

	ccu64_t tick=ccclocktick();

  ccdebug_t *super=ccdebugthis;

  cccaller_t caller=ccpullcaller();

  if(!ccdebugthis)
  {
    ccdebugthis=&ccdebugroot;
  } else
  {
    cctimed_enabled=ccfalse;
    ccdebugthis=cctblsetP(ccdebugthis->child,cccast(cci64_t,caller.guid));
    cctimed_enabled=cctrue;
  }


  if(ccdebugthis->event_count==0)
  { ccdebugthis->caller=caller;
    ccdebugthis->super=super;
    ccdebugthis->label=label;
    ccdebugthis->level=super?super->level+1:0;
  }

  ccdebugthis->event_count++;

  ccdebugthis->start_event_ticks=tick;

  cctimed_addoverhead(ccdebugthis,tick);
}



ccfunc void
cctimedtail_(const char *label)
{
  if(!cctimed_enabled)
    return;

	ccu64_t tick=ccclocktick();

  ccassert(ccdebugthis!=0);

  cccaller_t caller=ccpullcaller();

  ccassert(strcmp(ccdebugthis->caller.file,caller.file)==0);
  ccassert(strcmp(ccdebugthis->caller.func,caller.func)==0);

  ccclocktime_t span=tick-ccdebugthis->start_event_ticks;
  ccdebugthis->total_event_ticks+=span;

  cctimed_addoverhead(ccdebugthis,tick);

  cctimed_bubble(ccdebugthis);

  ccdebugthis=ccdebugthis->super;

  if(!ccdebugthis)
  {
    printf("timed:\n");
    cctimeddump(ccnil,&ccdebugroot);
  }
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
cctrace_(int guid, const char *file, int line, const char *func, const char *tag, const char *fmt, ...)
{
  (void)guid;

  va_list vli;
  va_start(vli, fmt);

  static char buf[0xfff];
  int len=1+ccformatvex(buf,0xff,fmt,vli);
  int rem=sizeof(buf)-len;
  ccformatex(buf+len,rem,"%s: %s[%i] %s() %s\n",tag,file,line,func,buf);
  ccout(buf+len);

  va_end(vli);
}

#endif