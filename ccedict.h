// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCEDICT
#define _CCEDICT

// Todo: document this properly ...
typedef enum ccedict_k
{
  ccedict_kLOCAL = 0,
  ccedict_kPARAM,

  // Note: expects an addressable l-value, a variable ...
  ccedict_kLADDR,
  // Note: expects any sort of addressable value, such as another address, a local or parameter and produces an address value...
  ccedict_kAADDR,

  ccedict_kSTORE,

  // Note: expects any sort of addressable value, loads its contents based on the type specified and produces a non-addressable r-value...
  ccedict_kFETCH,

  ccedict_kARITH,
  ccedict_kJUMP,
  ccedict_kJUMPT,
  ccedict_kJUMPF,
  ccedict_kINVOKE,
  ccedict_kRETURN,
  ccedict_kDBGBREAK,
  ccedict_kDBGERROR,
} ccedict_k;

ccglobal const char *ccedict_s[]=
{ "LOCAL   ",
  "PARAM   ",
  "LADDR   ",
  "AADDR   ",
  "STORE   ",
  "FETCH   ",
  "ARITH   ",
  "JUMP    ",
  "JUMPT   ",
  "JUMPF   ",
  "INVOKE  ",
  "RETURN  ",
  "DBGBREAK",
  "DBGERROR",
};

// Note: Produces an addressable lvalue, if param, value must be set before invoking a function ...

typedef struct ccleap_t ccleap_t;
typedef struct ccleap_t
{ const char * label;
  ccblock_t  * block;
  cci32_t      index;
} ccleap_t;

// Note: perhaps each edict should have a flag that indicates whether to produce a value or not.
typedef struct ccedict_t ccedict_t;
typedef struct ccedict_t
{ ccedict_k    kind;
  cctype_t  *  type;
  cctoken_k    sort;
  ccvalue_t *  lval;
  ccvalue_t *  rval;
  ccvalue_t ** blob;
  ccleap_t     leap;

  // Todo: implement!
  int yield;
  int is_zero_init;
} ccedict_t;


// Todo:
ccfunc ccinle ccedict_t *
ccedict(ccedict_k kind)
{
  ccedict_t *e=ccmalloc_T(ccedict_t);
  memset(e,ccnull,sizeof(*e));

  e->kind=kind;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_local(cctype_t *type)
{
  ccassert(type!=0);

  ccedict_t *e=ccedict(ccedict_kLOCAL);
  e->type=type;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_param(cctype_t *type)
{
  ccassert(type!=0);

  ccedict_t *e=ccedict(ccedict_kPARAM);
  e->type=type;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_store(cctype_t *type, ccvalue_t *lval, ccvalue_t *rval)
{
  ccassert(type!=0);
  ccassert(lval!=0);
  ccassert(rval!=0);

  ccedict_t *e=ccedict(ccedict_kSTORE);
  e->type=type;
  e->lval=lval;
  e->rval=rval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_fetch(cctype_t *type, ccvalue_t *lval)
{
  ccassert(type!=0);
  ccassert(lval!=0);

  ccedict_t *e=ccedict(ccedict_kFETCH);
  e->type=type;
  e->lval=lval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_laddr(cctype_t *type, ccvalue_t *lval)
{
  ccassert(type!=0);
  ccassert(lval!=0);

  ccedict_t *e=ccedict(ccedict_kLADDR);
  e->type=type;
  e->lval=lval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_aaddr(cctype_t *type, ccvalue_t *lval, ccvalue_t *rval)
{
  ccassert(type!=0);
  ccassert(lval!=0);
  ccassert(rval!=0);

  ccedict_t *e=ccedict(ccedict_kAADDR);
  e->type=type;
  e->lval=lval;
  e->rval=rval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_arith(cctoken_k sort, ccvalue_t *lval, ccvalue_t *rval)
{
  ccedict_t *e=ccedict(ccedict_kARITH);
  e->sort=sort;
  e->lval=lval;
  e->rval=rval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_jump(ccleap_t leap)
{
  ccedict_t *e=ccedict(ccedict_kJUMP);
  e->leap=leap;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_tjump(ccleap_t leap, ccvalue_t *rval)
{
  ccassert(rval!=0);

  ccedict_t *e=ccedict(ccedict_kJUMPT);
  e->rval=rval;
  e->leap=leap;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_fjump(ccleap_t leap, ccvalue_t *rval)
{
  ccassert(rval!=0);

  ccedict_t *e=ccedict(ccedict_kJUMPF);
  e->rval=rval;
  e->leap=leap;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_call(ccvalue_t *lval, ccvalue_t **blob)
{
  ccassert(lval!=0);

  ccedict_t *e=ccedict(ccedict_kINVOKE);
  e->lval=lval;
  e->blob=blob;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_return(ccvalue_t *rval)
{
  ccedict_t *e=ccedict(ccedict_kRETURN);
  e->rval=rval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_dbgbreak()
{
  return ccedict(ccedict_kDBGBREAK);
}

ccfunc ccinle ccedict_t *
ccedict_dbgerror()
{
  return ccedict(ccedict_kDBGERROR);
}

#endif