#ifndef _CCEDICT
#define _CCEDICT

typedef enum ccedict_k
{
	ccedict_kLOCAL = 0,
	ccedict_kPARAM,
	ccedict_kSTORE,
	ccedict_kFETCH,
  ccedict_kARITH,
  ccedict_kBLOCK,
  ccedict_kTERNARY,
  ccedict_kENTER,
  ccedict_kINVOKE,
  ccedict_kRETURN,
} ccedict_k;

ccglobal const char *ccedict_s[]=
{ "LOCAL","PARAM","STORE","FETCH","ARITH","BLOCK","TERNARY","ENTER","INVOKE","RETURN",
};


// Note: perhas each edict should have a flag that indicates whether to produce a value or not.
typedef struct ccedict_t
{
	ccedict_k   kind;
	ccstr_t     label; // Note: for debugging ...

#if defined(_DEBUG)
struct
#else
union
#endif
{
	// Note: Produces an addressable lvalue, if param, value must be set before invoking a function ...
	struct
	{
		cctree_t   *type;
	} local,param;
	// Note: Produces a non-addressable rvalue ...
	// Note: #lval is the lvalue to store to.
	// Note: #rval is the rvalue to store.
	struct
	{ ccemit_value_t *  lval;
		ccemit_value_t *  rval;
	} store;
	// Note: Produces an non-addressable rvalue ...
	// Note: #lval is the lvalue to load.
	struct
	{ ccemit_value_t  * lval;
	} fetch;
	// Note: Produces a non-addressable rvalue
	struct
	{ ccemit_value_t  * call;
		ccemit_value_t ** rval;
	} invoke;
	struct
	{ ccemit_value_t  * rval;
	} ret;
	struct
	{ ccemit_block_t  * blc;
	} enter;
	struct
	{ ccemit_value_t  * cnd;
  	ccemit_block_t  * lhs;
  	ccemit_block_t  * rhs;
	} ternary;
	struct
	{ cctoken_k         opr;
	  ccemit_value_t  * lhs;
	  ccemit_value_t  * rhs;
	} arith;
};
} ccedict_t;

ccfunc ccinle ccedict_t *
ccedict_local(cctree_t *tree)
{
	// Todo: check the tree

	ccedict_t *e=ccmalloc_T(ccedict_t);
	e->kind=ccedict_kLOCAL;
	e->label=tree->name;

	e->local.type=tree->type;
	return e;
}

ccfunc ccinle ccedict_t *
ccedict_param(cctree_t *tree)
{
	// Todo: check the tree

	ccedict_t *e=ccmalloc_T(ccedict_t);
	e->kind=ccedict_kPARAM;
	e->label=tree->name;

	e->local.type=tree->type;
	return e;
}

ccfunc ccinle ccedict_t *
ccedict_store(ccemit_value_t *lval, ccemit_value_t *rval)
{
	ccedict_t *e=ccmalloc_T(ccedict_t);
	e->kind=ccedict_kSTORE;
	e->store.lval=lval;
	e->store.rval=rval;
	return e;
}

ccfunc ccinle ccedict_t *
ccedict_fetch(ccemit_value_t *lval)
{
	ccedict_t *e=ccmalloc_T(ccedict_t);
	e->kind=ccedict_kFETCH;
	e->fetch.lval=lval;
	return e;
}

ccfunc ccinle ccedict_t *
ccedict_arith(cctoken_k opr, ccemit_value_t *lhs, ccemit_value_t *rhs)
{
	ccedict_t *e=ccmalloc_T(ccedict_t);
	e->kind=ccedict_kARITH;
	e->arith.opr=opr;
	e->arith.lhs=lhs;
	e->arith.rhs=rhs;
	return e;
}

ccfunc ccinle ccedict_t *
ccedict_enter(ccemit_block_t *blc)
{
	ccedict_t *e=ccmalloc_T(ccedict_t);
	e->kind=ccedict_kENTER;
	e->enter.blc=blc;

	return e;
}

ccfunc ccinle ccedict_t *
ccedict_call(ccemit_value_t *lval, ccemit_value_t **rval)
{
	ccassert(lval!=0);

	ccedict_t *e=ccmalloc_T(ccedict_t);
	e->kind=ccedict_kINVOKE;
	e->invoke.call=lval;
	e->invoke.rval=rval;

	return e;
}

ccfunc ccinle ccedict_t *
ccedict_return(ccemit_value_t *rval)
{
	ccedict_t *e=ccmalloc_T(ccedict_t);
  e->kind=ccedict_kRETURN;
  e->ret.rval=rval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_ternary(ccemit_value_t *cnd, ccemit_block_t *lhs, ccemit_block_t *rhs)
{
	ccedict_t *e=ccmalloc_T(ccedict_t);
  e->kind=ccedict_kTERNARY;
  e->ternary.cnd=cnd;
  e->ternary.lhs=lhs;
  e->ternary.rhs=rhs;

  return e;
}

#endif