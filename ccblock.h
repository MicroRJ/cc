#ifndef _CCEMIT_BLOCK
#define _CCEMIT_BLOCK

typedef struct ccblock_t ccblock_t;
typedef struct ccblock_t
{ const char      *label;
  ccemit_value_t **edict;
} ccblock_t;

ccfunc ccblock_t *
ccblock_I(ccblock_t *block, const char *label)
{
  block->label=label?label:"no-label";

  // Todo: we don't have to this anymore
  block->edict=ccnil;
  ccarrres(block->edict,0xff);
  ccarrfix(block->edict);

  return block;
}

ccfunc ccemit_value_t *
ccblock_add(ccblock_t *block)
{
	ccemit_value_t  *v=ccmalloc_T(ccemit_value_t);
	ccemit_value_t **t=ccarradd(block->edict,1);

  memset(v,ccnil,sizeof(*v));

  *t=v;

  return v;
}

ccfunc ccemit_value_t *
ccblock_add_edict(ccblock_t *block, ccedict_t *edict)
{ ccemit_value_t *value=ccblock_add(block);
	value->kind  = ccvalue_kEDICT;
	value->edict = edict;
  return value;
}



#endif