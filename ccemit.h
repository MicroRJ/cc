// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCEMIT_VALUE
#define _CCEMIT_VALUE

typedef struct ccvalue_t ccvalue_t;
typedef struct ccblock_t ccblock_t;
typedef struct ccprocd_t ccprocd_t;
typedef struct ccemit_t ccemit_t;
typedef struct cctype_t cctype_t;

typedef enum ccvalue_k
{ ccvalue_Kinvalid=0,
  ccvalue_kTARGET,
  ccvalue_kCONST,
  ccvalue_kGLOBAL,
  ccvalue_kPROCD,
  ccvalue_kEDICT,
} ccvalue_k;

typedef enum
{
  cctype_kINTEGER,
  cctype_kARRAY,
  cctype_kPROCD,
} cctype_k;

typedef struct cctype_t
{ ccstr_t     label;

	cctree_t  * tree;

  cctype_k    kind;
  cctype_t  * type;
  // Note: if it is an array ...
  ccvalue_t * length;
  // Note: if this is a procedure ..
  cctype_t  * list;
} cctype_t;

typedef struct ccvalue_t
{ ccvalue_k    kind;
  ccstr_t      label;
  ccloca_t     creator;
ccunion
{ ccedict_t * edict;
  ccprocd_t * procd;
  struct
  { cctype_t     * type;
    ccclassic_t    clsc;
  } constant;
};
} ccvalue_t;

//
typedef struct ccblock_t
{ ccstr_t      label;
  ccvalue_t * *edict;
} ccblock_t;

typedef struct ccprocd_t
{ ccstr_t     label; // Note: for debugging

  cctype_t   *type;

  // Todo: we need this because this is how we key into the local hash-table ...
  cctree_t   *tree;

  // Note: here we store all of our values, locals and parameters, this is a list of non-edicts,
  // edicts will reference values stored here ...
  ccvalue_t **local;

  ccblock_t **block;

  // Todo: here we store all of our LOCAL and PARAM edicts end a single JUMP to the enter routine of the function,
  // this is to be revised ... there's no need for the jump is there?
  ccblock_t  *decls;
  ccblock_t  *enter;
  ccblock_t  *leave;
} ccprocd_t;

typedef struct ccemit_t
{ ccvalue_t ** globals;
  ccblock_t *  current;
  ccprocd_t *  entry;
} ccemit_t;

// Todo:
ccfunc ccinle ccvalue_t *
ccvalue(ccstr_t label)
{
  ccvalue_t *t=ccmalloc_T(ccvalue_t);
  memset(t,ccnil,sizeof(*t));

  t->label=label;
  return t;
}

// Todo:
ccfunc ccinle cctype_t *
cctype(cctype_k kind, ccstr_t label)
{
  cctype_t *t=ccmalloc_T(cctype_t);
  memset(t,ccnil,sizeof(*t));
  t->kind=kind;
  t->label=label;
  return t;
}

// Todo:
ccfunc ccinle ccblock_t *
ccblock(ccstr_t label)
{
  ccblock_t *t=ccmalloc_T(ccblock_t);
  memset(t,ccnil,sizeof(*t));

  t->label=label;
  return t;
}

// Todo:
ccfunc ccinle ccprocd_t *
ccprocd(ccstr_t label)
{
  ccprocd_t *t=ccmalloc_T(ccprocd_t);
  memset(t,ccnil,sizeof(*t));

  t->label=label;
  return t;
}

// Note: some of these functions are beginning to rely a lot on actual trees, which is not a good idea,
// especially for the user level api... I don't expect to user to have to create actual trees to use this, so let's gradually
// stop using trees for hashing ...
ccfunc ccvalue_t *
ccemit_global(ccemit_t *emit, cctree_t *tree)
{
  ccvalue_t **v=cctblgetP(emit->globals,tree);
  ccassert(ccerrnon());

  ccvalue_t *value=*v;
  return value;
}

ccfunc ccvalue_t *
ccemit_include_global(ccemit_t *emit, cctree_t *tree, ccstr_t label)
{
  ccvalue_t **v=cctblputP(emit->globals,tree);
  ccassert(ccerrnon());

  ccvalue_t *value=ccvalue(label);
  *v=value;

  return value;
}

ccfunc ccprocd_t *
ccemit_global_procd(ccemit_t *emit, cctree_t *tree, ccstr_t label)
{
  ccprocd_t *p=ccprocd(label);

  ccvalue_t *v=ccemit_include_global(emit,tree,label);
  v->kind=ccvalue_kPROCD;
  v->procd=p;

  p->tree=tree;
  p->type=ccnil;
  p->block=ccnil;

  // Todo:
  *ccarradd(p->block,1)=p->decls=ccblock("$decls");
  *ccarradd(p->block,1)=p->enter=ccblock("$enter");
  *ccarradd(p->block,1)=p->leave=ccblock("$leave");

  return p;
}

ccfunc ccinle void
ccvalue_retarget(ccvalue_t *value, ccjump_point_t p)
{
  ccassert(value!=ccnil);
  ccassert(value->kind==ccvalue_kEDICT);

  ccedict_retarget(value->edict,p);
}

ccfunc ccjump_point_t
ccblock_label_ex(ccblock_t *block, int index, ccstr_t label)
{ ccjump_point_t t;
  t.label=label;
  t.block=block;
  t.index=index;
  return t;
}

ccfunc ccjump_point_t
ccblock_label(ccblock_t *block, ccstr_t label)
{ return ccblock_label_ex(block,ccarrlen(block->edict),label);
}

ccfunc ccvalue_t *
ccblock_add(ccblock_t *block)
{ ccvalue_t  *v=ccmalloc_T(ccvalue_t);
  ccvalue_t **t=ccarradd(block->edict,1);
  memset(v,ccnil,sizeof(*v));
  *t=v;
  return v;
}

ccfunc ccvalue_t *
ccblock_add_edict(ccblock_t *block, ccedict_t *edict)
{ ccvalue_t *value=ccblock_add(block);
  value->kind  = ccvalue_kEDICT;
  value->edict = edict;
  return value;
}

ccfunc ccvalue_t *
ccprocd_local(ccprocd_t *func, cctree_t *tree)
{
  // Todo: check tree ...

  ccvalue_t **v=cctblgetP(func->local,tree);
  if(ccerrnon()) return *v;
  return ccnil;
}

ccfunc ccinle ccvalue_t *
ccblock_store(ccblock_t *block, ccvalue_t *lval, ccvalue_t *rval)
{
  return ccblock_add_edict(block,ccedict_store(lval,rval));
}

ccfunc ccinle ccvalue_t *
ccblock_fetch(ccblock_t *block, ccvalue_t *lval, ccvalue_t *rval)
{
  return ccblock_add_edict(block,ccedict_fetch(lval,rval));
}

ccfunc ccinle ccvalue_t *
ccblock_address(ccblock_t *block, ccvalue_t *lval, ccvalue_t *rval)
{
  return ccblock_add_edict(block,ccedict_address(lval,rval));
}

ccfunc ccinle ccvalue_t *
ccblock_arith(ccblock_t *block, cctoken_k opr, ccvalue_t *lhs, ccvalue_t *rhs)
{
  return ccblock_add_edict(block,ccedict_arith(opr,lhs,rhs));
}

ccfunc ccinle ccvalue_t *
ccblock_return(ccblock_t *block, ccvalue_t *value)
{
  return ccblock_add_edict(block,ccedict_return(value));
}

ccfunc ccinle ccvalue_t *
ccblock_invoke(ccblock_t *block, ccprocd_t *p, ccvalue_t **i)
{
  return ccblock_add_edict(block,ccedict_call(p,i));
}

ccfunc ccinle ccvalue_t *
ccblock_jump(ccblock_t *block, ccjump_point_t point)
{
  return ccblock_add_edict(block,ccedict_jump(point));
}

ccfunc ccinle ccvalue_t *
ccblock_fjump(ccblock_t *block, ccjump_point_t point, ccvalue_t *cnd)
{
  return ccblock_add_edict(block,ccedict_fjump(point,cnd));
}

ccfunc ccinle ccvalue_t *
ccblock_tjump(ccblock_t *block, ccjump_point_t point, ccvalue_t *cnd)
{
  return ccblock_add_edict(block,ccedict_tjump(point,cnd));
}

ccfunc ccinle ccvalue_t *
ccblock_dbgbreak(ccblock_t *block)
{
  return ccblock_add_edict(block,ccedict_dbgbreak());
}

ccfunc ccinle ccvalue_t *
ccblock_dbgerror(ccblock_t *block)
{
  return ccblock_add_edict(block,ccedict_dbgerror());
}

#endif