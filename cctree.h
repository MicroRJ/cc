#ifndef _CCTREE
#define _CCTREE

typedef enum cctree_k
{
	cctree_kIDENTIFIER,
  cctree_kINTEGER,
  cctree_kFLOAT,
  cctree_kSTRING,

  cctree_kBLOCK,

  cctree_kLABEL,
  cctree_kRETURN,
  cctree_kGOTO,

  cctree_kIFEL,
  cctree_kWHILE,

  cctree_kDECLNAME,
  cctree_kDECL,

  cctree_kTERNARY,
  cctree_kBINARY,
  cctree_kUNARY,
  cctree_kGROUP,
  cctree_kCALL,

  cctree_t_designator,
  cctree_t_designation,
} cctree_k;

// Note: slowly but surely compact this ...
typedef struct cctree_t
{
  cctree_k kind;

  unsigned is_variadic: 1;
  unsigned is_abstract: 1;
  unsigned is_constant: 1;

  ccstr_t     name;
	cctoken_k   oper;
  cctype_t  * type;
  cctree_t  * list;
  cctree_t  * init; // Note: conditional
  cctree_t  * size; // Note: the size constant expression `unsigned a: 1`
  cctree_t  * lval; // Note: then
  cctree_t  * rval; // Note: else

  // Note: remove this ...
  cctree_t  * blob;

  union
	{ ccstr_t as_str;
		cci64   as_i64;
	  cci32   as_i32;
	  cci16   as_i16;
	  cci8    as_i8;
	  ccu64   as_u64;
	  ccu32   as_u32;
	  ccu16   as_u16;
	  ccu8    as_u8;
	};

	// Todo: pending ...
  union
  { struct
    { cctoken_t   token;
      cctree_t  * expr; // <-- either a const expression or an identifier.
      cctree_t  * next; // <-- part of the designator list.
    } designator;
    struct
    { cctree_t  * list;
      cctree_t  * init;
      cctree_t  * next; // <-- if part of an initializer list.
    } designation;
  };
} cctree_t;




ccfunc void
cctree_check(cctree_t *tree)
{
	if(!tree) return;

	cctree_t *list;
	switch(tree->kind)
	{
		case cctree_kINTEGER:
		case cctree_kFLOAT:
		break;

		case cctree_kUNARY:
			ccnotnil(tree->oper);
			cctree_check(ccnotnil(tree->rval));
		break;
		case cctree_kBINARY:
			ccnotnil(tree->oper);
			cctree_check(ccnotnil(tree->lval));
			cctree_check(ccnotnil(tree->rval));
		break;
		case cctree_kTERNARY:
			ccnotnil(tree->oper);
			cctree_check(ccnotnil(tree->init));
			cctree_check(ccnotnil(tree->lval));
			cctree_check(ccnotnil(tree->rval));
		break;
		case cctree_kIFEL:
		case cctree_kWHILE:
			ccnotnil(tree->init);

			cctree_check(tree->init);
			cctree_check(tree->lval);
			cctree_check(tree->rval);
		break;
		case cctree_kDECLNAME:
			ccnotnil(tree->type);
			ccnotnil(tree->name);

			ccassert(!tree->list);

			cctree_check(tree->init);
			cctree_check(tree->blob);
		break;
		case cctree_kDECL:
			ccnotnil(tree->type);
			ccnotnil(tree->list);

			ccassert(!tree->size);
			ccassert(!tree->name);
			ccassert(!tree->init);

			ccarrfor(tree->list,list) cctree_check(list);
		break;
		case cctree_kLABEL:
		case cctree_kIDENTIFIER:
			ccnotnil(tree->name);
		break;
		case cctree_kSTRING:
			ccnotnil(tree->as_str);
		break;
		case cctree_kBLOCK:
			// ccnotnil(tree->list);
			ccarrfor(tree->list,list) cctree_check(list);
		break;

		default: ccassert(!"noimpl");
	}
}
#endif