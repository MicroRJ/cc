// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCSEEK
#define _CCSEEK

// Todo: weird


// Note: the purpose of this file is create a map of dependencies, symbols, expressions and types and
// to check the validity of the tree ...
ccfunc ccinle void
ccseek_decl(cctree_t *);

ccfunc ccinle void
ccseek_tree(cctree_t *);

// Note: this should not bind to a tree, it should bind to a type ...
ccfunc int
ccseek_include_invokable(cctree_t *tree, const char *name)
{
  ccnotnil(tree);
  ccnotnil(name);

  cctree_t **value=cctblputS(func_decls,name);
  if(ccerrnon()) *value=tree;

  return ccerrnon();
}

// Note: find the symbol associated with the given tree and return it ...
ccfunc cctree_t *
ccseek_symbol(cctree_t *tree)
{
  ccnotnil(tree);

  cctree_t **symbol=ccnil;
  symbol=cctblgetP(symbols,tree);

  return ccerrnon()? *symbol :ccnil;
}

// I keep coming up with these weird names ...
ccfunc int
ccseek_shackle(cctree_t *tree, const char *name)
{
  cctree_t **solved=ccnil;

  ccerrset(ccerr_kNIT);

  if((tree->kind==cctree_kINDEX)&&
     (tree->lval->kind!=cctree_kLITIDE))
  {
    ccassert(!"error");
  }

  if((tree->kind==cctree_kLITIDE) || // Note: to figure out what variable we're referring to ..
     (tree->kind==cctree_kINDEX))    // Note: to figure out what variable we're referring to
    solved=cctblgetS(vari_decls,name);
  else
  if((tree->kind==cctree_kCALL)) // Note: to figure out what function we're referring to ..
    solved=cctblgetS(func_decls,name);
  else
    cctraceerr("'%s[0x%x]': invalid mingling tree, expected CALL or IDENTIFIER",
      cctree_s[tree->kind],tree);

  if(ccerrnon())
  {
    cctree_t **symbol=cctblputP(symbols,tree);
    ccassert(ccerrnon());
    *symbol=*solved;
  }

  return ccerrnon();
}

ccfunc void
ccseek_binary(cctoken_k oper, cctree_t *lvalue, cctree_t *rvalue);
ccfunc void
ccseek_rvalue(cctree_t *tree);
ccfunc void
ccseek_lvalue(cctree_t *tree);

ccfunc void
ccseek_call(cctree_t *tree)
{
  ccassert(tree->lval);
  // ccassert(tree->rval); Could be that it has no arguments ...

  if(!ccseek_shackle(tree,tree->name))
      cctraceerr("%s: identifier not found",tree->name);

  cctree_t *rval;
  ccarrfor(tree->rval,rval)
    ccseek_rvalue(rval);
}

ccfunc void
ccseek_index(cctree_t *tree)
{
  ccassert(tree->lval);
  ccassert(tree->rval);

  if(tree->lval->kind==cctree_kLITIDE)
  {
    if(!ccseek_shackle(tree->lval,tree->lval->name))
        cctraceerr("%s: identifier not found",tree->lval->name);
  } else
  if(tree->lval->kind==cctree_kINDEX)
  {
    ccseek_index(tree->lval);
  } else
  {
    ccassert(!"error");
  }

  ccseek_rvalue(tree->rval);
}

ccfunc void
ccseek_lvalue(cctree_t *tree)
{
  switch(tree->kind)
  {
    case cctree_kLITIDE:
    {
      if(!ccseek_shackle(tree,tree->name))
        cctraceerr("'%s': undeclared lvalue identifier",tree->name);
    } break;
    case cctree_kINDEX:
    {
      ccseek_index(tree);
    } break;
    default: ccassert(!"internal");
  }
}

ccfunc void
ccseek_rvalue(cctree_t *tree)
{
  switch(tree->kind)
  { case cctree_kLITINT:
    break;
    case cctree_kLITIDE:
    {
      if(!ccseek_shackle(tree,tree->name))
        cctraceerr("'%s': undeclared rvalue identifier",tree->name);

    } break;
    case cctree_kBINARY:
    {
      ccseek_binary(tree->oper,tree->lval,tree->rval);
    } break;
    case cctree_kCALL:
    {
      ccseek_call(tree);
    } break;
    case cctree_kINDEX:
    {
      ccseek_index(tree);
    } break;
    default: ccassert(!"internal");
  }
}

ccfunc void
ccseek_binary(cctoken_k oper, cctree_t *lvalue, cctree_t *rvalue)
{
  if(oper==cctoken_kASSIGN)
    ccseek_lvalue(lvalue);
  else
    ccseek_rvalue(lvalue);

  ccseek_rvalue(rvalue);
}

ccfunc void
ccseek_decl_name(cctree_t *tree)
{
  // Note: is this a good way to do things?
  if(tree->type->kind==cctree_kFUNC)
  {
    if(tree->mark&cctree_mEXTERNAL)
    {
      if(ccseek_include_invokable(tree,tree->name))
      {
        cctree_t **list;
        ccarrfor(tree->type->list,list)
          ccseek_decl_name(*list);

        ccarrfor(tree->blob->list,list)
          ccseek_tree(*list);
      } else
          cctraceerr("%s: already has a body", tree->name);
    } else
        cctraceerr("'%s': local function definitions are illegal", tree->name);
  } else
  {
    cctree_t **value=cctblputS(vari_decls,tree->name);
    if(ccerrnon())
      *value=tree;
    else
      cctraceerr("'%s': variable redefinition", tree->name);

    if(tree->init)
    {
      ccseek_rvalue(tree->init);
    }

    if(tree->type->kind==cctree_kARRAY)
    {

      ccseek_rvalue(tree->type->rval);

    } else
    if(tree->type->kind==cctree_kTYPENAME)
    {

    } else
    {
      ccassert(!"error");
    }
  }
}

ccfunc void
ccseek_tree(cctree_t *tree)
{
  if(tree->kind==cctree_kBLOCK)
  {
    cctree_t **list;
    ccarrfor(tree->list,list)
      ccseek_tree(*list);
  } else
  if(tree->kind==cctree_kDECL)
  {
    if(tree->root->kind==cctree_kTUNIT)
      ccassert(tree->mark&cctree_mEXTERNAL);

    if(tree->mark&cctree_mEXTERNAL)
      ccassert(tree->root->kind==cctree_kTUNIT);

    cctree_t **list;
    ccarrfor(tree->list,list)
      ccseek_decl_name(*list);
  } else
  if(tree->kind==cctree_kCALL)
  {
    ccseek_call(tree);

  } else
  if(tree->kind==cctree_kRETURN)
  {
    ccassert(tree->rval);
    ccseek_rvalue(tree->rval);
  } else
  if(tree->kind==cctree_kBINARY)
  {
    ccseek_binary(tree->oper,tree->lval,tree->rval);
  } else
  if(tree->kind==cctree_kWHILE)
  {
    ccseek_rvalue(tree->init);
    ccseek_tree(tree->lval);
  } else
  if(tree->kind==cctree_kTERNARY)
  {
    ccseek_rvalue(tree->init);
    if(tree->lval) ccseek_tree(tree->lval);
    if(tree->rval) ccseek_tree(tree->rval);
  } else
  {
    ccassert(!"error");
  }
}

ccfunc void
ccseek_translation_unit(cctree_t *tree)
{ ccassert(tree!=0);
  ccassert(tree->kind==cctree_kTUNIT);

  if(type_decls) ccdlbdel(type_decls);
  if(func_decls) ccdlbdel(func_decls);
  if(vari_decls) ccdlbdel(vari_decls);
  if(symbols) ccdlbdel(symbols);


  // Note: this is temporary
  cctree_t *tree_ccbreak=cctree_new(cctree_kFUNC,tree,cctree_mEXTERNAL);
  cctree_t *tree_ccerror=cctree_new(cctree_kFUNC,tree,cctree_mEXTERNAL);

  ccseek_include_invokable(tree_ccbreak,"ccbreak");
  ccseek_include_invokable(tree_ccerror,"ccerror");

  cctree_t **decl;
  ccarrfor(tree->list,decl) ccseek_tree(*decl);
}
#endif