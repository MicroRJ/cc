#ifndef _CCEXEC_VALUE
#define _CCEXEC_VALUE

typedef enum ccexec_value_k
{
	ccexec_value_kINVALID=0,

	ccexec_value_kCONST,
	ccexec_value_kADDRS,
} ccexec_value_k;

typedef struct ccexec_value_t
{ const char      * debug_label;
	ccexec_value_k    kind;
	cctree_t        * type;
	union
	{ void * value;
		ccf64_t  asf64;
		cci64_t  asi64;
		ccu64_t  asu64;

		ccf32_t  asf32;

		ccu32_t  asu32;
		ccu16_t  asu16;
		ccu8_t   asu8;

		ccu32_t  asi32;
		ccu16_t  asi16;
		ccu8_t   asi8;
	};
} ccexec_value_t;

typedef struct ccemit_block_t ccemit_block_t;
typedef struct ccemit_block_t
{ const char      *label;
  ccemit_value_t **edict;
} ccemit_block_t;

typedef struct ccemit_procd_t
{ cctree_t         *tree;
  ccemit_value_t * *local;
  ccemit_block_t        *block;
  ccemit_block_t        *decls;
  ccemit_block_t        *enter;
  ccemit_block_t        *leave;
} ccemit_procd_t;

typedef struct ccexec_stack_t
{ ccemit_procd_t   *function;

	ccemit_block_t      *current;
	cci32_t         irindex;

	ccexec_value_t *values;
} ccexec_stack_t;

typedef struct ccexec_t
{ ccemit_t       * emit;
  ccexec_value_t * values;
  ccemit_procd_t   * routine;
  ccemit_block_t      * current;
  cci32_t          irindex;
} ccexec_t;


ccfunc ccemit_block_t *
ccblock_I(ccemit_block_t *block, const char *label)
{
  block->label=label?label:"no-label";

  // Todo: we don't have to this anymore
  block->edict=ccnil;
  ccarrres(block->edict,0xff);
  ccarrfix(block->edict);

  return block;
}

ccfunc ccemit_block_t *
ccblock(ccemit_block_t *block, const char *label)
{
  return ccblock_I(ccmalloc_T(ccemit_block_t),label);
}

ccfunc ccemit_value_t *
ccblock_add(ccemit_block_t *block)
{
	ccemit_value_t  *v=ccmalloc_T(ccemit_value_t);
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
ccfunc_local(ccemit_procd_t *func, cctree_t *tree)
{ // Todo: check tree ...
	ccemit_value_t **v=cctblgetP(func->local,tree);
	if(ccerrnon()) return *v;
  return ccnil;
}

ccfunc ccemit_value_t *
ccfunc_include_local(ccemit_procd_t *func, cctree_t *tree, int is_param)
{ // Todo: check tree ...
	ccedict_t *e=is_param?ccedict_param(tree):ccedict_local(tree);
  ccemit_value_t  *i=ccblock_add_edict(func->decls,e);
  ccemit_value_t **v=cctblputP(func->local,tree);
  ccassert(ccerrnon());
  *v=i;
  return i;
}

ccfunc ccinle ccexec_value_t
ccexec_value_I(ccexec_value_k kind, cctree_t *type, void *value, const char *debug_label)
{
	ccexec_value_t t;
	t.kind=kind;
	t.type=type;
	t.value=value;
	t.debug_label=debug_label;
	return t;
}

ccfunc ccinle ccexec_value_t
ccexec_value_i64(cctree_t *type, cci64_t val, const char *debug_label)
{ ccexec_value_t t;
	t.kind=ccexec_value_kCONST;
	t.debug_label=debug_label;
	t.type=type;
	t.asi64=val;
	return t;
}

ccfunc ccinle ccexec_value_t
ccexec_value_u64(cctree_t *type, ccu64_t val, const char *debug_label)
{ ccexec_value_t t;
	t.kind=ccexec_value_kCONST;
	t.debug_label=debug_label;
	t.type=type;
	t.asu64=val;
	return t;
}

ccfunc ccinle ccexec_value_t
ccexec_value_f64(cctree_t *type, ccf64_t val, const char *debug_label)
{ ccexec_value_t t;
	t.kind=ccexec_value_kCONST;
	t.debug_label=debug_label;
	t.type=type;
	t.asf64=val;
	return t;
}

#endif