#ifndef _CCEMIT_VALUE
#define _CCEMIT_VALUE


// Note: boil this down to a function, constant value or instruction, local value, global value
typedef enum ccvalue_K
{ ccvalue_Kinvalid=0,
  ccvalue_Kblock,
  ccvalue_kCONST,
  ccvalue_kGLOBAL,
  ccvalue_kFUNC,
  ccvalue_kEDICT,
} ccvalue_K;

typedef struct ccemit_value_t
{
	ccvalue_K kind;

#if defined(_DEBUG)
struct
#else
union
#endif
{ struct
	{ cctype_t   * type;
		ccclassic_t  clsc;
	} constant;
  ccfunction_t *function;
  ccedict_t    *edict;
  ccblock_t    *block;
};
} ccemit_value_t;

#endif