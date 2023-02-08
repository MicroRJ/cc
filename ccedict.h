#ifndef _CCEDICT
#define _CCEDICT

typedef enum ccedict_k
{
	ccedict_kLOCAL = 0,
	ccedict_kSTORE,
	ccedict_kFETCH,
  ccedict_kARITH,
  ccedict_kBLOCK,
  ccedict_kCONDI,
  ccedict_kENTER,
  ccedict_kINVOKE,
  ccedict_kRETURN,
} ccedict_k;

ccglobal const char *ccedict_s[]=
{ "LOCAL","STORE","FETCH","ARITH","BLOCK","CONDI","ENTER","INVOKE","RETURN",
};

typedef struct ccedict_t
{ ccedict_k   kind;

#if defined(_DEBUG)
struct
#else
union
#endif
{
	struct
	{
		const char *debug_label;
		cctype_t   *type;
	} local;
	struct
	{ ccemit_value_t *lval;
		ccemit_value_t *rval;
	} store;
	struct
	{ ccemit_value_t *rval;
	} fetch;
	struct
	{ ccemit_value_t * cnd;
  	ccblock_t      * then_blc;
  	ccblock_t      * else_blc;
	} condi;
	struct
	{ ccblock_t      * blc;
	} enter;
	struct
	{ cctoken_k        opr;
	  ccemit_value_t * lhs;
	  ccemit_value_t * rhs;
	} arith;
};
} ccedict_t;

ccfunc ccinle ccedict_t *
ccedict_local(cctype_t *type, const char *label)
{
	ccedict_t *e=ccmalloc_T(ccedict_t);
	e->kind=ccedict_kLOCAL;
	e->local.type=type;
	e->local.debug_label=label;

	ccmem_check(e,sizeof(*e));
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
ccedict_fetch(ccemit_value_t *rval)
{
	ccedict_t *e=ccmalloc_T(ccedict_t);
	e->kind=ccedict_kFETCH;
	e->fetch.rval=rval;
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
ccedict_enter(ccblock_t *blc)
{
	ccedict_t *e=ccmalloc_T(ccedict_t);
	e->kind=ccedict_kENTER;
	e->enter.blc=blc;

	return e;
}

#endif