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

// Todo:
ccfunc ccexec_value_t
ccassert__(ccexec_t *exec, ccvalue_t *value, cci32_t argc, ccexec_value_t *args)
{
  ccexec_value_t r;
  r.constI=1;

  ccassert(args->constI);

  return r;
}

// Todo:
ccfunc ccexec_value_t
ccprintf__(ccexec_t *exec, ccvalue_t *value, cci32_t n, ccexec_value_t *i)
{
  ccassert(n>=1);

  const char *r=(char*)i->value;

  char c;
  for(;c=*r;c)
  {
    for(;(c!='\0')&&(c!='%'); r+=1,c=*r) printf("%c",c);

    if((!n)||(c=='\0')) break;

    n--;
    i++;

    r+=1,c=*r;
    if(c=='%')
        printf("%%"),r+=1,c=*r;
    else
    if(c=='i')
      printf("%i",(cci32_t)i->constI),r+=1,c=*r;
    else
    if(c=='c')
      printf("%c",(char)i->constI),r+=1,c=*r;
    else
    if(c=='s')
      printf("%s",(char *)i->value),r+=1,c=*r;
    else
    if(c=='p')
      printf("%p",(void *)i->constI),r+=1,c=*r;
    else
    if(c=='f')
      printf("%f",i->constR),r+=1,c=*r;
    else
    { if(c=='l'&&r[1]=='l'&&r[2]=='i')
        printf("%lli",i->constI),r+=3,c=*r;
      else
      if(c=='l'&&r[1]=='l'&&r[2]=='u')
        printf("%llu",i->constU),r+=3,c=*r;
      else
        ccassert(!"error");
    }
  }

  ccexec_value_t z;
  z.constI=1;
  return z;
}

ccfunc ccexec_value_t buildrunfile(const char *filename)
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
  {
    ccvalue_t *v=ccemit_include_global(&emit,"ccassert");
    v->kind=ccvalue_kPROCU;
    ccprocu_t *p=ccprocu("ccassert");
    v->procu=p;
    p->proc=ccassert__;
  }

  {
    ccvalue_t *v=ccemit_include_global(&emit,"ccprintf");
    v->kind=ccvalue_kPROCU;
    ccprocu_t *p=ccprocu("ccprintf");
    v->procu=p;
    p->proc=ccprintf__;
  }

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