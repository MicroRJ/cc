#ifndef _CCEMIT_VALUE
#define _CCEMIT_VALUE

typedef enum ccvalue_k ccvalue_k;
typedef enum ccvalue_k
{ ccvalue_Kinvalid=0,
  ccvalue_Kblock,
  ccvalue_kCONST,
  ccvalue_kGLOBAL,
  ccvalue_kFUNC,
  ccvalue_kEDICT,
} ccvalue_k;

typedef struct ccemit_value_t ccemit_value_t;
typedef struct ccemit_value_t
{
	ccvalue_k kind;
#if defined(_DEBUG)
struct
#else
union
#endif
{ ccedict_t    *edict;
	ccemit_procd_t *func;
  ccemit_block_t    *block;
	struct
	{ cctree_t   * type;
		ccclassic_t  clsc;
	} constant;
};
} ccemit_value_t;

typedef struct ccemit_block_t ccemit_block_t;
typedef struct ccemit_block_t
{ ccstr_t          label; // Note: for debugging?
  ccemit_value_t **edict;
} ccemit_block_t;

typedef struct ccemit_t ccemit_t;
typedef struct ccemit_t
{ ccemit_value_t ** globals;
  ccemit_value_t *  entry;
  ccemit_block_t *  current; // Todo:
} ccemit_t;


ccfunc ccemit_block_t *
ccblock(ccstr_t label)
{
	ccemit_block_t *block=ccmalloc_T(ccemit_block_t);
	block->label=label;
	block->edict=ccnil;
  return block;
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

#endif