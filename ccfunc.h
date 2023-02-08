#ifndef _CCFUNC
#define _CCFUNC

typedef struct ccfunction_t
{ cctree_t         *tree;
  ccemit_value_t * *local;
  ccblock_t        *block;
  ccblock_t        *decls;
  ccblock_t        *enter;
  ccblock_t        *leave;
} ccfunction_t;


ccfunc ccemit_value_t *
ccfunc_local(ccfunction_t *func, cctree_t *tree)
{ // Todo: check tree ...
	ccemit_value_t **v=cctblgetP(func->local,tree);
	if(ccerrnon()) return *v;
  return ccnil;
}

ccfunc ccemit_value_t *
ccfunc_include_local(ccfunction_t *func, cctree_t *tree, int is_param)
{ // Todo: check tree ...
	ccedict_t *e=is_param?ccedict_param(tree):ccedict_local(tree);
  ccemit_value_t  *i=ccblock_add_edict(func->decls,e);
  ccemit_value_t **v=cctblputP(func->local,tree);
  ccassert(ccerrnon());
  *v=i;
  return i;
}
#endif