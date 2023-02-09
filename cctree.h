#ifndef _CCTREE
#define _CCTREE

//
// Todo: make the parser simply be the parser and then add another stage that actually checks everything and
// creates the types, checks the scopes and so on ...
//

typedef enum cctree_k
{
  cctree_kTYPENAME,

  cctree_kSTRUCT,
  cctree_kENUM,

  cctree_kFUNC,
  cctree_kARRAY,
  cctree_kPOINTER,

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

typedef struct cctree_t cctree_t;

ccfunc void cctree_del(cctree_t *);
ccfunc cctree_t *cctree_new(cctree_k, cctree_t *, cci32_t);

ccglobal cctree_t
  *ctype_flo32  = cctree_new(cctree_kTYPENAME,ccnil,ccnil),
  *ctype_flo64  = cctree_new(cctree_kTYPENAME,ccnil,ccnil),
  *ctype_int64  = cctree_new(cctree_kTYPENAME,ccnil,ccnil),
  *ctype_int32  = cctree_new(cctree_kTYPENAME,ccnil,ccnil),
  *ctype_int16  = cctree_new(cctree_kTYPENAME,ccnil,ccnil),
  *ctype_int8   = cctree_new(cctree_kTYPENAME,ccnil,ccnil),
  *ctype_uint64 = cctree_new(cctree_kTYPENAME,ccnil,ccnil),
  *ctype_uint32 = cctree_new(cctree_kTYPENAME,ccnil,ccnil),
  *ctype_uint16 = cctree_new(cctree_kTYPENAME,ccnil,ccnil),
  *ctype_uint8  = cctree_new(cctree_kTYPENAME,ccnil,ccnil),
  *ctype_void   = cctree_new(cctree_kTYPENAME,ccnil,ccnil);


// Note: slowly but surely compact this ...
typedef struct cctree_t
{ cctree_k    kind;
  cctree_t   *root;
  cci32_t     mark;

  ccstr_t     name;
  cctree_t  **list;
  cctoken_k   oper;
  cctree_t  * type;
  cctree_t  * size; // Note: the size constant expression `unsigned a: 1`
  cctree_t  * init; // Note: conditional
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

ccfunc cctree_t *
cctree_clone(cctree_t *tree)
{ cctree_t *result=cctree_new(tree->kind,tree->root,tree->mark);
  *result=*tree;
  return result;
}

ccfunc ccstr_t
cctree_name(cctree_t *name)
{
  return name?name->name:0;
}

ccfunc cctree_t *
cctype_new_ptr(cctree_t *type)
{ cctree_t *tree=cctree_new(cctree_kPOINTER,0,0);
  tree->type=type;
  return tree;
}

ccfunc cctree_t *
cctype_new_arr(cctree_t *type)
{ cctree_t *tree=cctree_new(cctree_kARRAY,0,0);
  tree->type=type;
  return tree;
}

ccfunc cctree_t *
cctype_new_fun(cctree_t *type, cctree_t **list)
{ cctree_t *tree=cctree_new(cctree_kFUNC,0,0);
  tree->type=type;
  tree->list=list;
  return tree;
}

ccfunc cctree_t *
cctree_struct(cctree_t **list, cctree_t *name)
{ ccassert(list!=0);
  cctree_t *tree=cctree_new(cctree_kSTRUCT,0,0);
  tree->list=list;
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
cctree_call(cctree_t *root, cci32_t mark, cctree_t *lval, cctree_t *rval)
{ cctree_t *tree=cctree_new(cctree_kCALL,root,mark);
  tree->lval=lval;
  tree->rval=rval;
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
cctree_identifier(cctree_t *root, cci32_t mark, ccstr_t name)
{ cctree_t *tree=cctree_new(cctree_kIDENTIFIER,root,mark);
  tree->name=name;
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
cctree_constant(cctree_t *root, cci32_t mark, cctree_t *type, cctoken_t *token)
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
cctree_new_struct_decl(cctree_t *type, cctree_t *list)
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

// Todo: this is temporary ...
ccglobal cctree_t **type_decls;
ccglobal cctree_t **func_decls;
ccglobal cctree_t **vari_decls;
ccglobal cctree_t **symbols;


ccfunc ccinle void
cctree_solve_decl(cctree_t *);

ccfunc ccinle void
cctree_solve_statement(cctree_t *);

ccfunc cctree_t *
cctree_resolve_symbol(cctree_t *tree)
{
  cctree_t **symbol=ccnil;
  symbol=cctblgetP(symbols,tree);

  return ccerrnon()? *symbol :ccnil;
}

ccfunc int
cctree_resolve_call_symbol_allusion(cctree_t *tree)
{
  cctree_t **vdecl=cctblgetS(func_decls,tree->name);

  if(ccerrnon())
  {
    cctree_t **symbol=cctblputP(symbols,tree);
    *symbol=*vdecl;
  }

  return ccerrnon();
}

ccfunc int
cctree_resolve_symbol_allusion(cctree_t *tree)
{
  cctree_t **vdecl=cctblgetS(vari_decls,tree->name);

  if(ccerrnon())
  {
    cctree_t **symbol=cctblputP(symbols,tree);
    *symbol=*vdecl;
  }

  return ccerrnon();
}

ccfunc void
cctree_solve_lvalue(cctree_t *tree)
{ switch(tree->kind)
  {
    case cctree_kIDENTIFIER:
    {
      if(!cctree_resolve_symbol_allusion(tree))
        cctraceerr("undeclared lvalue recipient '%s', did you forget to declare the variable?",tree->name);
    } break;
    default: ccassert(!"internal");
  }
}

ccfunc void
cctree_solve_binary(cctoken_k oper, cctree_t *lvalue, cctree_t *rvalue);

ccfunc void
cctree_solve_rvalue(cctree_t *tree)
{ switch(tree->kind)
  { case cctree_kINTEGER:
    break;
    case cctree_kIDENTIFIER:
    {
      if(!cctree_resolve_symbol_allusion(tree))
        cctraceerr("undeclared lvalue address '%s', did you forget to declare the variable?",tree->name);

    } break;
    case cctree_kBINARY:
    {
      cctree_solve_binary(tree->oper,tree->lval,tree->rval);
    } break;
  }
}

ccfunc void
cctree_solve_binary(cctoken_k oper, cctree_t *lvalue, cctree_t *rvalue)
{ if(oper==cctoken_kASSIGN)
    cctree_solve_lvalue(lvalue);
  else
    cctree_solve_rvalue(lvalue);

  cctree_solve_rvalue(rvalue);
}
ccfunc void
cctree_solve_statement(cctree_t *tree)
{
  if(tree->kind==cctree_kDECL)
  {
    cctree_solve_decl(tree);
  } else
  if(tree->kind==cctree_kCALL)
  {
    ccassert(tree->lval);
    ccassert(tree->rval);

    if(!cctree_resolve_call_symbol_allusion(tree->lval))
        cctraceerr("%s: identifier not found",tree->lval->name);

    cctree_t *rval;
    ccarrfor(tree->rval,rval) cctree_solve_rvalue(rval);

  } else
  if(tree->kind==cctree_kBINARY)
  {
    cctree_solve_binary(tree->oper,tree->lval,tree->rval);
  } else
  {
    ccassert(!"error");
  }
}

// Todo: the root of a decl name should always be a declaration,
// with the exception of function parameters ...
ccfunc void
cctree_solve_decl_name(cctree_t *tree)
{
  // Note: is this a good way to do things?
  if(tree->root->kind==cctree_kTUNIT)
  { ccassert(tree->mark&cctree_mEXTERNAL);
  }
  if(tree->mark&cctree_mEXTERNAL)
  { ccassert(tree->root->kind==cctree_kTUNIT);
  }

  if(tree->type->kind==cctree_kFUNC)
  {
    if(tree->mark&cctree_mEXTERNAL)
    {
      cctree_t **value=cctblputS(func_decls,tree->name);
      if(ccerrnon())
      {
        cctree_t **list;
        ccarrfor(tree->type->list,list)
          cctree_solve_decl_name(*list);

        ccarrfor(tree->blob->list,list)
          cctree_solve_statement(*list);

        *value=tree;
      } else
          cctraceerr("%s: already has a body", tree->name);
    } else
        cctraceerr("'%s': local function defintions are illegal", tree->name);
  } else
  {
    cctree_t **value=cctblputS(vari_decls,tree->name);
    if(ccerrnon())
      *value=tree;
    else
      cctraceerr("'%s': variable redefinition", tree->name);

    if(tree->init)
    {
      cctree_solve_rvalue(tree->init);
    }
  }
}

ccfunc ccinle void
cctree_solve_decl(cctree_t *decl)
{ cctree_t **list;
  ccarrfor(decl->list,list) cctree_solve_decl_name(*list);
}

ccfunc void
cctree_solve(cctree_t *tree)
{ ccassert(tree->kind==cctree_kTUNIT);

  cctree_t **decl;
  ccarrfor(tree->list,decl) cctree_solve_decl(*decl);
}

#endif