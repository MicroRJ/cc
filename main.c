// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.

// Todo: why are you like this? remove this!
#define ARG 22

#include "cc.c"

int fib(int x)
{ if(x>=2)
  { int l=fib(x-2);
    int r=fib(x-1);
    return l+r;
  }
  return x;
}

// Todo: temporary!
ccfunc void
ccregister(ccseer_t *seer, ccemit_t *emit, const char *name, cctype_t *retr, ccbinding_t *binding)
{
  ccesse_t *e=ccmalloc_T(ccesse_t);
  e->kind=ccesse_kCBINDING;
  e->sort=ccbuiltin_kINVALID;
  // Todo: proper modifier
  e->type=cctype_function_modifier(retr,ccnull,ccfalse);

  if(ccseer_include_entity(seer,e,name))
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

  ccregister(&seer,&emit,"ccassert", cctype_void, ccassert__);
  ccregister(&seer,&emit,"ccprintf", cctype_void, ccprintf__);
  ccregister(&seer,&emit,"ccmalloc", cctype_void_ptr, ccmalloc__);
  ccregister(&seer,&emit,"ccrealloc", cctype_void_ptr, ccrealloc__);
  ccregister(&seer,&emit,"ccfree", cctype_void, ccfree__);

  ccregister(&seer,&emit,"ccopenfile", cctype_void_ptr, ccopenfile__);
  ccregister(&seer,&emit,"ccpushfile", cctype_stdc_int, ccpushfile__);

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

  ccread_uninit(&read);
  ccexec_uninit(&exec);
  ccseer_uninit(&seer);

ccdbleave("build-run-file");
  return result;
}

int main(int argc, char **argv)
{
// Todo:
ccdebugnone=cctrue;
ccini();
ccdbenter("main");

  ccdlb_test();

  ++ argv;
  -- argc;

  const char *f[]=
  {
    "code\\lval.cc",
  };

  enum { l=sizeof(f)/sizeof(f[0]) };

  ccexec_value_t e[l];

  for(int i=0; i<l; ++i)
  {
    e[i]=buildrunfile(f[i]);
  }

  for(int i=0; i<l; ++i)
  {
    ccprintf("<!6'%s'!>: <!3%lli!>\n",f[i],e[i].constI);
  }

ccdbleave("main");

ccdebugend();
}