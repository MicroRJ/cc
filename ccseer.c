// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCSEER_C
#define _CCSEER_C

ccfunc ccinle void
ccseer_tree(ccseer_t *seer, cctree_t *);

// Note: add the tree to the map of variable declarations
ccfunc int
ccseer_include_variable(ccseer_t *seer, cctree_t *tree, const char *name)
{
  ccassert(tree!=0);
  ccassert(name!=0);

  cctree_t **value=cctblputS(seer->vdecl,name);
  if(ccerrnon())
    *value=tree;
  else
    cctraceerr("'%s': variable redefinition",name);

  return ccerrnon();
}

// Note: add a tree to the map of function declarations
ccfunc int
ccseer_include_invokable(ccseer_t *seer, cctree_t *tree, const char *name)
{
  // Todo: check if the's an existing declaration, check if the
  // signature is the same, check if it already has a body and we
  // have a body ...

  ccassert(tree!=0);
  ccassert(name!=0);

  cctree_t **value=cctblputS(seer->fdecl,name);
  if(ccerrnon())
    *value=tree;
  else
    cctraceerr("'%s': function redefinition",name);

  return ccerrnon();
}

// Note: find the 'symbol' that the tree is alluding to ...
ccfunc cctree_t *
ccseer_allusion(ccseer_t *seer, cctree_t *tree)
{
  ccassert(tree!=0);

  cctree_t **symbol=cctblgetP(seer->symbols,tree);

  return ccerrnon()? *symbol :ccnil;
}

// Note: find what the IDENTIFIER or CALL tree is referring to, could be a decl_name or a function, store
// the reference in the symbol table under the given tree ...
ccfunc int
ccseer_allude(ccseer_t *seer, cctree_t *tree, const char *name)
{
  cctree_t **solved=ccnil;

  ccerrset(ccerr_kNIT);

  if((tree->kind==cctree_kLITIDE))
    solved=cctblgetS(seer->vdecl,name);
  else
  if((tree->kind==cctree_kCALL))
    solved=cctblgetS(seer->fdecl,name);
  else
    cctraceerr("'%s[0x%x]': invalid tree, expected CALL or IDENTIFIER",
      cctree_s[tree->kind],tree);

  if(ccerrnon())
  {
    cctree_t **symbol=cctblputP(seer->symbols,tree);
    ccassert(ccerrnon());
    *symbol=*solved;
  }

  return ccerrnon();
}

ccfunc void
ccseer_binary(ccseer_t *seer,cctoken_k oper, cctree_t *lvalue, cctree_t *rvalue);
ccfunc void
ccseer_rvalue(ccseer_t *seer, cctree_t *tree);
ccfunc void
ccseer_lvalue(ccseer_t *seer, cctree_t *tree);

ccfunc void
ccseer_call(ccseer_t *seer, cctree_t *tree)
{
  ccassert(tree->lval!=0);

  // Note: temporarly let this fail ...
  if(!ccseer_allude(seer,tree,tree->name))
      cctracewar("%s: identifier not found",tree->name);

  cctree_t *rval;
  ccarrfor(tree->rval,rval)
    ccseer_rvalue(seer,rval);
}

ccfunc void
ccseer_index(ccseer_t *seer, cctree_t *tree)
{
  ccassert(tree->lval);
  ccassert(tree->rval);

  if(tree->lval->kind==cctree_kLITIDE)
  {
    if(!ccseer_allude(seer,tree->lval,tree->lval->name))
        cctraceerr("%s: identifier not found",tree->lval->name);

  } else
  if(tree->lval->kind==cctree_kINDEX)
  {
    ccseer_index(seer,tree->lval);
  } else
    ccassert(!"error");

  ccseer_rvalue(seer,tree->rval);
}

ccfunc void
ccseer_lvalue(ccseer_t *seer, cctree_t *tree)
{
  switch(tree->kind)
  {
    case cctree_kLITIDE:
    {
      if(!ccseer_allude(seer,tree,tree->name))
        cctraceerr("'%s': undeclared lvalue identifier",tree->name);
    } break;
    case cctree_kINDEX:
    {
      ccseer_index(seer,tree);
    } break;
    default: ccassert(!"internal");
  }
}

ccfunc void
ccseer_rvalue(ccseer_t *seer, cctree_t *tree)
{
  switch(tree->kind)
  {
    case cctree_kLITINT:
    case cctree_kLITSTR:
    break;
    case cctree_kLITIDE:
    {
      if(!ccseer_allude(seer,tree,tree->name))
        cctraceerr("'%s': undeclared rvalue identifier",tree->name);

    } break;
    case cctree_kBINARY:
    {
      ccseer_binary(seer,tree->oper,tree->lval,tree->rval);
    } break;
    case cctree_kCALL:
    {
      ccseer_call(seer,tree);
    } break;
    case cctree_kINDEX:
    {
      ccseer_index(seer,tree);
    } break;
    case cctree_kUNARY:
    {
      if((tree->oper==cctoken_kADR) ||
         (tree->oper==cctoken_kDRF))
      {

        ccseer_rvalue(seer,tree->rval);
      } else
        ccassert(!"internal");
    } break;
    default: ccassert(!"internal");
  }
}

ccfunc void
ccseer_binary(ccseer_t *seer, cctoken_k oper, cctree_t *lvalue, cctree_t *rvalue)
{
  if(oper==cctoken_kASSIGN)
    ccseer_lvalue(seer,lvalue);
  else
    ccseer_rvalue(seer,lvalue);

  ccseer_rvalue(seer,rvalue);
}

ccfunc void
ccseer_decl_name(ccseer_t *seer, cctree_t *tree)
{
  // Note: is this a good way to do things?
  if(tree->type->kind==cctree_kFUNC)
  {
    if(tree->mark&cctree_mEXTERNAL)
    {
      if(ccseer_include_invokable(seer,tree,tree->name))
      {
        cctree_t **list;
        ccarrfor(tree->type->list,list)
          ccseer_decl_name(seer,*list);

        ccarrfor(tree->blob->list,list)
          ccseer_tree(seer,*list);

      } else
          cctraceerr("%s: already has a body", tree->name);
    } else
        cctraceerr("'%s': local function definitions are illegal", tree->name);
  } else
  {
    ccseer_include_variable(seer,tree,tree->name);

    if(tree->init)
    {
      ccseer_rvalue(seer,tree->init);
    }

    if(tree->type->kind==cctree_kARRAY)
    {
      cctree_t *i;
      for(i=tree->type; i->kind!=cctree_kTYPENAME; i=i->type)
      {
        if(i->kind==cctree_kARRAY)
          ccseer_rvalue(seer,i->rval);
        else
          ccassert(!"error");
      }
      ccassert(i->kind==cctree_kTYPENAME);

    } else
    if(tree->type->kind==cctree_kPOINTER)
    {
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
ccseer_tree(ccseer_t *seer, cctree_t *tree)
{
  if(tree->kind==cctree_kBLOCK)
  {
    cctree_t **list;
    ccarrfor(tree->list,list)
      ccseer_tree(seer,*list);
  } else
  if(tree->kind==cctree_kDECL)
  {
    if(tree->root->kind==cctree_kTUNIT)
      ccassert(tree->mark&cctree_mEXTERNAL);

    if(tree->mark&cctree_mEXTERNAL)
      ccassert(tree->root->kind==cctree_kTUNIT);

    // Todo: solve the base-type

    cctree_t **list;
    ccarrfor(tree->list,list)
      ccseer_decl_name(seer,*list);
  } else
  if(tree->kind==cctree_kCALL)
  {
    ccseer_call(seer,tree);
  } else
  if(tree->kind==cctree_kRETURN)
  {
    if(tree->rval)
    {
      ccseer_rvalue(seer,tree->rval);
    }
  } else
  if(tree->kind==cctree_kBINARY)
  {
    ccseer_binary(seer,tree->oper,tree->lval,tree->rval);
  } else
  if(tree->kind==cctree_kWHILE)
  {
    ccseer_rvalue(seer,tree->init);
    ccseer_tree(seer,tree->lval);
  } else
  if(tree->kind==cctree_kTERNARY)
  {
    ccseer_rvalue(seer,tree->init);

    if(tree->lval) ccseer_tree(seer,tree->lval);
    if(tree->rval) ccseer_tree(seer,tree->rval);
  } else
  {
    ccassert(!"error");
  }
}

ccfunc void
ccseer_uninit(ccseer_t *seer)
{
  if(seer->tdecl) ccdlbdel(seer->tdecl);
  if(seer->fdecl) ccdlbdel(seer->fdecl);
  if(seer->vdecl) ccdlbdel(seer->vdecl);
  if(seer->symbols) ccdlbdel(seer->symbols);
}

ccfunc void
ccseer_init(ccseer_t *seer)
{
  memset(seer,ccnil,sizeof(*seer));
}


ccfunc void
ccseer_translation_unit(ccseer_t *seer, cctree_t *tree)
{ ccassert(tree!=0);
  ccassert(tree->kind==cctree_kTUNIT);

  // Note: this is temporary and this should not be the way an user has to include functions ...
  cctree_t **decl;
  ccarrfor(tree->list,decl) ccseer_tree(seer,*decl);
}


#endif