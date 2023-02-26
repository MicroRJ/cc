#ifndef _CCCORE
#define _CCCORE


// Todo: this is trash!
ccglobal ccthread_local ccu32_t ccxorstate32=38747;

ccfunc ccinle ccu32_t
ccxorshift32(ccu32_t *s)
{ ccu32_t x=*s;
  x^=x<<13;
  x^=x>>17;
  x^=x<<5;
  return *s=x;
}

ccfunc ccinle ccu32_t
ccrandi(ccu32_t l)
{ return ccxorshift32(&ccxorstate32)%l;
}




// Todo: this could just be in the language...
ccfunc ccexec_value_t
ccassert__(ccexec_t *e, ccvalue_t *v, cci32_t n, ccexec_value_t *i)
{
  ccassert(n==1);

  ccassert(i->value!=0);

  ccexec_value_t r;
  r.kind=ccexec_value_kINVALID;
  r.value=0;
  return r;
}

// Todo:
ccfunc ccexec_value_t
ccfree__(ccexec_t *e, ccvalue_t *v, cci32_t n, ccexec_value_t *i)
{
  ccassert(n==1);

  ccfree((void *)i[0].value);

  ccexec_value_t r;
  r.kind=ccexec_value_kINVALID;
  r.value=0;
  return r;
}

// Todo:
ccfunc ccexec_value_t
ccmalloc__(ccexec_t *e, ccvalue_t *v, cci32_t n, ccexec_value_t *i)
{
  ccassert(n==1);

  ccexec_value_t r;
  r.kind=ccexec_value_kVALID;
  r.value=ccmalloc((size_t)i[0].value);
  return r;
}

// Todo
ccfunc ccexec_value_t
ccrealloc__(ccexec_t *e, ccvalue_t *v, cci32_t n, ccexec_value_t *i)
{
  ccassert(n==2);

  ccexec_value_t r;
  r.kind=ccexec_value_kVALID;
  r.value=ccrealloc((void *)i[0].value,(size_t)i[1].value);
  return r;
}

ccfunc ccexec_value_t
ccopenfile__(ccexec_t *e, ccvalue_t *v, cci32_t n, ccexec_value_t *i)
{
  ccassert(n==2);

  ccexec_value_t r;
  r.kind=ccexec_value_kVALID;
  r.value=(void*)ccopenfile((char *)i[0].value,(char *)i[1].value);
  return r;
}

ccfunc ccexec_value_t
ccpushfile__(ccexec_t *e, ccvalue_t *v, cci32_t n, ccexec_value_t *i)
{
  ccassert(n==4);

  ccexec_value_t r;
  r.kind=ccexec_value_kVALID;
  r.value=(void*)ccpushfile((void *)i[0].value,(ccu32_t)i[1].value,(ccu32_t)i[2].value,(void *)i[3].value);
  return r;
}


// Todo:
ccfunc ccexec_value_t
ccprintf__(ccexec_t *e, ccvalue_t *v, cci32_t n, ccexec_value_t *i)
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

#endif