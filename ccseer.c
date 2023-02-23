// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCSEER_C
#define _CCSEER_C

// Todo:
ccglobal cctype_t *t_stdc_int;

ccfunc void ccseer_tree(ccseer_t *seer, cctree_t *);

ccfunc ccinle ccesse_t *
ccseer_symbol(ccseer_t *seer, const char *name)
{
  ccesse_t **holder=cctblgetS(seer->entity_tale,name);
  ccesse_t *held=ccerrnon()? *holder: 0;

  return held;
}

ccfunc int
ccseer_include_entity(ccseer_t *seer, ccesse_t *esse, const char *name)
{
  ccesse_t **holder=cctblputS(seer->entity_tale,name);
  ccesse_t *held=*holder;

  if(ccerrnon())
    *holder=esse;
  else
  if(ccerrait())
    cctraceerr("'%s': redefinition, previous definition was '%s'", name,
      ((held->kind==ccesse_kVARIABLE)?"data variable":
      ((held->kind==ccesse_kFUNCTION)?"function":"unknown")));
  else
   ccassert(!"internal");

  return ccerrnon();
}

// Todo: legit scoping
ccfunc ccesse_t *
ccseer_allude(ccseer_t *seer, cctree_t *tree, const char *name)
{
  ccesse_t **holder=cctblgetS(seer->entity_tale,name);
  ccesse_t *held=*holder;

  if(ccerrnon())
  {
    ccesse_t **symbol=cctblputP(seer->symbol_tale,tree);
    ccassert(ccerrnon());

    *symbol=held;
  }

  return ccerrnon()? held :0;
}

// Todo: legit scoping
ccfunc ccesse_t *
ccseer_allusion(ccseer_t *seer, cctree_t *tree)
{
  ccassert(tree!=0);

  ccesse_t **symbol=cctblgetP(seer->symbol_tale,tree);

  return ccerrnon()? *symbol :ccnil;
}

ccfunc cctype_t *ccseer_binary(ccseer_t *seer,cctoken_k oper, cctree_t *lvalue, cctree_t *rvalue);
ccfunc cctype_t *ccseer_rvalue(ccseer_t *seer, cctree_t *tree);
ccfunc cctype_t *ccseer_lvalue(ccseer_t *seer, cctree_t *tree);

// Todo:
ccfunc ccesse_t *
ccesse_builtin(ccbuitin_k builtin)
{ ccesse_t *e=ccmalloc_T(ccesse_t);
  e->kind=ccesse_kFUNCTION;
  e->builtin=builtin;

  // Todo:
  e->type=ccmalloc_T(cctype_t);
  e->type->kind=cctype_kTYPENAME;
  return e;
}

// Todo:
ccfunc ccinle int
cctype_indirect(cctype_t *type)
{
  return (type->kind==cctype_kPOINTER)||(type->kind==cctype_kARRAY);
}

// Todo:
ccfunc ccinle cctype_t *
cctype_basetype(cctype_t *type)
{
  while(cctype_indirect(type)) type=type->type;

  return type;
}

// Todo: rework this
ccfunc cctype_t *
ccseer_call(ccseer_t *seer, cctree_t *tree)
{ ccassert(tree->lval!=0);

  cctype_t *result=ccnull;

  if(tree->lval->kind==cctree_kLITIDE)
  {
    ccesse_t *esse=ccseer_allude(seer,tree->lval,tree->lval->name);
    if(!esse)
      cctracewar("'%s': identifier not found",tree->lval->name);
    else
    if(esse->kind!=ccesse_kFUNCTION)
      cctracewar("'%s': not a function",tree->lval->name);

    // Note: the type is a function, the type of the type is the return type ...
    result=esse->type->type;
  } else
    ccassert(!"error");

  cctree_t *rval;
  ccarrfor(tree->rval,rval)
    ccseer_rvalue(seer,rval);

  return result;
}

ccfunc cctype_t *
ccseer_index(ccseer_t *seer, cctree_t *tree)
{ ccassert(tree->lval);
  ccassert(tree->rval);

  cctype_t *result=ccnull;

  if(tree->lval->kind==cctree_kLITIDE)
  { ccesse_t *esse=ccseer_allude(seer,tree->lval,tree->lval->name);
    if(!esse)
      cctraceerr("%s: identifier not found",tree->lval->name);
    result=esse->type;
  } else
  if(tree->lval->kind==cctree_kINDEX)
  { result=ccseer_index(seer,tree->lval);
  } else
    ccassert(!"error");

  if(!cctype_indirect(result))
    cctraceerr("%s: subscript requires array or pointer type",tree->lval->name);

  ccseer_rvalue(seer,tree->rval);

  return result->type;
}

ccfunc cctype_t *
ccseer_lvalue(ccseer_t *seer, cctree_t *tree)
{
  cctype_t *type=ccnull;
  switch(tree->kind)
  { case cctree_kLITIDE:
    { ccesse_t *esse=ccseer_allude(seer,tree,tree->name);
      if(!esse)
        cctraceerr("'%s': undeclared identifier",tree->name);
      else
      if(!esse->kind==ccesse_kVARIABLE)
        ccbreak();
      type=esse->type;
    } break;
    case cctree_kINDEX:
    { type=ccseer_index(seer,tree);
    } break;
    case cctree_kUNARY:
    { if((tree->sort==cctoken_kDEREFERENCE))
      { type=ccseer_lvalue(seer,tree->rval);

        // Todo
        char buf[256];
        cctype_to_string(type,buf);

        if(!cctype_indirect(type))
          cctraceerr("'%s': illegal indirection",buf);

        type=type->type;
      } else
        ccassert(!"internal");
    } break;
    default: ccassert(!"internal");
  }
  return type;
}

ccfunc cctype_t *
ccseer_rvalue(ccseer_t *seer, cctree_t *tree)
{ cctype_t *result=ccnull;

  switch(tree->kind)
  { case cctree_kLITINT:
      result=t_stdc_int;
    break;
    case cctree_kLITSTR:
    break;
    case cctree_kLITIDE:
    { ccesse_t *esse=ccseer_allude(seer,tree,tree->name);
      if(!esse)
        cctraceerr("'%s': undeclared identifier",tree->name);
      result=esse->type;
    } break;
    case cctree_kBINARY:
    {
      result=ccseer_binary(seer,tree->sort,tree->lval,tree->rval);
    } break;
    case cctree_kCALL:
    {
      result=ccseer_call(seer,tree);
    } break;
    case cctree_kINDEX:
    {
      result=ccseer_index(seer,tree);
    } break;
    // Todo:
    case cctree_kUNARY:
    { if((tree->sort==cctoken_kADDRESSOF))
      {
        result=ccseer_lvalue(seer,tree->rval);

        cctype_t *pointer=ccmalloc_T(cctype_t);
        pointer->kind=cctype_kPOINTER;
        pointer->sort=cctoken_kINVALID;
        pointer->type=result;

        result=pointer;
      } else
      if((tree->sort==cctoken_kDEREFERENCE))
      {
        result=ccseer_rvalue(seer,tree->rval);
        result=result->type;
      } else
        ccassert(!"internal");
    } break;
    default: ccassert(!"internal");
  }

  return result;
}

ccfunc cctype_t *
ccseer_binary(ccseer_t *seer, cctoken_k oper, cctree_t *lvalue, cctree_t *rvalue)
{
  cctype_t *result=ccnull;

  if(oper==cctoken_kASSIGN)
  {
    cctype_t *ltype,*rtype;

    ltype=ccseer_lvalue(seer,lvalue);
    rtype=ccseer_rvalue(seer,rvalue);
    ccassert(ltype!=0);
    ccassert(rtype!=0);

    result=ltype;

    // Todo:
    char lbuf[256];
    char rbuf[256];

    for(;;)
    {
      if(cctype_indirect(ltype)!=cctype_indirect(rtype))
      {
        cctype_to_string(ltype,lbuf);
        cctype_to_string(rtype,rbuf);

        cctraceerr("'=': '%s' differs in levels of indirection from '%s'",lbuf,rbuf);
      } else
      if((ltype->kind!=rtype->kind) ||
         (ltype->sort!=rtype->sort))
      {
        cctype_to_string(ltype,lbuf);
        cctype_to_string(rtype,rbuf);

        cctraceerr("'=': incompatible types - from '%s' to '%s'",lbuf,rbuf);
      }

      ccassert(!cctype_indirect(ltype)||ltype->type!=0);
      ccassert(!cctype_indirect(rtype)||rtype->type!=0);

      ltype=ltype->type;
      rtype=rtype->type;

      if(!ltype) break;
      if(!rtype) break;
    }

  } else
  { // Todo:
    result=ccseer_rvalue(seer,lvalue);
    ccseer_rvalue(seer,rvalue);
  }

  return result;
}

ccfunc cctype_t *
ccseer_tree_to_type(ccseer_t *seer, cctree_t *tree)
{
  // Note: we're expecting a modifier tree ...

  ccassert(tree!=0);

  cctype_t *type=ccmalloc_T(cctype_t);
  type->kind=cctype_kINVALID;
  type->name=tree->name;
  type->sort=cctoken_kINVALID;
  type->type=ccnull;
  type->list=ccnull;
  type->size=ccnull;

  if(tree->kind==cctree_kARRAY)
  {
    type->kind=cctype_kARRAY;
    type->type=ccseer_tree_to_type(seer,tree->type);

    // Todo:
    if(tree->rval->kind==cctree_kLITINT)
      type->size=tree->rval->as_i32;
    else
      ccassert(!"error");

    ccassert(type->size!=0);


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
  if(tree->kind==cctree_kPOINTER)
  { type->kind=cctype_kPOINTER;
    type->type=ccseer_tree_to_type(seer,tree->type);
  } else
  if(tree->kind==cctree_kFUNCTION)
  { type->kind=cctype_kFUNCTION;
    type->type=ccseer_tree_to_type(seer,tree->type);

    // Todo: re-work this ...
    cctree_t **list;
    ccarrfor(tree->list,list)
      *ccarradd(type->list,1)=*ccseer_tree_to_type(seer,(*list)->type);

  } else
  if(tree->kind==cctree_kTYPENAME)
  {
    type->kind=cctype_kTYPENAME;
    type->sort=tree->sort;
  } else
    ccassert(!"error");
  return type;
}

ccfunc void
ccseer_decl_name(ccseer_t *seer, cctree_t *tree)
{
  ccassert(tree!=0);
  ccassert(tree->kind==cctree_kDECLNAME);
  ccassert(tree->type!=0);
  ccassert(tree->name!=0);

  if(tree->type->kind==cctree_kFUNCTION)
  { if(tree->mark&cctree_mEXTERNAL)
    {
      ccesse_t *esse=ccseer_symbol(seer,tree->name);

      if(!esse)
      { esse=ccmalloc_T(ccesse_t);
        esse->kind=ccesse_kFUNCTION;
        esse->name=tree->name;
        esse->tree=tree;
        esse->type=ccseer_tree_to_type(seer,tree->type);

        ccseer_include_entity(seer,esse,tree->name);
      } else
      {
        // Todo: better debug info
        if(esse->tree->blob && tree->blob)
        {
          cctraceerr("%s: already has a body", tree->name);
        }
      }

      // Todo: actually create the type and check that it matches the previous one

      cctree_t **list;

      // Note: check all parameters
      ccarrfor(tree->type->list,list) ccseer_decl_name(seer,*list);

      // Note: check every statement
      ccarrfor(tree->blob->list,list) ccseer_tree(seer,*list);
    } else
        cctraceerr("'%s': local function definitions are illegal", tree->name);
  } else
  {
    // Todo: remove this ...
    ccassert(!(tree->mark&cctree_mEXTERNAL));

    ccesse_t *esse=ccmalloc_T(ccesse_t);
    esse->kind=ccesse_kVARIABLE;
    esse->name=tree->name;
    esse->tree=tree;
    esse->type=ccseer_tree_to_type(seer,tree->type);

    if(ccseer_include_entity(seer,esse,tree->name))
    {
      ccassert(
        ccseer_allude(seer,tree,tree->name));


      if(tree->init)
        ccseer_rvalue(seer,tree->init);
    }
  }
}

ccfunc void
ccseer_tree(ccseer_t *seer, cctree_t *tree)
{ switch(tree->kind)
  { cctree_t **list;

    case cctree_kBLOCK:
      ccarrfor(tree->list,list) ccseer_tree(seer,*list);
    break;
    case cctree_kDECL:
      if(tree->root->kind==cctree_kTUNIT)
        ccassert(tree->mark&cctree_mEXTERNAL);
      if(tree->mark&cctree_mEXTERNAL)
        ccassert(tree->root->kind==cctree_kTUNIT);

      // Todo: solve the base-type
      ccarrfor(tree->list,list) ccseer_decl_name(seer,*list);
    break;
    case cctree_kCALL:
      ccseer_call(seer,tree);
    break;
    case cctree_kRETURN:
      if(tree->rval)
        ccseer_rvalue(seer,tree->rval);
    break;
    case cctree_kBINARY:
      ccseer_binary(seer,tree->sort,tree->lval,tree->rval);
    break;
    case cctree_kWHILE:
      ccseer_rvalue(seer,tree->init);
      ccseer_tree(seer,tree->lval);
    break;
    case cctree_kTERNARY:
      ccseer_rvalue(seer,tree->init);
      if(tree->lval) ccseer_tree(seer,tree->lval);
      if(tree->rval) ccseer_tree(seer,tree->rval);
    break;
    default: ccassert(!"error");
  }
}

ccfunc void
ccseer_uninit(ccseer_t *seer)
{
  if(seer->entity_tale) ccdlbdel(seer->entity_tale);
  if(seer->symbol_tale) ccdlbdel(seer->symbol_tale);
}

ccfunc void
ccseer_init(ccseer_t *seer)
{
  memset(seer,0,sizeof(*seer));

  // TODO:
  ccseer_include_entity(seer,ccesse_builtin(ccbuiltin_kCCASSERT),"ccassert");
  ccseer_include_entity(seer,ccesse_builtin(ccbuiltin_kCCBREAK),"ccbreak");
  ccseer_include_entity(seer,ccesse_builtin(ccbuiltin_kCCERROR),"ccerror");

  t_stdc_int=ccmalloc_T(cctype_t);
  t_stdc_int->kind=cctype_kTYPENAME;
  t_stdc_int->sort=cctoken_kSTDC_INT;

}

ccfunc void
ccseer_translation_unit(ccseer_t *seer, cctree_t *tree)
{ ccassert(tree!=0);
  ccassert(tree->kind==cctree_kTUNIT);

  cctree_t **decl;
  ccarrfor(tree->list,decl) ccseer_tree(seer,*decl);
}
#endif