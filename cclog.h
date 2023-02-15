/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved. **/
/*****************************************************************/
#ifndef _CCLOG
#define _CCLOG
#endif

#if defined(_CCLOG) && defined(_CCLOG_IMPL)

ccfunc ccinle cccaller_t
cccaller(int guid, const char *file, int line, const char *func)
{ cccaller_t t;
  t.guid=guid;
  t.file=file;
  t.line=line;
  t.func=func;
  return t;
}

#ifdef _HARD_DEBUG

ccfunc ccsentry_t *
ccdebug_()
{ ccglobal ccsentry_t dummy={"dummy"};
  if(ccdebugnone) return &dummy;

  ccassert(ccdebugthis!=ccnil);

  return ccdebugthis;
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
#include "cccolor.c"

ccfunc void
ccsentry_report(
  ccsentry_t  * r,
  ccsentry_t  * h,
  ccsentry_t  * t)
{
  // Todo: percentage is not as useful ...
  ccf64_t seconds_used,percent_used;
  seconds_used=ccclocksecs(t->total_time_in_ticks);
  percent_used=ccclockperc(r? r->total_time_in_ticks :t->total_time_in_ticks,t->total_time_in_ticks);

  const char *leak_string="deferred";
  if(!r) leak_string="leaked";

  for(int i=0;i<t->level*1;++i) printf(" ");

#if 1
  ccf64_t memory;
  const char *suffix;
  suffix=ccbytecountreadable(t->metrics.pm-t->metrics.nm,&memory);
  ccprintf("<!%i#%i in %s[%i] %s(): %i <!6'%s'!> event(s), took %f(s) <!9(%%%f)!> with [%lli-%lli] @ <!%i%s %f%s!> in %i block(s)!>\n",
    h==t?4:7,
    t->caller.guid,ccfilename(t->caller.file),t->caller.line,t->caller.func,
      t->enter_count,t->marker,seconds_used,percent_used,
        t->metrics.pma,t->metrics.nma,
          t->metrics.pm-t->metrics.nm>0?14:7,
            leak_string,memory,suffix,t->metrics.pbc-t->metrics.nbc);
#else
  printf("#%i %s [%lli-%lli] %lli\n",
    t->caller.guid,t->marker,
      t->metrics.pma,t->metrics.nma,
        t->metrics.pm-t->metrics.nm);
#endif

  ccsentry_t *n=0,*i;

  if(h==t)
  { n=t->slaves;

    ccarrfor(t->slaves,i)
      if(i->total_time_in_ticks>n->total_time_in_ticks) n=i;
  }

  ccarrfor(t->slaves,i)
    ccsentry_report(t,n,i);
}

ccfunc ccinle void
ccdebugdump()
{
  cctracelog("Debug Dump: ");

  // Note: this is dumb!
  ccclocktime_t t=0;

  ccdebugroot.caller=cccall();
  ccdebugroot.marker="root-master";
  ccsentry_t *s;
  ccarrfor(ccdebugroot.slaves,s) t+=s->total_time_in_ticks;
  ccdebugroot.total_time_in_ticks=t;

  ccsentry_report(ccnil,&ccdebugroot,&ccdebugroot);

  ccdebugnone=cctrue;

  int freed_count=0;

  ccsentry_block_t *f;
  for(ccsentry_block_t *i=ccdebugroot.block_list;i;freed_count++)
  {
    f=i;
    i=i->prev;

    ccfree(f+1);
  }

  cctracelog("freed %i block(s) for you", freed_count);
}


ccfunc ccsentry_block_t *
ccsentry_remblock(ccsentry_t *sentry, ccsentry_block_t *block)
{ ccassert(block->sentry==ccdebugthis);

  if(block->prev) block->prev->next=block->next;
  if(block->next) block->next->prev=block->prev;
  else sentry->block_list=block->prev;

  block->prev=ccnull;
  block->next=ccnull;

  return block;
}

ccfunc ccsentry_block_t *
ccsentry_addblock(ccsentry_t *sentry, ccsentry_block_t *block)
{
  if(sentry->block_list)
  { block->prev=sentry->block_list;
    sentry->block_list->next=block;
  }
  sentry->block_list=block;
  block->sentry=sentry;
  return block;
}

ccfunc void
ccini()
{ ccdebugthis=&ccdebugroot;
}

ccfunc ccsentry_t *
ccsentry_enter(cccaller_t caller, ccsentry_t *master, const char *marker)
{ if(ccdebugnone) return master;

	// Note: this is a recursive function ...
	if(caller.guid==master->caller.guid)
	{
		ccassert(ccdebugthis==master);

		master->enter_count++;
		return master;
	}

  ccallocator_t *a=ccallocator;
  int d=ccdebugnone;

  ccdebugnone=cctrue;
  ccallocator=ccinternalallocator_;

  ccsentry_t *sentry=cctblsetP(master->slaves,cccast(cci64_t,1+caller.guid));

  ccdebugnone=d;
  ccallocator=a;

  if(!sentry->enter_count++)
  { ccassert(sentry!=ccnil);
    ccassert(master!=ccnil);
    ccassert(marker!=ccnil);

    sentry->caller=caller;
    sentry->master=master;
    sentry->marker=marker;
    sentry->slaves=ccnull;

    sentry->level=master->level+1;

  	sentry->start_time_in_ticks=0;
  	sentry->total_time_in_ticks=0;
  }

  sentry->start_time_in_ticks=ccclocktick();
  return sentry;
}

ccfunc ccsentry_t *
ccsentry_leave(cccaller_t caller, ccsentry_t *sentry, const char *marker)
{
  if(ccdebugnone) return sentry;

  ccassert(sentry!=0);

  ccassert(strcmp(sentry->marker,marker)==0);
  ccassert(strcmp(sentry->caller.file,caller.file)==0);
  ccassert(strcmp(sentry->caller.func,caller.func)==0);
  ccassert(sentry->enter_count>sentry->leave_count);

  sentry->leave_count++;

  // Note: this could be a recursive function and we've entered a bunch of times but we haven't left yet ...
  if(sentry->leave_count!=sentry->enter_count)
  {
  	return sentry;
  }

  ccu64_t tick=ccclocktick();
  sentry->total_time_in_ticks+=tick-sentry->start_time_in_ticks;

  sentry->master->metrics.pma+=sentry->metrics.pma-sentry->last_metrics.pma;
  sentry->master->metrics.nma+=sentry->metrics.nma-sentry->last_metrics.nma;
  sentry->master->metrics.pm+=sentry->metrics.pm-sentry->last_metrics.pm;
  sentry->master->metrics.nm+=sentry->metrics.nm-sentry->last_metrics.nm;
  sentry->master->metrics.pbc+=sentry->metrics.pbc-sentry->last_metrics.pbc;
  sentry->master->metrics.nbc+=sentry->metrics.nbc-sentry->last_metrics.nbc;

  sentry->last_metrics.pma=sentry->metrics.pma;
  sentry->last_metrics.nma=sentry->metrics.nma;
  sentry->last_metrics.pm=sentry->metrics.pm;
  sentry->last_metrics.nm=sentry->metrics.nm;
  sentry->last_metrics.pbc=sentry->metrics.pbc;
  sentry->last_metrics.nbc=sentry->metrics.nbc;

  // Note:
  ccsentry_block_t *block;
  for(block=sentry->block_list;block;)
  { ccsentry_block_t *remove=block;
    block=block->prev;

    // Todo: are you silly? you don't have to "remove" each block, you're given them all off anyways ...
    ccsentry_addblock(sentry->master,ccsentry_remblock(sentry,remove));
  }

  return sentry->master;
}
#endif

ccfunc void
ccdebugend()
{
#ifdef _HARD_DEBUG
  ccassert(ccdebugthis==&ccdebugroot);
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
cctrace_(cccaller_t caller, const char *label, const char *format, ...)
{
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