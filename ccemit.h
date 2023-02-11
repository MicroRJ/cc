#ifndef _CCEMIT_VALUE
#define _CCEMIT_VALUE

typedef struct ccemit_value_t ccemit_value_t;
typedef struct ccemit_block_t ccemit_block_t;
typedef struct ccemit_procd_t ccemit_procd_t;
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
} cctype_k;

typedef struct cctype_t
{ ccstr_t   label;

  cctype_k  kind;
  cctype_t *type;

  // Note: if it is an array ...
  ccemit_value_t *length;
} cctype_t;

typedef struct ccemit_value_t
{ ccvalue_k        kind;

  ccstr_t label;
  ccloc_t creator;

ccunion
{ ccedict_t      * edict;
  ccemit_procd_t * procd;
  struct
  { cctype_t     * type;
    ccclassic_t    clsc;
  } constant;
};
} ccemit_value_t;

typedef struct ccemit_block_t
{ ccstr_t          label; // Note: for debugging
  ccemit_value_t **edict;
} ccemit_block_t;

typedef struct ccemit_procd_t
{ ccstr_t          label; // Note: for debugging

  cctree_t        *tree;
  ccemit_value_t **local;
  ccemit_block_t **block;

  ccemit_block_t  *decls;
  ccemit_block_t  *enter;
  ccemit_block_t  *leave;
} ccemit_procd_t;

typedef struct ccemit_t
{ ccemit_value_t ** globals;
  ccemit_block_t *  current;
  ccemit_procd_t *  entry;
} ccemit_t;

// Todo:
ccfunc ccinle ccemit_value_t *
ccvalue(ccstr_t label)
{
  ccemit_value_t *t=ccmalloc_T(ccemit_value_t);
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
ccfunc ccinle ccemit_block_t *
ccblock(ccstr_t label)
{
  ccemit_block_t *t=ccmalloc_T(ccemit_block_t);
  memset(t,ccnil,sizeof(*t));

  t->label=label;
  return t;
}

// Todo:
ccfunc ccinle ccemit_procd_t *
ccprocd(ccstr_t label)
{
  ccemit_procd_t *t=ccmalloc_T(ccemit_procd_t);
  memset(t,ccnil,sizeof(*t));

  t->label=label;
  return t;
}

// Note: some of these functions are beginning to rely a lot on actual trees, which is not a good idea,
// especially for the user level api... I don't expect to user to have to create actual trees to use this, so let's gradually
// stop using trees for hashing ...
ccfunc ccemit_value_t *
ccemit_global(ccemit_t *emit, cctree_t *tree)
{
  ccemit_value_t **v=cctblgetP(emit->globals,tree);
  ccassert(ccerrnon());

  ccemit_value_t *value=*v;
  return value;
}

ccfunc ccemit_value_t *
ccemit_include_global(ccemit_t *emit, cctree_t *tree, ccstr_t label)
{
  ccemit_value_t **v=cctblputP(emit->globals,tree);
  ccassert(ccerrnon());

  ccemit_value_t *value=ccvalue(label);
  *v=value;

  return value;
}

ccfunc ccemit_procd_t *
ccemit_global_procd(ccemit_t *emit, cctree_t *tree, ccstr_t label)
{
  ccemit_value_t *v=ccemit_include_global(emit,tree,label);
  v->kind=ccvalue_kPROCD;

  ccemit_procd_t *p=ccprocd(label);

  p->tree=tree;
  p->block=ccnil;

  // Todo:
  *ccarradd(p->block,1)=p->decls=ccblock("$decls");
  *ccarradd(p->block,1)=p->enter=ccblock("$enter");
  *ccarradd(p->block,1)=p->leave=ccblock("$leave");

  v->procd=p;

  return p;
}

ccfunc ccinle void
ccvalue_retarget(ccemit_value_t *value, ccjump_point_t p)
{
	ccassert(value!=ccnil);
	ccassert(value->kind==ccvalue_kEDICT);

  ccedict_retarget(value->edict,p);
}

ccfunc ccjump_point_t
ccblock_label(ccemit_block_t *block, ccstr_t label)
{ ccjump_point_t t;
  t.label=label;
  t.block=block;
  t.index=ccarrlen(block->edict);
  return t;
}

ccfunc ccemit_value_t *
ccblock_add(ccemit_block_t *block)
{ ccemit_value_t  *v=ccmalloc_T(ccemit_value_t);
  ccemit_value_t **t=ccarradd(block->edict,1);
  memset(v,ccnil,sizeof(*v));
  *t=v;
  return v;
}

ccfunc ccemit_value_t *
ccblock_add_edict(ccemit_block_t *block, ccedict_t *edict)
{ ccemit_value_t *value=ccblock_add(block);
  value->kind  = ccvalue_kEDICT;
  value->edict = edict;
  return value;
}

ccfunc ccemit_value_t *
ccprocd_local(ccemit_procd_t *func, cctree_t *tree)
{
  // Todo: check tree ...

  ccemit_value_t **v=cctblgetP(func->local,tree);
  if(ccerrnon()) return *v;
  return ccnil;
}

ccfunc ccinle ccemit_value_t *
ccblock_store(ccemit_block_t *block, ccemit_value_t *lval, ccemit_value_t *rval)
{
  return ccblock_add_edict(block,ccedict_store(lval,rval));
}

ccfunc ccinle ccemit_value_t *
ccblock_fetch(ccemit_block_t *block, ccemit_value_t *lval, ccemit_value_t *rval)
{
  return ccblock_add_edict(block,ccedict_fetch(lval,rval));
}

ccfunc ccinle ccemit_value_t *
ccblock_address(ccemit_block_t *block, ccemit_value_t *lval, ccemit_value_t *rval)
{
  return ccblock_add_edict(block,ccedict_address(lval,rval));
}

ccfunc ccinle ccemit_value_t *
ccblock_arith(ccemit_block_t *block, cctoken_k opr, ccemit_value_t *lhs, ccemit_value_t *rhs)
{
  return ccblock_add_edict(block,ccedict_arith(opr,lhs,rhs));
}

ccfunc ccinle ccemit_value_t *
ccblock_enter(ccemit_block_t *block, ccemit_block_t *blc)
{
  return ccblock_add_edict(block,ccedict_enter(blc));
}

ccfunc ccinle ccemit_value_t *
ccblock_return(ccemit_block_t *block, ccemit_value_t *value)
{
  return ccblock_add_edict(block,ccedict_return(value));
}

ccfunc ccinle ccemit_value_t *
ccblock_ternary(ccemit_block_t *block, ccemit_value_t *init, ccemit_block_t *lval, ccemit_block_t *rval)
{
  return ccblock_add_edict(block,ccedict_ternary(init,lval,rval));
}

ccfunc ccinle ccemit_value_t *
ccblock_invoke(ccemit_block_t *block, ccemit_procd_t *p, ccemit_value_t **i)
{
  return ccblock_add_edict(block,ccedict_call(p,i));
}

ccfunc ccinle ccemit_value_t *
ccblock_jump(ccemit_block_t *block, ccjump_point_t point)
{
  return ccblock_add_edict(block,ccedict_jump(point));
}

ccfunc ccinle ccemit_value_t *
ccblock_cjump(ccemit_block_t *block, ccjump_point_t point, ccemit_value_t *cnd)
{
  return ccblock_add_edict(block,ccedict_cjump(point,cnd));
}

ccfunc ccinle ccemit_value_t *
ccblock_fjump(ccemit_block_t *block, ccjump_point_t point, ccemit_value_t *cnd)
{
  return ccblock_add_edict(block,ccedict_fjump(point,cnd));
}

ccfunc ccinle ccemit_value_t *
ccblock_tjump(ccemit_block_t *block, ccjump_point_t point, ccemit_value_t *cnd)
{
  return ccblock_add_edict(block,ccedict_tjump(point,cnd));
}

#endif