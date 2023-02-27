// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#include "cc.c"

// Todo: temporary!
ccfunc void
ccregister(ccseer_t *seer, ccemit_t *emit, const char *name, cctype_t *retr, ccbinding_t *binding)
{
  // Todo:
  ccesse_t *e=ccmalloc_T(ccesse_t);
  memset(e,ccnull,sizeof(*e));

  e->kind=ccesse_kCBINDING;
  e->sort=ccbuiltin_kINVALID;
  e->name=name;

  // Todo: proper modifier
  e->type=cctype_function_modifier(retr,ccnull,ccfalse);

  if(ccseer_register(seer,e,name))
  {
    ccvalue_t *v=ccemit_include_global(emit,name);
    v->kind=ccvalue_kPROCU;

    ccprocu_t *p=ccprocu();
    v->procu=p;
    p->proc=binding;
  }
}


ccfunc ccexec_value_t
buildrunfile(const char *filename)
{
ccdbenter("build-run-file");

  ccexec_value_t result;

  ccread_t read;
  ccseer_t seer;
  ccemit_t emit;
  ccexec_t exec;

  cctree_t *tree;

  ccread_init(&read);
  ccseer_init(&seer);
  ccemit_init(&emit);
  ccexec_init(&exec);

  // Todo:
  ccregister(&seer,&emit,"ccassert",  seer.type_void,      ccassert__);
  ccregister(&seer,&emit,"ccprintf",  seer.type_void,      ccprintf__);
  ccregister(&seer,&emit,"ccmalloc",  seer.type_void_ptr,  ccmalloc__);
  ccregister(&seer,&emit,"ccrealloc", seer.type_void_ptr,  ccrealloc__);
  ccregister(&seer,&emit,"ccfree",    seer.type_void,      ccfree__);

  ccregister(&seer,&emit,"ccopenfile", seer.type_void_ptr, ccopenfile__);
  ccregister(&seer,&emit,"ccpushfile", seer.type_stdc_int, ccpushfile__);

ccdbenter("read");
  ccread_include(&read,filename);
  tree=ccread_translation_unit(&read);
ccdbleave("read");

ccdbenter("seer");
  ccseer_translation_unit(&seer,tree);
ccdbleave("seer");

ccdbenter("emit");
  ccemit_translation_unit(&emit,&seer);
ccdbleave("emit");

ccdbenter("exec");
  result=ccexec_translation_unit(&exec,&emit);
ccdbleave("exec");

  ccexec_uninit(&exec);
  ccemit_uninit(&emit);
  ccseer_uninit(&seer);
  ccread_uninit(&read);

ccdbleave("build-run-file");
  return result;
}

void cctest();

int main(int argc, char **argv)
{
// Todo:
ccdebugnone=ccfalse;
ccini();
ccdbenter("main");

  cctest();

  // sizeof(ccdebugthis);

#if 1
  const char *n="code\\lval.cc";
  ccexec_value_t e=buildrunfile(n);
  ccprintf("<!6'%s'!>: <!3%lli!>\n",n,e.constI);
#endif

ccdbleave("main");

ccdebugend();
}

void cctest()
{
  char h[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  char r[1024][128];
  typedef struct _t { char *s; } _t;

  _t *v=ccnull;
  for(int i=0; i<256; ++i)
  { ccarradd(v,1)->s=h;
  }

ccdbenter("test");

  int i,j;
  for(i=0; i<ccCarrlenL(r); r[i++][j]=0)
    for(j=0; j<ccCarrlenL(r[0])-1; r[i][j++]=h[ccrandi(ccCstrlenL(h))]);

ccdbenter("test-ccdlbputS");
  _t *a=ccnull;
  for(i=0; i<ccCarrlenL(r); ++i)
  { _t *t=cctblputS(a,r[i]);
    ccassert(ccerrnon());
    t->s=cckeyget();
    ccassert(!strcmp(t->s,r[i]));
  }
ccdbleave("test-ccdlbputS");

ccdbenter("test-ccdlbgetS");
  for(i=0; i<ccCarrlenL(r); ++i)
  { _t *t=cctblgetS(a,r[i]);
    ccassert(ccerrnon());
    ccassert(t->s!=0);
    ccassert(!strcmp(t->s,r[i]));
    ccassert(!strcmp(t->s,cckeyget()));
  }
ccdbleave("test-ccdlbgetS");

ccdbenter("test-ccdlbdel");
  ccdlbdel(a);
ccdbleave("test-ccdlbdel");

ccdbleave("test");
}
