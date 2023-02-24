// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCSEER_C
#define _CCSEER_C

// Todo:
ccglobal cctype_t *t_stdc_int;
ccglobal cctype_t *t_stdc_char;
ccglobal cctype_t *t_stdc_char_ptr;

ccfunc void ccseer_tree(ccseer_t *seer, cctree_t *);
ccfunc cctype_t *ccseer_rvalue(ccseer_t *seer, cctree_t *tree);
ccfunc cctype_t *ccseer_lvalue(ccseer_t *seer, cctree_t *tree);
ccfunc cctype_t *ccseer_value(ccseer_t *seer, cctree_t *tree, cci32_t is_lval);


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
{ ccesse_t **holder=cctblgetS(seer->entity_tale,name);
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

ccfunc void
ccseer_tether(ccseer_t *seer, cctree_t *tree, cctype_t *type)
{
  cctype_t **holder=cctblputP(seer->tether_tale,tree);
  ccassert(ccerrnon());

  *holder=type;
}

ccfunc cctype_t *
ccseer_tree_type(ccseer_t *seer, cctree_t *tree)
{
  cctype_t **holder=cctblgetP(seer->tether_tale,tree);
  ccassert(ccerrnon());

  cctype_t *result=*holder;
  return result;
}



// Todo:
ccfunc ccesse_t *
ccesse_builtin(ccbuitin_k builtin)
{ ccesse_t *e=ccmalloc_T(ccesse_t);
  e->kind=ccesse_kFUNCTION;
  e->builtin=builtin;

  e->type=ccseer_create_function_type(t_stdc_int);
  return e;
}

// Todo:
ccfunc ccinle int
cctype_indirect(cctype_t *type)
{
  return (type->kind==cctype_kPOINTER)||(type->kind==cctype_kARRAY);
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

    ccseer_tether(seer,tree->lval,result);
  } else
  if(tree->lval->kind==cctree_kINDEX)
  { result=ccseer_index(seer,tree->lval);
  } else
    ccassert(!"error");

  if(!cctype_indirect(result))
    cctraceerr("%s: subscript requires array or pointer type",tree->lval->name);

  ccseer_rvalue(seer,tree->rval);

  if(result)
  {
    result=result->type;
    ccseer_tether(seer,tree,result);
  }

  return result;
}

ccfunc cctype_t *
ccseer_value(ccseer_t *seer, cctree_t *tree, cci32_t is_lval)
{ cctype_t *result=ccnull;

  switch(tree->kind)
  { case cctree_kLITIDE:
    {
      ccesse_t *esse=ccseer_allude(seer,tree,tree->name);
      if(!esse)
        cctraceerr("'%s': undeclared identifier",tree->name);

      // Care:
      result=esse->type;
      ccseer_tether(seer,tree,result);
    } break;
    case cctree_kBINARY:
    { ccassert(tree->lval!=0);
      ccassert(tree->rval!=0);

      if(tree->sort==cctoken_kASSIGN)
      { cctype_t *ltype,*rtype;
        ltype=ccseer_value(seer,tree->lval,1);
        rtype=ccseer_value(seer,tree->rval,0);

        ccassert(ltype!=0);
        ccassert(rtype!=0);

        // Care:
        result=ltype;
        ccseer_tether(seer,tree,result);

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
      {
        // Todo: figure out what the proper conversion would be here!
        result=
        ccseer_value(seer,tree->lval,ccfalse);
        ccseer_value(seer,tree->rval,ccfalse);

        ccseer_tether(seer,tree,result);
      }
    } break;
    case cctree_kCALL:
    { ccassert(tree->lval!=0);

      result=ccseer_value(seer,tree->lval,0);

      if(result->kind==cctype_kFUNCTION)
      {
        cctree_t *rval;
        ccarrfor(tree->rval,rval)
          ccseer_rvalue(seer,rval);

        // Care:
        result=result->type;
        ccseer_tether(seer,tree,result);
      } else
        cctracewar("not a function",0);

    } break;
    case cctree_kINDEX:
    { ccassert(tree->lval!=0);
      ccassert(tree->rval!=0);

      result=ccseer_value(seer,tree->lval,0);

      if(!cctype_indirect(result))
      {

        // Todo: better logging
        char buf[256];
        cctype_to_string(result,buf);

        cctraceerr("'%s': subscript requires array or pointer type",buf);
      }

      ccseer_value(seer,tree->rval,0);

      // Care:
      result=result->type;
      ccseer_tether(seer,tree,result);
    } break;
    case cctree_kUNARY:
    { if(tree->sort==cctoken_kADDRESSOF)
      {
        // Todo:
        ccassert(!is_lval);

        result=ccseer_value(seer,tree->rval,cctrue);

        // Care:
        result=ccseer_create_pointer_type(result);
        ccseer_tether(seer,tree,result);
      } else
      if(tree->sort==cctoken_kDEREFERENCE)
      { result=ccseer_value(seer,tree->rval,ccfalse);

        if(!cctype_indirect(result))
        {
          // Todo:
          char buf[256];
          cctype_to_string(result,buf);

          cctraceerr("'%s': illegal indirection",buf);
        }

        // Care:
        result=result->type;
        ccseer_tether(seer,tree,result);
      } else
        ccassert(!"internal");
    } break;
    case cctree_kLITINT:
      ccassert(!is_lval);
      result=t_stdc_int;
    break;
    case cctree_kLITSTR:
      ccassert(!is_lval);
      result=t_stdc_char_ptr;
    break;

    default: ccassert(!"internal");
  }

  return result;
}

ccfunc ccinle cctype_t *
ccseer_lvalue(ccseer_t *seer, cctree_t *tree)
{
  return ccseer_value(seer,tree,cctrue);
}

ccfunc ccinle cctype_t *
ccseer_rvalue(ccseer_t *seer, cctree_t *tree)
{
  return ccseer_value(seer,tree,ccfalse);
}

// Todo: not very good!
ccfunc cctype_t *
ccseer_tree_to_type(ccseer_t *seer, cctree_t *tree)
{
  ccassert(tree!=0);

  cctype_t *type=ccmalloc_T(cctype_t);
  type->kind=cctype_kINVALID;
  type->sort=cctoken_kINVALID;
  type->type=ccnull;
  type->list=ccnull;
  type->size=ccnull;

  if(tree->kind==cctree_kARRAY)
  { type->kind=cctype_kARRAY;
    type->sort=cctoken_kINVALID;

    // Todo:
    if(tree->rval->kind==cctree_kLITINT)
      type->size=tree->rval->as_i32;
    else
      ccassert(!"error");

    ccassert(type->size!=0);

    type->type=ccseer_tree_to_type(seer,tree->type);

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
    type->sort=cctoken_kINVALID;
    type->size=sizeof(char*)*8;

    type->type=ccseer_tree_to_type(seer,tree->type);
  } else
  if(tree->kind==cctree_kFUNCTION)
  { type->kind=cctype_kFUNCTION;
    type->sort=cctoken_kINVALID;
    type->size=0;

    type->type=ccseer_tree_to_type(seer,tree->type);

    // Todo: re-work this ...
    cctree_t **list;
    ccarrfor(tree->list,list)
      *ccarradd(type->list,1)=*ccseer_tree_to_type(seer,(*list)->type);

  } else
  if(tree->kind==cctree_kTYPENAME)
  { type->kind=cctype_kTYPENAME;
    type->sort=tree->sort;

    switch(tree->sort)
    { case cctoken_kVOID:          type->size=   0; break;
      case cctoken_kSTDC_INT:      type->size=0x20; break;
      case cctoken_kSTDC_LONG:     type->size=0x20; break;
      case cctoken_kSTDC_SHORT:    type->size=0x10; break;
      case cctoken_kSTDC_DOUBLE:   type->size=0x40; break;
      case cctoken_kSTDC_FLOAT:    type->size=0x20; break;
      case cctoken_kSTDC_CHAR:     type->size=0x08; break;
      case cctoken_kSTDC_BOOL:     type->size=0x08; break;
      case cctoken_kSTDC_SIGNED:   type->size=0x20; break;
      case cctoken_kSTDC_UNSIGNED: type->size=0x20; break;
      case cctoken_kMSVC_INT8:     type->size=0x08; break;
      case cctoken_kMSVC_INT16:    type->size=0x10; break;
      case cctoken_kMSVC_INT32:    type->size=0x20; break;
      case cctoken_kMSVC_INT64:    type->size=0x40; break;
      default: ccassert(!"error");
    }

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
    case cctree_kRETURN:
      if(tree->rval)
        ccseer_rvalue(seer,tree->rval);
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
    default:
      ccseer_rvalue(seer,tree);
    break;
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

  // Todo:!
  ccseer_include_entity(seer,ccesse_builtin(ccbuiltin_kCCASSERT), "ccassert");
  ccseer_include_entity(seer,ccesse_builtin(ccbuiltin_kCCBREAK), "ccbreak");
  ccseer_include_entity(seer,ccesse_builtin(ccbuiltin_kCCERROR), "ccerror");
  ccseer_include_entity(seer,ccesse_builtin(ccbuiltin_kCCPRINTF), "ccprintf");

  // Todo:!
  t_stdc_int=ccmalloc_T(cctype_t);
  t_stdc_int->kind=cctype_kTYPENAME;
  t_stdc_int->sort=cctoken_kSTDC_INT;
  t_stdc_int->size=32;

  // Todo:!
  t_stdc_char=ccmalloc_T(cctype_t);
  t_stdc_char->kind=cctype_kTYPENAME;
  t_stdc_char->sort=cctoken_kSTDC_CHAR;
  t_stdc_int->size=8;

  t_stdc_char_ptr=ccseer_create_pointer_type(t_stdc_char);
}

ccfunc void
ccseer_translation_unit(ccseer_t *seer, cctree_t *tree)
{ ccassert(tree!=0);
  ccassert(tree->kind==cctree_kTUNIT);

  cctree_t **decl;
  ccarrfor(tree->list,decl) ccseer_tree(seer,*decl);
}
#endif