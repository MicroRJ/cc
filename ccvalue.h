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
{ struct
	{ cctree_t   * type;
		ccclassic_t  clsc;
	} constant;

  ccfunction_t *func;
  ccedict_t    *edict;
  ccblock_t    *block;
};
} ccemit_value_t;

#endif