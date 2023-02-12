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


ccfunc ccinle ccdebug_t *
ccdebug_()
{
	cccaller_t caller=ccpullcaller();

  ccglobal ccdebug_t dummy={"dummy"};
	if(ccdebug_disabled) return &dummy;
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
cctimed_bubble(ccdebug_t *timed)
{ if(!timed->super)
		return;

	size_t *a,*c,*s;
	a=timed->event.e;
	c=timed->last_event.e;
	s=timed->super->event.e;

	for(int i=0; i<6; ++i)
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
cctextcolor()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED);
}

ccfunc void
cctextreset()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
}

ccfunc void
ccdebugdump_(ccdebug_t *r, ccdebug_t *h, ccdebug_t *t)
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

  if(h==t && t!=&ccdebugroot) cctextcolor();
	for(int i=0;i<t->level*4;++i) printf(" ");
  printf("#%i in %s[%i] %s(): %i '%s' event(s), took %f(s)(%%%.2f) with [%lli-%lli,%lli] allocations %s %.2f%s in %lli block(s), %lli collision(s)\n",
    t->caller.guid,ccfilename(t->caller.file),t->caller.line,t->caller.func,
      t->event_count,t->label,seconds_used,percent_used,
        t->event.allocations,t->event.deallocations,t->event.reallocations,
        	leak_string,memory,suffix,heap_blocks,t->event.collisions);
  if(h==t && t!=&ccdebugroot) cctextreset();


  ccdebug_t *c;

  ccdebug_t *n=ccnil;

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
	cctracelog("Debug Dump:");
	ccdebugdump_(ccnil,&ccdebugroot,&ccdebugroot);
}

ccfunc void
cctimedhead_(const char *label)
{ cccaller_t caller=ccpullcaller();
  if(ccdebug_disabled) return;
	ccu64_t tick=ccclocktick();
	ccdebug_t *super=ccdebug();

	ccalloctr_t *was_alloctr=ccalloctr;
	int was_disabled=ccdebug_disabled;
	ccdebug_disabled=cctrue;
	ccalloctr=ccinternalalloctr_;
  ccdebugthis=cctblsetP(super->child,cccast(cci64_t,1+caller.guid));
  ccdebug_disabled=was_disabled;
  ccalloctr=was_alloctr;
  if(ccdebugthis->event_count==0)
  { ccdebugthis->caller=caller;
    ccdebugthis->super=super;
    ccdebugthis->label=label;
    ccdebugthis->level=super->level+1;
  }
  ccdebugthis->event_count++;
  ccdebugthis->start_event_ticks=tick;
}

ccfunc void
cctimedtail_(const char *label)
{ cccaller_t caller=ccpullcaller();
  if(ccdebug_disabled) return;
	ccu64_t tick=ccclocktick();
  ccassert(ccdebugthis!=0);
  ccassert(strcmp(ccdebugthis->label,label)==0);
  ccassert(strcmp(ccdebugthis->caller.file,caller.file)==0);
  ccassert(strcmp(ccdebugthis->caller.func,caller.func)==0);
  ccclocktime_t span=tick-ccdebugthis->start_event_ticks;
  ccdebugthis->total_event_ticks+=span;
  cctimed_bubble(ccdebugthis);
  ccdebugthis=ccdebugthis->super;

  if(ccdebugthis==&ccdebugroot)
  {
  	cccallas(ccdebugroot.caller,cctimedtail_("$root"));
  	ccdebugdump();
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