// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCTREE
#define _CCTREE

typedef enum cctree_k
{ cctree_kTYPENAME,
  cctree_kSTRUCT,
  cctree_kENUM,
  cctree_kFUNC,
  cctree_kARRAY,
  cctree_kPOINTER,
  cctree_kLITIDE,
  cctree_kLITINT,
  cctree_kLITFLO,
  cctree_kLITSTR,
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
  cctree_kINDEX,
  cctree_kTUNIT,
  cctree_t_designator,
  cctree_t_designation,
} cctree_k;

ccglobal const char *cctree_s[]=
{ "kTYPENAME",
  "kSTRUCT",
  "kENUM",
  "kFUNC",
  "kARRAY",
  "kPOINTER",
  "kIDENTIFIER",
  "kINTEGER",
  "kFLOAT",
  "kSTRING",
  "kBLOCK",
  "kLABEL",
  "kRETURN",
  "kGOTO",
  "kWHILE",
  "kDECLNAME",
  "kDECL",
  "kTERNARY",
  "kBINARY",
  "kUNARY",
  "kGROUP",
  "kCALL",
  "kINDEX",
  "kTUNIT",
  "t_designator",
  "t_designation",
};


#define cctree_mVARIADIC (0x01<<0x00)
#define cctree_mCONSTANT (0x01<<0x01)
#define cctree_mLVALUE   (0x01<<0x02)
#define cctree_mRVALUE   (0x01<<0x03)
#define cctree_mEXTERNAL (0x01<<0x04)

typedef struct cctree_t cctree_t;

// Note: slowly but surely compact this ...
typedef struct cctree_t
{
  cctree_k    kind;

  const char *loca;

  cctree_t   *root;
  cci32_t     mark;

  ccstr_t     name;
  cctree_t  **list; // Todo: demote this to just *
  cctoken_k   oper;
  cctree_t  * type;
  cctree_t  * size; // Note: the size constant expression `unsigned a: 1`
  cctree_t  * init;
  cctree_t  * lval;
  cctree_t  * rval;

  // Note: remove this ...
  cctree_t  * blob;

  // Todo:
  union
  { ccstr_t as_str;

    ccf64_t as_f64;

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
{ // Todo: proper tree allocator ...
  cctree_t *tree=ccmalloc_T(cctree_t);
  memset(tree,ccnil,sizeof(*tree));
  tree->kind=kind;
  tree->root=root;
  tree->mark=mark;
  return tree;
}

ccfunc cctree_t *
cctree_clone(cctree_t *tree)
{ cctree_t *result=ccmalloc_T(cctree_t);
  *result=*tree;
  return result;
}

ccfunc ccstr_t
cctree_name(cctree_t *name)
{
  return name?name->name:0;
}

ccfunc cctree_t *
cctree_pointer_modifier(cctree_t *type)
{ cctree_t *tree=cctree_new(cctree_kPOINTER,0,0);
  tree->type=type;
  return tree;
}

ccfunc cctree_t *
cctree_array_modifier(cctree_t *type, cctree_t *rval)
{ cctree_t *tree=cctree_new(cctree_kARRAY,0,0);
  tree->type=type;
  tree->rval=rval;
  return tree;
}

ccfunc cctree_t *
cctree_function_modifier(cctree_t *type, cctree_t **list)
{ cctree_t *tree=cctree_new(cctree_kFUNC,0,0);
  tree->type=type;
  tree->list=list;
  return tree;
}

ccfunc cctree_t *
cctree_struct_specifier(cctree_t **list, cctree_t *name)
{ ccassert(list!=0);
  cctree_t *tree=cctree_new(cctree_kSTRUCT,0,0);
  tree->list=list;
  tree->name=cctree_name(name);
  return tree;
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
cctree_call(cctree_t *root, cci32_t mark, cctree_t *lval, cctree_t *rval, ccstr_t name)
{ cctree_t *tree=cctree_new(cctree_kCALL,root,mark);
  tree->lval=lval;
  tree->rval=rval;
  tree->name=name;
  return tree;
}

ccfunc cctree_t *
cctree_index(cctree_t *root, cci32_t mark, cctree_t *lval, cctree_t *rval, ccstr_t name)
{ cctree_t *tree=cctree_new(cctree_kINDEX,root,mark);
  tree->lval=lval;
  tree->rval=rval;
  tree->name=name;
  return tree;
}

ccfunc cctree_t *
cctree_block(cctree_t *root, cci32_t mark, cctree_t **list)
{ cctree_t *tree=cctree_new(cctree_kBLOCK,root,mark);
  tree->list=list;
  return tree;
}

ccfunc cctree_t *
cctree_while(cctree_t *root, cci32_t mark, cctree_t *cond_tree, cctree_t *then_tree)
{ ccassert(cond_tree!=0);
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
cctree_decl_name(cctree_t *root, cci32_t mark, cctree_t *type, cctree_t *name, cctree_t *size, cctree_t *init)
{ cctree_t *tree=cctree_new(cctree_kDECLNAME,root,mark);
  tree->type=type;
  tree->name=cctree_name(name);
  tree->size=size;
  tree->init=init;
  return tree;
}

ccfunc cctree_t *
cctree_decl(cctree_t *root, cci32_t mark, cctree_t *type, cctree_t **list)
{ cctree_t *tree=cctree_new(cctree_kDECL,root,mark);
  tree->type=type;
  tree->list=list;
  return tree;
}

ccfunc cctree_t *
cctree_litide(cctree_t *root, cci32_t mark, const char *loca, ccstr_t name)
{ cctree_t *tree=cctree_new(cctree_kLITIDE,root,mark);
  tree->name=name;

  tree->loca=loca;
  return tree;
}

ccfunc cctree_t *
cctree_litint(cctree_t *root, cci32_t mark, cctoken_t *token)
{ cctree_t *result = cctree_new(cctree_kLITINT,root,mark);
  result->as_i64=token->asi64;
  return result;
}

ccfunc cctree_t *
cctree_litflo(cctree_t *root, cci32_t mark, cctoken_t *token)
{ cctree_t *result = cctree_new(cctree_kLITFLO,root,mark);
  result->as_f64=token->asf64;
  return result;
}

ccfunc cctree_t *
cctree_litstr(cctree_t *root, cci32_t mark, cctoken_t *token)
{ cctree_t *result = cctree_new(cctree_kLITSTR,root,mark);
  result->as_str=token->str;
  return result;
}

ccfunc cctree_t *
cctree_unary(cctree_t *root, cci32_t mark, cctoken_k kind, cctree_t *rval)
{ cctree_t *result=cctree_new(cctree_kUNARY,root,mark);
  result->oper=kind;
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


#if 0
ccfunc cctree_t *
cctree_new_designation(cctree_t *list, cctree_t *init)
{
  // Make sure we return null here, not just for safety but because other functions
  // depend on it for convenience.
  if(list)
  {
    cctree_t *tree=cctree_new(cctree_t_designation);
    tree->designation.list = list;
    tree->designation.init = init;
    return tree;
  }
  return ccnil;
}
#endif


#endif