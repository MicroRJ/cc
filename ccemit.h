#ifndef _CCEMIT_VALUE
#define _CCEMIT_VALUE

typedef enum ccvalue_k
{ ccvalue_Kinvalid=0,
  ccvalue_Kblock,
  ccvalue_kCONST,
  ccvalue_kGLOBAL,
  ccvalue_kFUNC,
  ccvalue_kEDICT,
} ccvalue_k;

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

typedef struct ccemit_t
{ ccemit_value_t ** globals;
  ccemit_value_t *  entry;
  ccemit_block_t      *  current;
} ccemit_t;

#endif