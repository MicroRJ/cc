#ifndef _CCEXEC_VALUE
#define _CCEXEC_VALUE

typedef enum ccexec_value_k ccexec_value_k;
typedef enum ccexec_value_k
{ ccev_kINVALID=0,
	ccev_kRVALUE,
	ccev_kLVALUE,
} ccexec_value_k;

typedef struct ccexec_value_t ccexec_value_t;
typedef struct ccexec_value_t
{ ccstr_t           label; // Note: for debugging
	ccexec_value_k    kind;


	union
	{ void    * value;
		ccf64_t   asf64; ccf32_t   asf32;
		ccu64_t   asu64; cci64_t   asi64;
		ccu32_t   asu32; ccu32_t   asi32;
		ccu16_t   asu16; ccu16_t   asi16;
		ccu8_t    asu8;   ccu8_t    asi8;
	};
} ccexec_value_t;

typedef struct ccexec_stack_t ccexec_stack_t;
typedef struct ccexec_stack_t
{ ccemit_procd_t *function;

	ccemit_block_t *current;
	cci32_t         irindex;

	ccexec_value_t *values;
} ccexec_stack_t;

typedef struct ccexec_t ccexec_t;
typedef struct ccexec_t
{ ccemit_t * emit;
} ccexec_t;

ccfunc ccemit_value_t *
ccprocd_local(ccemit_procd_t *func, cctree_t *tree)
{ // Todo: check tree ...
	ccemit_value_t **v=cctblgetP(func->local,tree);
	if(ccerrnon()) return *v;
  return ccnil;
}

ccfunc ccemit_value_t *
ccprocd_include_local(ccemit_procd_t *func, cctree_t *tree, int is_param)
{
	ccnotnil(tree);
	ccassert(tree->kind==cctree_kDECLNAME);

// Todo: check tree ...
	ccedict_t *e=is_param?ccedict_param(tree):ccedict_local(tree);
  ccemit_value_t  *i=ccblock_add_edict(func->decls,e);
  ccemit_value_t **v=cctblputP(func->local,tree);
  ccassert(ccerrnon());
  *v=i;
  return i;
}

ccfunc ccinle ccexec_value_t
ccexec_rvalue(void *value, ccstr_t label)
{ ccexec_value_t t;
	t.kind=ccev_kRVALUE;
	t.value=value;
	t.label=label;
	return t;
}

ccfunc ccinle ccexec_value_t
ccexec_lvalue(void *value, ccstr_t label)
{ ccexec_value_t t;
	t.kind=ccev_kLVALUE;
	t.value=value;
	t.label=label;
	return t;
}

#endif