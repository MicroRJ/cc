#ifndef _CCEDICT
#define _CCEDICT

typedef enum ccedict_k
{
	ccedict_kLOCAL = 0,
	ccedict_kPARAM,
	ccedict_kADDRESS,
	ccedict_kSTORE,
	ccedict_kFETCH,
  ccedict_kARITH,
  ccedict_kJUMP,
  ccedict_kJUMPT,
  ccedict_kJUMPF,
  ccedict_kJUMPC,
  ccedict_kTERNARY,
  ccedict_kENTER,
  ccedict_kINVOKE,
  ccedict_kRETURN,
} ccedict_k;

ccglobal const char *ccedict_s[]=
{ "LOCAL","PARAM","ADDRESS","STORE","FETCH","ARITH","JUMP","JUMPT","JUMPF","JUMPC","TERNARY","ENTER","INVOKE","RETURN",
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
	{ ccemit_value_t * lval;
		ccemit_value_t * rval;
	} addr;
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
		ccemit_value_t  * rval;
	} fetch;
	// Note: Produces a non-addressable rvalue
	struct
	{ ccemit_procd_t  * call;
		ccemit_value_t ** rval;
	} invoke;
	struct
	{ ccemit_value_t  * rval;
	} ret;
	struct
	{ ccemit_block_t  * blc;
		ccu32_t           tar;
		ccemit_value_t  * cnd;
	} jump;
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

typedef struct ccjump_point_t ccjump_point_t;
typedef struct ccjump_point_t
{ ccstr_t         label; // Note: for debugging
  ccemit_block_t *block;
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


ccfunc ccinle ccedict_t *
ccedict_local(cctype_t *type, ccstr_t label)
{
	ccnotnil(type);

	// Todo: check the tree
	ccedict_t *e=ccedict(ccedict_kLOCAL,label);

	e->local.type=type;
	return e;
}

ccfunc ccinle ccedict_t *
ccedict_param(cctype_t *type, ccstr_t label)
{
	ccnotnil(type);

	// Todo: check the tree
	ccedict_t *e=ccedict(ccedict_kPARAM,label);

	e->local.type=type;
	return e;
}

ccfunc ccinle void
ccedict_retarget(ccedict_t *e, ccjump_point_t p)
{
	e->jump.blc=p.block;
	e->jump.tar=p.index;
}

ccfunc ccinle ccedict_t *
ccedict_store(ccemit_value_t *lval, ccemit_value_t *rval)
{
	ccedict_t *e=ccedict(ccedict_kSTORE,"store");
	e->store.lval=lval;
	e->store.rval=rval;
	return e;
}

ccfunc ccinle ccedict_t *
ccedict_fetch(ccemit_value_t *lval, ccemit_value_t  *rval)
{
	ccedict_t *e=ccedict(ccedict_kFETCH,"fetch");
	e->fetch.lval=lval;
	e->fetch.rval=rval;
	return e;
}

ccfunc ccinle ccedict_t *
ccedict_arith(cctoken_k opr, ccemit_value_t *lhs, ccemit_value_t *rhs)
{
	ccedict_t *e=ccedict(ccedict_kARITH,"arith");
	e->arith.opr=opr;
	e->arith.lhs=lhs;
	e->arith.rhs=rhs;
	return e;
}

ccfunc ccinle ccedict_t *
ccedict_enter(ccemit_block_t *blc)
{
	ccedict_t *e=ccedict(ccedict_kENTER,"enter");
	e->enter.blc=blc;
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
ccedict_tjump(ccjump_point_t point, ccemit_value_t *cnd)
{
	ccedict_t *e=ccedict(ccedict_kJUMPT,point.label);
  e->jump.blc=point.block;
  e->jump.tar=point.index;
  e->jump.cnd=cnd;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_fjump(ccjump_point_t point, ccemit_value_t *cnd)
{
	ccedict_t *e=ccedict(ccedict_kJUMPF,point.label);
  e->jump.blc=point.block;
  e->jump.tar=point.index;
  e->jump.cnd=cnd;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_cjump(ccjump_point_t point, ccemit_value_t *cnd)
{
	ccedict_t *e=ccedict(ccedict_kJUMPC,"jump-c");
  e->jump.blc=point.block;
  e->jump.tar=point.index;
  e->jump.cnd=cnd;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_call(ccemit_procd_t *lval, ccemit_value_t **rval)
{
	ccassert(lval!=0);

	ccedict_t *e=ccedict(ccedict_kINVOKE,"invoke");
	e->invoke.call=lval;
	e->invoke.rval=rval;
	return e;
}

ccfunc ccinle ccedict_t *
ccedict_return(ccemit_value_t *rval)
{
	ccedict_t *e=ccedict(ccedict_kRETURN,"return");
  e->ret.rval=rval;
  return e;
}

ccfunc ccinle ccedict_t *
ccedict_address(ccemit_value_t *lval, ccemit_value_t *rval)
{
	ccedict_t *e=ccedict(ccedict_kADDRESS,"address");
  e->addr.lval=lval;
  e->addr.rval=rval;
  return e;
}

// Note: if anything, these should be values, that way you can execute a block, or an instruction ...
ccfunc ccinle ccedict_t *
ccedict_ternary(ccemit_value_t *cnd, ccemit_block_t *lhs, ccemit_block_t *rhs)
{
	ccedict_t *e=ccedict(ccedict_kTERNARY,"ternary");
  e->ternary.cnd=cnd;
  e->ternary.lhs=lhs;
  e->ternary.rhs=rhs;
  return e;
}

#endif