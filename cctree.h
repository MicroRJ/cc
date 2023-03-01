// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCTREE
#define _CCTREE

typedef enum cctree_k cctree_k;
typedef enum cctree_k
{
  cctree_kTYPENAME,
  cctree_kSTRUCT,
  cctree_kENUM,
  cctree_kFUNCTION,
  cctree_kARRAY,
  cctree_kPOINTER,

  cctree_kIDENTIFIER,

  cctree_kCONSTANT,

  cctree_kBLOCK,
  cctree_kLABEL,
  cctree_kRETURN,
  cctree_kGOTO,
  cctree_kWHILE,

  cctree_kDECL,

  // Note: this is the same for ternarny conditionals, main difference is that it is not a statement unlike if ...
  cctree_kCONDITIONAL,

  // Note: i think I want to keep these for purely bitwise and logical operators, and keep others, such as '&' or '*' or '[]' or '()'
  // as their own trees...
  cctree_kBINARY,
  cctree_kUNARY,

  cctree_kGROUP,

  cctree_kDEREFERENCE,
  cctree_kADDRESSOF,
  cctree_kSIZEOF,
  cctree_kCALL,
  cctree_kINDEX,
  cctree_kSELECTOR,

  cctree_kTUNIT,

  cctree_t_designator,
  cctree_t_designation,
} cctree_k;

#define cctree_mVARIADIC (0x01<<0x00)
#define cctree_mCONSTANT (0x01<<0x01)
#define cctree_mLVALUE   (0x01<<0x02)
#define cctree_mRVALUE   (0x01<<0x03)
#define cctree_mEXTERNAL (0x01<<0x04)

#define cctree_casti64(tree) ((tree)?cccast(cci64_t,tree->name):0)
#define cctree_castu64(tree) ((tree)?cccast(ccu64_t,tree->name):0)

#define cctree_casti32(tree) cccast(cci32_t,cctree_casti64(tree))
#define cctree_castu32(tree) cccast(ccu32_t,cctree_castu64(tree))


// Note: slowly but surely compact this ...
typedef struct cctree_t cctree_t;
typedef struct cctree_t
{
  cctree_k    kind;

  char      * name; // Note: this is also the value of a constant tree, cast it to the appropriate type ...

  char      * loca; // Todo: make this a legit location!

  cctoken_k   sort;

  cctree_t   *root; // Todo: is this silly?
  cci32_t     mark;

  cctree_t   *next;

  // Todo: should we demote this to just *
  cctree_t  **list;

  cctree_t  * type;
  cctree_t  * size; // Note: the size constant expression `unsigned a: 1`
  cctree_t  * init;

  // Todo: can we rename this to ltree and rtree?
  cctree_t  * lval;
  cctree_t  * rval;

  // Note: can we remove this?
  cctree_t  * blob;


  // Todo: pending ...
  union
  { struct
    { cctoken_t   token;
      cctree_t  * expr;
      cctree_t  * next;
    } designator;
    struct
    { cctree_t  * list;
      cctree_t  * init;
      cctree_t  * next;
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
{ cctree_t *tree=cctree_new(cctree_kFUNCTION,0,0);
  tree->type=type;
  tree->list=list;
  return tree;
}

ccfunc cctree_t *
cctree_translation_unit()
{ cctree_t *tree=cctree_new(cctree_kTUNIT,ccnil,ccnil);
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
cctree_decl(cctree_t *root, cci32_t mark, cctree_t *type, cctree_t *name, cctree_t *size, cctree_t *init)
{ cctree_t *tree=cctree_new(cctree_kDECL,root,mark);
  tree->type=type;
  tree->name=cctree_name(name);
  tree->size=size;
  tree->init=init;
  return tree;
}

ccfunc cctree_t *
cctree_identifier(cctree_t *root, cci32_t mark, cctoken_t *token)
{ cctree_t *tree=cctree_new(cctree_kIDENTIFIER,root,mark);
  tree->name=token->name;
  return tree;
}

ccfunc cctree_t *
cctree_constant(cctree_t *root, cci32_t mark, cctoken_t *token)
{ cctree_t *tree=cctree_new(cctree_kCONSTANT,root,mark);
  tree->name=token->name;
  tree->sort=token->kind;
  return tree;
}

ccfunc cctree_t *
cctree_suffixed(cctree_t *root, cci32_t mark, cctree_k kind, cctree_t *lval, cctree_t *rval)
{ cctree_t *result=cctree_new(kind,root,mark);
  result->kind=kind;
  result->lval=lval;
  result->rval=rval;
  return result;
}

ccfunc cctree_t *
cctree_unary_ex(cctree_t *root, cci32_t mark, cctree_k kind, cctree_t *lval)
{
  ccassert(lval!=0);

  cctree_t *result=cctree_new(kind,root,mark);
  result->kind=kind;
  result->lval=lval;
  return result;
}


ccfunc cctree_t *
cctree_unary(cctree_t *root, cci32_t mark, cctoken_k sort, cctree_t *lval)
{
  ccassert(lval!=0);

  cctree_t *result=cctree_new(cctree_kUNARY,root,mark);
  result->sort=sort;
  result->lval=lval;
  return result;
}

ccfunc cctree_t *
cctree_binary(cctree_t *root, cci32_t mark, cctoken_t *token, cctree_t *lhs, cctree_t *rhs)
{ cctree_t *result=cctree_new(cctree_kBINARY,root,mark);
  result->sort=token->kind;
  result->lval=lhs;
  result->rval=rhs;
  return result;
}

ccfunc cctree_t *
cctree_conditional(cctree_t *root, cci32_t mark, cctree_t *cond_tree, cctree_t *then_tree, cctree_t *else_tree)
{ cctree_t *result=cctree_new(cctree_kCONDITIONAL,root,mark);
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

ccfunc char *
cctree_string(cctree_t *tree, char **stringer)
{
  ccassert(tree!=0);

  char *buffer=stringer?*stringer:ccnull;

  switch(tree->kind)
  {
    case cctree_kCONSTANT:
      if(tree->sort==cctoken_kLITINT)
        ccstrcatS(buffer,ccformat("%lli",cctree_casti64(tree)));
      else
      if(tree->sort==cctoken_kLITSTR)
        ccstrcatS(buffer,tree->name);
      else
      ccassert(!"error");
    break;
    case cctree_kSTRUCT:
      ccstrcatL(buffer,"struct");
    break;
    case cctree_kENUM:
      ccstrcatL(buffer,"enum");
    break;
    case cctree_kIDENTIFIER:
    case cctree_kTYPENAME:
      ccstrcatS(buffer,tree->name);
    break;
    case cctree_kFUNCTION:
      cctree_string(tree->type,&buffer);
      ccstrcatL(buffer,"(..)");
    break;
    case cctree_kARRAY:
      ccstrcatL(buffer,"[");
        cctree_string(tree->rval,&buffer);
      ccstrcatL(buffer,"]");
      cctree_string(tree->type,&buffer);
    break;
    case cctree_kPOINTER:
      ccstrcatL(buffer,"*");
      cctree_string(tree->type,&buffer);
    break;

    case cctree_kINDEX:
      cctree_string(tree->lval,&buffer);
      ccstrcatL(buffer,"[");
        cctree_string(tree->rval,&buffer);
      ccstrcatL(buffer,"]");
    break;
  }

  if(stringer) *stringer=buffer;

  return buffer;
}


#endif