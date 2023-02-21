// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCEDICT
#define _CCEDICT

typedef enum ccedict_k
{
  ccedict_kLOCAL = 0,
  ccedict_kPARAM,
  ccedict_kLADDR,
  ccedict_kAADDR,
  ccedict_kSTORE,
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

// Note: perhaps each edict should have a flag that indicates whether to produce a value or not.
typedef struct ccedict_t ccedict_t;
typedef struct ccedict_t
{
  ccedict_k   kind;
  ccstr_t     label; // Note: for debugging ...

ccunion
{
  // Note: Produces an addressable lvalue, if param, value must be set before invoking a function ...
  struct
  {
    cctype_t *type;
  } local,param;
  // Note: Produces an addressable lvalue at an offset specified by rval ...
  struct
  { ccvalue_t * lval;
    ccvalue_t * rval;
  } addr;
  // Note: Produces a non-addressable rvalue ...
  // Note: #lval is the lvalue to store to.
  // Note: #rval is the rvalue to store.
  struct
  { ccvalue_t *  lval;
    ccvalue_t *  rval;
  } store;
  // Note: Produces an non-addressable rvalue ...
  // Note: #lval is the lvalue to load.
  struct
  { cctype_t  *type;
    ccvalue_t *lval;
  } fetch;
  // Note: Produces a non-addressable rvalue
  struct
  { ccvalue_t  * call;
    ccvalue_t ** rval;
  } invoke;
  struct
  { ccvalue_t  * rval;
  } ret;
  struct
  { ccblock_t  * blc;
    ccu32_t      tar;
    ccvalue_t  * cnd;
  } jump;
  struct
  { ccblock_t  * blc;
  } enter;
  struct
  { ccvalue_t  * cnd;
    ccblock_t  * lhs;
    ccblock_t  * rhs;
  } ternary;
  struct
  { cctoken_k         opr;
    ccvalue_t  * lhs;
    ccvalue_t  * rhs;
  } arith;
};
} ccedict_t;

typedef struct ccjump_point_t ccjump_point_t;
typedef struct ccjump_point_t
{ ccstr_t         label; // Note: for debugging
  ccblock_t      *block;
  ccu32_t         index;
} ccjump_point_t;

// Todo:
ccfunc ccinle ccedict_t *
ccedict(ccedict_k kind, ccstr_t label)
{
  ccedict_t *e=ccmalloc_T(ccedict_t);
  memset(e,ccnil,sizeof(*e));

  e->kind=kind;
  e->label=label;
  return e;
}

// Todo:
ccfunc ccinle void
ccedict_retarget(ccedict_t *e, ccjump_point_t p)
{
  e->jump.blc=p.block;
  e->jump.tar=p.index;
}

ccfunc ccinle ccedict_t *
ccedict_local(cctype_t *type, ccstr_t label)
{
  ccassert(type!=0);

  // Todo: check the tree
  ccedict_t *e=ccedict(ccedict_kLOCAL,label);

  e->local.type=type;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_param(cctype_t *type, ccstr_t label)
{
  ccassert(type!=0);

  // Todo: check the tree
  ccedict_t *e=ccedict(ccedict_kPARAM,label);

  e->local.type=type;
  return e;
}


ccfunc ccinle ccedict_t *
ccedict_store(ccvalue_t *lval, ccvalue_t *rval)
{
  ccedict_t *e=ccedict(ccedict_kSTORE,"store");
  e->store.lval=lval;
  e->store.rval=rval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_fetch(ccvalue_t *lval, cctype_t *type)
{
  ccedict_t *e=ccedict(ccedict_kFETCH,"fetch");
  e->fetch.lval=lval;
  e->fetch.type=type;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_arith(cctoken_k opr, ccvalue_t *lhs, ccvalue_t *rhs)
{
  ccedict_t *e=ccedict(ccedict_kARITH,"arith");
  e->arith.opr=opr;
  e->arith.lhs=lhs;
  e->arith.rhs=rhs;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_jump(ccjump_point_t point)
{
  ccedict_t *e=ccedict(ccedict_kJUMP,point.label);
  e->jump.blc=point.block;
  e->jump.tar=point.index;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_tjump(ccjump_point_t point, ccvalue_t *cnd)
{
  ccedict_t *e=ccedict(ccedict_kJUMPT,point.label);
  e->jump.blc=point.block;
  e->jump.tar=point.index;
  e->jump.cnd=cnd;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_fjump(ccjump_point_t point, ccvalue_t *cnd)
{
  ccedict_t *e=ccedict(ccedict_kJUMPF,point.label);
  e->jump.blc=point.block;
  e->jump.tar=point.index;
  e->jump.cnd=cnd;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_call(ccvalue_t *lval, ccvalue_t **rval)
{
  ccassert(lval!=0);

  ccedict_t *e=ccedict(ccedict_kINVOKE,"invoke");
  e->invoke.call=lval;
  e->invoke.rval=rval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_return(ccvalue_t *rval)
{
  ccedict_t *e=ccedict(ccedict_kRETURN,"return");
  e->ret.rval=rval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_dbgbreak()
{
  ccedict_t *e=ccedict(ccedict_kDBGBREAK,"dbg_break");
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_dbgerror()
{
  ccedict_t *e=ccedict(ccedict_kDBGERROR,"dbg_error");
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_laddr(ccvalue_t *lval)
{
  ccedict_t *e=ccedict(ccedict_kLADDR,"laddr");
  e->addr.lval=lval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_aaddr(ccvalue_t *lval, ccvalue_t *rval)
{
  ccedict_t *e=ccedict(ccedict_kAADDR,"aaddr");
  e->addr.lval=lval;
  e->addr.rval=rval;
  return e;
}

#endif