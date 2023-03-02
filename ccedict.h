
// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCEDICT
#define _CCEDICT

// Todo: document this properly ...
typedef enum ccedict_k
{
  ccedict_kINVALID=0,
  ccedict_kLOCAL,
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

typedef struct ccleap_t ccleap_t;
typedef struct ccleap_t
{ const char * label;
  ccblock_t  * block;
  cci32_t      index;
} ccleap_t;

// Todo: this has to be smaller...
typedef struct ccedict_t ccedict_t;
typedef struct ccedict_t
{ ccedict_k    kind;

  // Todo: switch this off in debug mode...
  cctree_t  *  tree;

  cctype_t  *  type;
  cctoken_k    sort;
  ccvalue_t *  lval;
  ccvalue_t *  rval;
  ccvalue_t ** blob;
  ccleap_t     leap;

  // Todo:
  int is_zero_init;

  // Todo: each edict should have a flag that indicates whether to produce a value or not. Implement!
  int yield;
} ccedict_t;


// Todo: proper allocator ...
ccfunc ccinle ccedict_t *
ccedict(cctree_t *tree, ccedict_k kind)
{
  ccedict_t *e=ccmalloc_T(ccedict_t);
  memset(e,ccnull,sizeof(*e));

  e->kind=kind;
  e->tree=tree;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_local(cctree_t *tree, cctype_t *type)
{
  ccassert(type!=0);

  ccedict_t *e=ccedict(tree,ccedict_kLOCAL);
  e->type=type;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_param(cctree_t *tree, cctype_t *type)
{
  ccassert(type!=0);

  ccedict_t *e=ccedict(tree,ccedict_kPARAM);
  e->type=type;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_store(cctree_t *tree, cctype_t *type, ccvalue_t *lval, ccvalue_t *rval)
{
  ccassert(type!=0);
  ccassert(lval!=0);
  ccassert(rval!=0);

  ccedict_t *e=ccedict(tree,ccedict_kSTORE);
  e->type=type;
  e->lval=lval;
  e->rval=rval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_fetch(cctree_t *tree, cctype_t *type, ccvalue_t *lval)
{
  ccassert(type!=0);
  ccassert(lval!=0);

  ccedict_t *e=ccedict(tree,ccedict_kFETCH);
  e->type=type;
  e->lval=lval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_laddr(cctree_t *tree, cctype_t *type, ccvalue_t *lval)
{
  ccassert(type!=0);
  ccassert(lval!=0);

  ccedict_t *e=ccedict(tree,ccedict_kLADDR);
  e->type=type;
  e->lval=lval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_aaddr(cctree_t *tree, cctype_t *type, ccvalue_t *lval, ccvalue_t *rval)
{
  ccassert(type!=0);
  ccassert(lval!=0);
  ccassert(rval!=0);

  ccedict_t *e=ccedict(tree,ccedict_kAADDR);
  e->type=type;
  e->lval=lval;
  e->rval=rval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_arith(cctree_t *tree, cctoken_k sort, ccvalue_t *lval, ccvalue_t *rval)
{
  ccassert(sort!=cctoken_kINVALID);
  ccassert(lval!=0);
  ccassert(rval!=0);

  ccedict_t *e=ccedict(tree,ccedict_kARITH);
  e->sort=sort;
  e->lval=lval;
  e->rval=rval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_jump(cctree_t *tree, ccleap_t leap)
{
  ccassert(leap.block!= 0);
  ccassert(leap.index!=-1);

  ccedict_t *e=ccedict(tree,ccedict_kJUMP);
  e->leap=leap;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_tjump(cctree_t *tree, ccleap_t leap, ccvalue_t *rval)
{
  ccassert(rval!=0);
  ccassert(leap.block!= 0);
  ccassert(leap.index!=-1);

  ccedict_t *e=ccedict(tree,ccedict_kJUMPT);
  e->rval=rval;
  e->leap=leap;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_fjump(cctree_t *tree, ccleap_t leap, ccvalue_t *rval)
{
  ccassert(rval!=0);
  ccassert(leap.block!= 0);
  ccassert(leap.index!=-1);

  ccedict_t *e=ccedict(tree,ccedict_kJUMPF);
  e->rval=rval;
  e->leap=leap;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_call(cctree_t *tree, ccvalue_t *lval, ccvalue_t **blob)
{
  ccassert(lval!=0);

  ccedict_t *e=ccedict(tree,ccedict_kINVOKE);
  e->lval=lval;
  e->blob=blob;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_return(cctree_t *tree, ccvalue_t *rval)
{
  ccedict_t *e=ccedict(tree,ccedict_kRETURN);
  e->rval=rval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_dbgbreak(cctree_t *tree)
{
  return ccedict(tree,ccedict_kDBGBREAK);
}

ccfunc ccinle ccedict_t *
ccedict_dbgerror(cctree_t *tree)
{
  return ccedict(tree,ccedict_kDBGERROR);
}

#endif