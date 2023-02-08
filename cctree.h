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

  cctree_kWHILE,

  cctree_kDECLNAME,
  cctree_kDECL,

  cctree_kTERNARY,
  cctree_kBINARY,
  cctree_kUNARY,
  cctree_kGROUP,
  cctree_kCALL,

  cctree_kTUNIT,

  cctree_t_designator,
  cctree_t_designation,
} cctree_k;

#define cctree_mVARIADIC (0x01<<0x00)
#define cctree_mCONSTANT (0x01<<0x01)
#define cctree_mLVALUE   (0x01<<0x02)
#define cctree_mRVALUE   (0x01<<0x03)
#define cctree_mEXTERNAL (0x01<<0x04)

// Note: slowly but surely compact this ...
typedef struct cctree_t
{ cctree_k    kind;
  cctree_t   *root;
	cci32_t     mark;

  ccstr_t     name;
  cctree_t  **decl;
  cctree_t  **list;
	cctoken_k   oper;
  cctype_t  * type;
  cctree_t  * init; // Note: conditional
  cctree_t  * size; // Note: the size constant expression `unsigned a: 1`
  cctree_t  * lval; // Note: then
  cctree_t  * rval; // Note: else

  // Note: remove this ...
  cctree_t  * blob;

  union
	{ ccstr_t as_str;
		cci64_t as_i64;
	  cci32_t as_i32;
	  cci16_t as_i16;
	  cci8_t  as_i8;
	  ccu64_t as_u64;
	  ccu32_t as_u32;
	  ccu16_t as_u16;
	  ccu8_t  as_u8;
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
cctree_del(cctree_t *tree)
{ ccfree(tree);
}

ccfunc cctree_t *
cctree_new(cctree_k kind, cctree_t *root, cci32_t mark)
{ cctree_t *tree=ccmalloc_T(cctree_t);
  memset(tree,ccnil,sizeof(*tree));
  tree->kind=kind;
  tree->root=root;
  tree->mark=mark;
  return tree;
}

ccfunc ccstr_t
cctree_name(cctree_t *name)
{
  return name?name->name:0;
}

ccfunc cctree_t *
cctree_translation_unit()
{ cctree_t *tree=cctree_new(cctree_kTUNIT,ccnil,ccnil);
	return tree;
}

ccfunc cctree_t *
cctree_group(cctree_t *root, cci32_t mark, cctree_t *init)
{ cctree_t *tree=cctree_new(cctree_kGROUP,root,mark);
	tree->init=init;
	return tree;
}

ccfunc cctree_t *
cctree_call(cctree_t *root, cci32_t mark, cctree_t *lval, cctree_t *rval)
{ cctree_t *tree=cctree_new(cctree_kCALL,root,mark);
	tree->lval=lval;
	tree->rval=rval;
	return tree;
}

ccfunc cctree_t *
cctree_block(cctree_t *root, cci32_t mark, cctree_t **decl, cctree_t **list)
{ cctree_t *tree=cctree_new(cctree_kBLOCK,root,mark);
  tree->decl=decl;
  tree->list=list;
  return tree;
}

ccfunc cctree_t *
cctree_while(cctree_t *root, cci32_t mark, cctree_t *cond_tree, cctree_t *then_tree)
{ ccnotnil(cond_tree);
  cctree_t *tree=cctree_new(cctree_kWHILE,root,mark);
  tree->init=cond_tree;
  tree->lval=then_tree;
  return tree;
}

ccfunc cctree_t *
cctree_label(cctree_t *root, cci32_t mark, cctree_t *name, cctree_t **list)
{ cctree_t *tree=cctree_new(cctree_kLABEL,root,mark);
  tree->name=cctree_name(name);
  tree->list=list;
  return tree;
}

ccfunc cctree_t *
cctree_goto(cctree_t *root, cci32_t mark, cctree_t *name)
{ cctree_t *tree=cctree_new(cctree_kGOTO,root,mark);
  tree->name=cctree_name(name);
  return tree;
}

ccfunc cctree_t *
cctree_return(cctree_t *root, cci32_t mark, cctree_t *rval)
{ cctree_t *tree=cctree_new(cctree_kRETURN,root,mark);
  tree->rval=rval;
  return tree;
}

ccfunc cctree_t *
cctree_decl_name(cctree_t *root, cci32_t mark, cctype_t *type, cctree_t *name, cctree_t *size, cctree_t *init)
{ cctree_t *tree=cctree_new(cctree_kDECLNAME,root,mark);
  tree->type=type;
  tree->name=cctree_name(name);
  tree->size=size;
  tree->init=init;
  return tree;
}

ccfunc cctree_t *
cctree_decl(cctree_t *root, cci32_t mark, cctype_t *type, cctree_t **list)
{ cctree_t *tree=cctree_new(cctree_kDECL,root,mark);
  tree->type=type;
  tree->list=list;
  return tree;
}

ccfunc cctree_t *
cctree_identifier(cctree_t *root, cci32_t mark, cctree_t *lval, const char *name)
{ ccnotnil(name);
	cctree_t *tree=cctree_new(cctree_kIDENTIFIER,root,mark);
  tree->lval=lval;
  tree->name=(ccstr_t)name;
  return tree;
}

ccfunc cctree_t *
cctree_unary(cctree_t *root, cci32_t mark, cctoken_t *token, cctree_t *rval)
{ cctree_t *result=cctree_new(cctree_kUNARY,root,mark);
  result->oper= token->bit;
  result->rval=rval;
  return result;
}

ccfunc cctree_t *
cctree_binary(cctree_t *root, cci32_t mark, cctoken_t *token, cctree_t *lhs, cctree_t *rhs)
{ cctree_t *result=cctree_new(cctree_kBINARY,root,mark);
	result->oper=token->bit;
  result->lval=lhs;
  result->rval=rhs;
  return result;
}

ccfunc cctree_t *
cctree_ternary(cctree_t *root, cci32_t mark, cctree_t *cond_tree, cctree_t *then_tree, cctree_t *else_tree)
{ cctree_t *result=cctree_new(cctree_kTERNARY,root,mark);
  result->init=cond_tree;
  result->lval=then_tree;
  result->rval=else_tree;
  return result;
}

// Todo:
ccfunc cctree_t *
cctree_new_designator(cctree_t *root, cci32_t mark, cctoken_t *token, cctree_t *expr)
{ cctree_t *tree=cctree_new(cctree_t_designator,root,mark);
  tree->designator.token = *token;
  tree->designator.expr  =  expr;
  return tree;
}

// Todo:
ccfunc cctree_t *
cctree_constant(cctree_t *root, cci32_t mark, cctype_t *type, cctoken_t *token)
{ cctree_t *result = cctree_new(cctree_kINTEGER,root,mark);
  result->type  =  type;
  result->as_i64=token->sig;
  return result;
}



#if 0
ccfunc cctree_t *
cctree_new_struct_decl_name(cctree_t *decl, cctree_t *expr)
{ ccassert(decl!=0);
  cctree_t *tree=cctree_new(cctree_Kstruct_decl_name);
  tree->struct_decl_name.decl = decl;
  tree->struct_decl_name.expr = expr;
  return tree;
}
ccfunc cctree_t *
cctree_new_struct_decl(cctype_t *type, cctree_t *list)
{ ccassert(type!=0);
  ccassert(list!=0);
  cctree_t *tree=cctree_new(cctree_Kstruct_decl);
  tree->struct_decl.type = type;
  tree->struct_decl.list = list;
  return tree;
}
#endif

ccfunc cctree_t *
cctree_new_designation(cctree_t *list, cctree_t *init)
{
#if 0
	// Make sure we return null here, not just for safety but because other functions
  // depend on it for convenience.
  if(list)
  {
    cctree_t *tree=cctree_new(cctree_t_designation);
    tree->designation.list = list;
    tree->designation.init = init;
    return tree;
  }
#endif
  return ccnil;
}

ccfunc void
cctree_check(cctree_t *tree)
{
	if(!tree) return;

	if(tree->kind!=cctree_kTUNIT)
	{
		ccnotnil(tree->root);
	}


	cctree_t **list;

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

			if(tree->mark&cctree_mRVALUE)
			{ ccnotnil(tree->init);
				ccnotnil(tree->lval);
				ccnotnil(tree->rval);
			}

			cctree_check(tree->init);
			cctree_check(tree->lval);
			cctree_check(tree->rval);
		break;
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

			ccarrfor(tree->list,list) cctree_check(*list);
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
			ccarrfor(tree->list,list) cctree_check(*list);
		break;

		default: ccassert(!"noimpl");
	}
}
#endif