#ifndef _CCCHECK
#define _CCCHECK

// Todo: this is temporary ...
ccglobal cctree_t **type_decls;
ccglobal cctree_t **func_decls;
ccglobal cctree_t **vari_decls;
ccglobal cctree_t **symbols;

ccfunc ccinle void
cctree_solve_decl(cctree_t *);

ccfunc ccinle void
cctree_solve_statement(cctree_t *);

ccfunc int
cctree_include_invokable(cctree_t *tree, const char *name)
{
  ccnotnil(tree);
  ccnotnil(name);

  cctree_t **value=cctblputS(func_decls,tree->name);
  if(ccerrnon()) *value=tree;

  return ccerrnon();
}

ccfunc cctree_t *
cctree_resolve_symbol(cctree_t *tree)
{
  ccnotnil(tree);

  cctree_t **symbol=ccnil;
  symbol=cctblgetP(symbols,tree);

  if(ccerrsom())
    cctraceerr("'%s[0x%x]': uncoupled tree, '%s', checker did not do its job",cctree_s[tree->kind],tree,tree->name);

  return ccerrnon()? *symbol :ccnil;
}

ccfunc int
cctree_mingle(cctree_t *tree, const char *name)
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
cctree_solve_binary(cctoken_k oper, cctree_t *lvalue, cctree_t *rvalue);
ccfunc void
cctree_solve_rvalue(cctree_t *tree);
ccfunc void
cctree_solve_lvalue(cctree_t *tree);

ccfunc void
cctree_solve_call(cctree_t *tree)
{
  ccassert(tree->lval);
  ccassert(tree->rval);

  if(!cctree_mingle(tree,tree->name))
      cctraceerr("%s: identifier not found",tree->name);

  cctree_t *rval;
  ccarrfor(tree->rval,rval) cctree_solve_rvalue(rval);
}

ccfunc void
cctree_solve_index(cctree_t *tree)
{
  ccassert(tree->lval);
  ccassert(tree->rval);

  // a[][]
  cctree_t *lval=tree->lval;
  while(lval&&lval->kind!=cctree_kLITIDE)
  	lval=lval->lval;

  ccassert(lval!=ccnil);
  ccassert(lval->kind==cctree_kLITIDE);

  if(!cctree_mingle(lval,lval->name))
      cctraceerr("%s: identifier not found",lval->name);

  cctree_solve_rvalue(tree->rval);
}

ccfunc void
cctree_solve_lvalue(cctree_t *tree)
{
  switch(tree->kind)
  {
    case cctree_kLITIDE:
    {
      if(!cctree_mingle(tree,tree->name))
        cctraceerr("'%s': undeclared lvalue identifier",tree->name);
    } break;
    case cctree_kINDEX:
    {
    	cctree_solve_index(tree);
    } break;
    default: ccassert(!"internal");
  }
}

ccfunc void
cctree_solve_rvalue(cctree_t *tree)
{
  switch(tree->kind)
  { case cctree_kLITINT:
    break;
    case cctree_kLITIDE:
    {
      if(!cctree_mingle(tree,tree->name))
        cctraceerr("'%s': undeclared rvalue identifier",tree->name);

    } break;
    case cctree_kBINARY:
    {
      cctree_solve_binary(tree->oper,tree->lval,tree->rval);
    } break;
    case cctree_kCALL:
    {
      cctree_solve_call(tree);
    } break;
    case cctree_kINDEX:
    {
    	cctree_solve_index(tree);
    } break;
    default: ccassert(!"internal");
  }
}

ccfunc void
cctree_solve_binary(cctoken_k oper, cctree_t *lvalue, cctree_t *rvalue)
{
  if(oper==cctoken_kASSIGN)
    cctree_solve_lvalue(lvalue);
  else
    cctree_solve_rvalue(lvalue);

  cctree_solve_rvalue(rvalue);
}

ccfunc void
cctree_solve_block(cctree_t *block)
{
  cctree_t **tree;
  ccarrfor(block->list,tree) cctree_solve_statement(*tree);
}

ccfunc void
cctree_solve_statement(cctree_t *tree)
{
	if(tree->kind==cctree_kBLOCK)
  {
  	cctree_t **list;
    ccarrfor(tree->list,list)
      cctree_solve_statement(*list);
  } else
  if(tree->kind==cctree_kDECL)
  {
    cctree_solve_decl(tree);
  } else
  if(tree->kind==cctree_kCALL)
  {
    cctree_solve_call(tree);

  } else
  if(tree->kind==cctree_kRETURN)
  {
    ccassert(tree->rval);
    cctree_solve_rvalue(tree->rval);
  } else
  if(tree->kind==cctree_kBINARY)
  {
    cctree_solve_binary(tree->oper,tree->lval,tree->rval);
  } else
  if(tree->kind==cctree_kWHILE)
  {
    cctree_solve_rvalue(tree->init);
    cctree_solve_statement(tree->lval);
  } else
  if(tree->kind==cctree_kTERNARY)
  {
    cctree_solve_rvalue(tree->init);
    if(tree->lval) cctree_solve_block(tree->lval);
    if(tree->rval) cctree_solve_block(tree->rval);
  } else
  {
    ccassert(!"error");
  }
}

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
      if(cctree_include_invokable(tree,tree->name))
      {
        cctree_t **list;
        ccarrfor(tree->type->list,list)
          cctree_solve_decl_name(*list);

        ccarrfor(tree->blob->list,list)
          cctree_solve_statement(*list);
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

    if(tree->type->kind==cctree_kARRAY)
    {

      cctree_solve_rvalue(tree->type->rval);

    } else
    if(tree->type->kind==cctree_kTYPENAME)
    {

    } else
    {
    	ccassert(!"error");
    }
  }
}

ccfunc ccinle void
cctree_solve_decl(cctree_t *decl)
{ cctree_t **list;
  ccarrfor(decl->list,list) cctree_solve_decl_name(*list);
}

ccfunc void
cctree_solve_translation_unit(cctree_t *tree)
{ ccassert(tree->kind==cctree_kTUNIT);

  cctree_t **decl;
  ccarrfor(tree->list,decl) cctree_solve_decl(*decl);
}
#endif