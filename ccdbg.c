// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCDBG_C
#define _CCDBG_C

// Note: this is just a mess of stuff that I have to organize ...

// Todo: remove this from here!

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

ccfunc ccinle ccf64_t
ccclockperc(ccclocktick_t total, ccclocktick_t local)
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

// Todo: dude, what is this mess
#include "cccolor.c"

ccfunc void
cccheck_ward(ccallocator_t *allocator, ccward_t *block)
{ for(int i=0; i<4; ++i)
    ccassert(block->head_guard[i]==ccnull ||
      cctraceerr("corrupted heap block, head guard breached",0));
  for(int i=0; i<4; ++i)
    ccassert(block->tail_guard[i]==ccnull ||
      cctraceerr("corrupted heap block, tail guard breached",0));

  ccassert(block->master!=ccnull);
  ccassert(block->sentry!=ccnull);

  ccassert(block->allocator==allocator);

  // ccassert(!block->prev||block->prev->sentry==block->sentry);
  // ccassert(!block->next||block->next->sentry==block->sentry);

  ccassert(!block->prev||block->prev->next==block);
  ccassert(!block->next||block->next->prev==block);

  ccassert(!block->next||block->next!=block);
  ccassert(!block->prev||block->prev!=block);
}

ccfunc void *ccinternalallocator_(cccaller_t caller, size_t size,void *data)
{ (void)caller;

	if(size)
  { if(data)
      return realloc(data,size);
    else
      return malloc(size);
  }
  free(data);
  return ccnil;
}

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
#if 1
  cctracelog("Debug Dump: ",0);

  // Note: this is dumb!
  ccclocktick_t t=0;

  cc.debug_root.caller=cccall();
  cc.debug_root.marker="root-master";
  ccsentry_t *s;
  ccarrfor(cc.debug_root.slaves,s) t+=s->total_time_in_ticks;
  cc.debug_root.total_time_in_ticks=t;

  ccsentry_report(ccnil,&cc.debug_root,&cc.debug_root);

  cc.debug_none=cctrue;

  int freed_count=0;

  ccward_t *f;
  for(ccward_t *i=cc.wards;i;freed_count++)
  {
    f=i;
    i=i->prev;

    ccfree(f+1);
  }

  cctracelog("freed %i block(s) for you", freed_count);
#endif
}

ccfunc void *ccuserallocator_(cccaller_t caller, size_t size,void *data)
{
  ccward_t *ward=ccnull;
  if(!size)
  {
    if(data)
    {
      ccsentry_t *sentry=cc.debug_this;
      ward=(ccward_t*)data-1;
      cccheck_ward(ccuserallocator_,ward);

      if(ward->prev) ward->prev->next=ward->next;
      if(ward->next) ward->next->prev=ward->prev;
      else cc.wards=ward->prev;

      sentry->metrics.nbc++;
      sentry->metrics.nma++;
      sentry->metrics.nm+=ward->size;

      // Note: just in case?
      ward->next=ccnull;
      ward->prev=ccnull;
      free(ward);
    }
  } else
  { if(data)
    {
      ward=(ccward_t*)data-1;
      cccheck_ward(ccuserallocator_,ward);

      cc.debug_this->metrics.nm+=ward->size;

      ward=(ccward_t*)realloc(ward,sizeof(*ward)+size);
      ward->caller=caller;
      ward->size=size;

      if(ward->prev) ward->prev->next=ward;
      if(ward->next) ward->next->prev=ward;
      else cc.wards=ward;

      cc.debug_this->metrics.pma++;
      cc.debug_this->metrics.nma++;
      cc.debug_this->metrics.pm+=size;
    } else
    {
      ward=(ccward_t*)malloc(sizeof(*ward)+size);
      memset(ward,ccnull,sizeof(*ward));

      ward->allocator=ccuserallocator_;
      ward->caller=caller;
      ward->master=cc.debug_this;
      ward->sentry=cc.debug_this;
      ward->size=size;

      if(cc.wards)
		  { ward->prev=cc.wards;
		    cc.wards->next=ward;
		  }
		  cc.wards=ward;
		  ward->sentry=cc.debug_this;

      cccheck_ward(ccuserallocator_,ward);

      cc.debug_this->metrics.pbc++;
      cc.debug_this->metrics.pma++;
      cc.debug_this->metrics.pm+=size;
    }
  }
  return ward+1;
}

ccfunc ccsentry_t *
ccsentry_enter(cccaller_t caller, ccsentry_t *master, const char *marker)
{
	if(cc.debug_none) return master;

  // Note: this is a recursive function ...
  if(caller.guid==master->caller.guid)
  {
    ccassert(cc.debug_this==master);

    master->enter_count++;
    return master;
  }

  ccpush_allocator();
  ccmove_allocator(ccinternalallocator_);

  int d=cc.debug_none;
  cc.debug_none=cctrue;

  ccsentry_t *sentry=cctblsetP(master->slaves,cccast(cci64_t,1+caller.guid));

  cc.debug_none=d;

  ccpull_allocator();

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
  if(cc.debug_none) return sentry;

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

  return sentry->master;
}

ccfunc void
ccdebugend()
{
  if(cc.debug_none) return;

  ccassert(cc.debug_this==&cc.debug_root);
  ccdebugdump();
}
#endif