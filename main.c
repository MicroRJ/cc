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

ccfunc ccexec_value_t
ccassert__(ccexec_t *exec, ccvalue_t *value, cci32_t argc, ccexec_value_t *args)
{
  ccexec_value_t r;
  r.asi32=1;

  ccassert(args->asi32);

  return r;
}

ccfunc ccexec_value_t buildrunfile(const char *filename)
{
ccenter("build-run-file");

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

  ccvalue_t *v=ccemit_include_global(&emit,"ccassert");
  v->kind=ccvalue_kPROCU;
  ccprocu_t *p=ccprocu("ccassert");
  v->procu=p;
  p->proc=ccassert__;

  // ccemit_include_procu(ccnil,"ccassert",ccassert);

ccenter("read");
  ccread_include(&read,filename);
  tree=ccread_translation_unit(&read);
ccleave("read");

ccenter("seer");
  ccseer_translation_unit(&seer,tree);
ccleave("seer");

ccenter("emit");
  ccemit_translation_unit(&emit,&seer,tree);
ccleave("emit");

ccenter("exec");
  result=ccexec_translation_unit(&exec,&emit);
ccleave("exec");

  ccread_uninit(&read);
  ccexec_uninit(&exec);
  ccseer_uninit(&seer);

ccleave("build-run-file");
  return result;
}

int main(int argc, char **argv)
{
// Todo:
ccdebugnone=cctrue;
ccini();

ccenter("main");
  ++ argv;
  -- argc;

  const char *f[]=
  {
    "code\\builtin.cc",
    "code\\decl.cc",
    "code\\retr.cc",
    "code\\lval.cc",
    "code\\fib.cc",
  };

  enum { l=sizeof(f)/sizeof(f[0]) };

  ccexec_value_t e[l];

  for(int i=0; i<l; ++i)
  {
    e[i]=buildrunfile(f[i]);
  }

  for(int i=0; i<l; ++i)
  {
    ccprintf("<!6'%s'!>: <!3%lli!>\n",f[i],e[i].asi64);
  }

ccleave("main");

ccdebugend();
}