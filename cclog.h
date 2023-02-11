/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved. **/
/*****************************************************************/
#ifndef _CCLOG
#define _CCLOG

// Note: ensure caller guid is never 0 ...
typedef struct cccaller_t cccaller_t;
typedef struct cccaller_t
{ int           guid;
  const char   *file;
  int           line;
  const char   *func;
} cccaller_t;

typedef struct cctimed_t cctimed_t;
typedef struct cctimed_t
{
  cccaller_t    caller;

  const char   *label;
  cctimed_t    *super;
  cctimed_t    *child;
  cci32_t       level;


  cci32_t       event;

  ccclocktick_t clock;
  ccclocktick_t total;

  cci32_t allocations;
  cci32_t deallocations;
  cci32_t reallocations;
  cci32_t collisions;

  ccu64_t memory;

} cctimed_t;


ccfunc ccinle cccaller_t cccaller(int guid, const char *file, int line, const char *func);
ccfunc ccinle cccaller_t ccmycaller();

ccfunc void ccpushcaller_(cccaller_t);
ccfunc void ccpullcaller_(cccaller_t);

#ifndef ccpushcaller
# define ccpushcaller() ccpushcaller_(cccaller(1+__COUNTER__,ccfilename(_CCFILE),_CCLINE,_CCFUNC))
#endif

#ifndef ccpullcaller
# define ccpullcaller() ccpullcaller_(cccaller(1+__COUNTER__,ccfilename(_CCFILE),_CCLINE,_CCFUNC))
#endif

#ifndef cccall
# define cccall(expression) (ccpushcaller(),expression,ccpullcaller())
#endif

#ifndef cctimedhead
# define cctimedhead(label) cccall(cctimedhead_(label))
#endif

#ifndef cctimedtail
# define cctimedtail(label) (ccpushcaller(),cctimedtail_(label))
#endif

ccfunc void cctimedhead_(const char *label);
ccfunc void cctimedtail_(const char *label);

ccfunc ccinle cctimed_t *cctimed();


#endif

#if defined(_CCLOG) && defined(_CCLOG_IMPL)

// Note: getting a little too comfortable with globals
ccglobal ccthread_local cccaller_t  cccallstack[0x04];
ccglobal ccthread_local cci32_t     cccallindex;


ccglobal ccthread_local cctimed_t   cctimedroot;
ccglobal ccthread_local cctimed_t  *cctimedthis;
ccglobal ccthread_local cci32_t     cctimed_enabled=cctrue;


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

ccfunc ccinle cccaller_t
ccmycaller()
{
  ccassert(cccallindex>=1);

  return cccallstack[cccallindex-1];
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

ccfunc void
ccpullcaller_(cccaller_t caller)
{
  ccassert(cccallindex>=1);

  cccallindex --;
}

ccfunc ccinle cctimed_t *
cctimed()
{
  ccglobal cctimed_t dummy;
  return cctimed_enabled && cctimedthis? cctimedthis :&dummy;
}

ccfunc void
cctimeddump(cctimed_t *root)
{
  for(int i=0;i<root->level*2;++i)
    printf(" ");

  printf("#%i %s[%i] %s(): %i '%s' %f(s) %i-%i,%i = %llu !%i\n",
    root->caller.guid,root->caller.file,root->caller.line,root->caller.func,
      root->event,root->label,ccclocksecs(root->total),
        root->allocations,root->deallocations,root->reallocations,root->memory,root->collisions);

  cctimed_t *child;
  ccarrfor(root->child,child)
    cctimeddump(child);
}

ccfunc void
cctimedhead_(const char *label)
{
  if(!cctimed_enabled)
    return;

  cctimed_t *super=cctimedthis;

  cccaller_t caller=ccmycaller();

  if(!cctimedthis)
  {
    cctimedthis=&cctimedroot;
  } else
  {
    cctimed_enabled=ccfalse;
    cctimedthis=cctblsetP(cctimedthis->child,cccast(cci64_t,caller.guid));
    cctimed_enabled=cctrue;
  }


  if(cctimedthis->event==0)
  { cctimedthis->caller=caller;
    cctimedthis->super=super;
    cctimedthis->label=label;
    cctimedthis->level=super?super->level+1:0;
  }
  cctimedthis->event++;

  cctimedthis->clock=ccclocktick();
}

ccfunc void
cctimedtail_(const char *label)
{
  if(!cctimed_enabled)
    return;

  ccassert(cctimedthis!=0);

  cccaller_t caller=ccmycaller();

  ccassert(strcmp(cctimedthis->caller.file,caller.file)==0);
  ccassert(strcmp(cctimedthis->caller.func,caller.func)==0);

  ccclocktick_t tick=ccclocktick();
  ccclocktick_t span=tick-cctimedthis->clock;
  cctimedthis->total+=span;

  cctimedthis=cctimedthis->super;

  if(!cctimedthis)
  {
    printf("timed:\n");
    cctimeddump(&cctimedroot);
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