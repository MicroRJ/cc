// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCSEER_C
#define _CCSEER_C

// Todo:
ccglobal cctype_t
  *cctype_void,
  *cctype_stdc_int,
  *cctype_stdc_long,
  *cctype_stdc_short,
  *cctype_stdc_double,
  *cctype_stdc_float,
  *cctype_stdc_char,
  *cctype_stdc_bool,
  *cctype_stdc_signed,
  *cctype_stdc_unsigned,
  *cctype_msvc_int8,
  *cctype_msvc_int16,
  *cctype_msvc_int32,
  *cctype_msvc_int64,
  *cctype_stdc_char_ptr;

ccfunc void ccseer_tree(ccseer_t *seer, cctree_t *);
ccfunc cctype_t *ccseer_rvalue(ccseer_t *seer, cctree_t *tree);
ccfunc cctype_t *ccseer_lvalue(ccseer_t *seer, cctree_t *tree);
ccfunc cctype_t *ccseer_value(ccseer_t *seer, cctree_t *tree, cci32_t is_lval);


ccfunc ccinle ccesse_t *
ccseer_entity(ccseer_t *seer, const char *name)
{
  ccassert(name!=0);

  ccesse_t **holder=cctblgetS(seer->entity_table,name);
  ccesse_t *held=ccerrnon()? *holder :0;

  return held;
}

// Todo: legit scoping
ccfunc ccinle ccesse_t *
ccseer_symbol(ccseer_t *seer, cctree_t *tree)
{
  ccassert(tree!=0);

  ccesse_t **holder=cctblgetP(seer->symbol_table,tree);
  ccesse_t *held=ccerrnon()? *holder :0;

  return held;
}

ccfunc cctype_t *
ccseer_tree_type(ccseer_t *seer, cctree_t *tree)
{
  cctype_t **holder=cctblgetP(seer->tether_table,tree);
  cctype_t *held=ccerrnon()? *holder :0;

  ccassert(ccerrnon());

  return held;
}

ccfunc void
ccseer_tether(ccseer_t *seer, cctree_t *tree, cctype_t *type)
{
  cctype_t **holder=cctblputP(seer->tether_table,tree);
  ccassert(ccerrnon());

  *holder=type;
}

ccfunc int
ccseer_include_entity(ccseer_t *seer, ccesse_t *esse, const char *name)
{
  ccesse_t **holder=cctblputS(seer->entity_table,name);
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
{ ccesse_t **holder=cctblgetS(seer->entity_table,name);
  ccesse_t *held=*holder;

  if(ccerrnon())
  {
    ccesse_t **symbol=cctblputP(seer->symbol_table,tree);
    ccassert(ccerrnon());

    *symbol=held;
  }
  return ccerrnon()? held :0;
}

// Todo:
ccfunc ccesse_t *
ccesse_builtin(ccbuitin_k builtin)
{ ccesse_t *e=ccmalloc_T(ccesse_t);
  e->kind=ccesse_kFUNCTION;
  e->builtin=builtin;

  e->type=cctype_function_modifier(cctype_stdc_int,ccnull,ccfalse);
  return e;
}

// Todo:
ccfunc ccinle int
cctype_indirect(cctype_t *type)
{
  return (type->kind==cctype_kPOINTER)||(type->kind==cctype_kARRAY);
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

      if(cctoken_is_assignment(tree->sort))
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
        result=cctype_pointer_modifier(result);
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
      result=cctype_stdc_int;
    break;
    case cctree_kLITSTR:
      ccassert(!is_lval);
      result=cctype_stdc_char_ptr;
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

  if(tree->kind==cctree_kARRAY)
  {
    cci32_t   size=0;
    cctype_t *type;

    type=ccseer_tree_to_type(seer,tree->type);
    ccseer_rvalue(seer,tree->rval);

    // Todo:
    if(tree->rval->kind==cctree_kLITINT)
      size=tree->rval->as_i32;
    else
      ccassert(!"error");

    ccassert(size!=0);

    return cctype_array_modifier(type,size);
  } else
  if(tree->kind==cctree_kPOINTER)
  { cctype_t *type=ccseer_tree_to_type(seer,tree->type);

    return cctype_pointer_modifier(type);
  } else
  if(tree->kind==cctree_kFUNCTION)
  { cctype_t *type=ccseer_tree_to_type(seer,tree->type);
    cctype_t *list=ccnull;

    // Todo: re-work this ...
    cctree_t **i;
    ccarrfor(tree->list,i)
      *ccarradd(list,1)=*ccseer_tree_to_type(seer,(*i)->type);

    return cctype_function_modifier(type,list,ccfalse);
  } else
  if(tree->kind==cctree_kSPECIFIER)
  {
    switch(tree->sort)
    { case cctoken_kVOID:          return cctype_void;
      case cctoken_kSTDC_INT:      return cctype_stdc_int;
      case cctoken_kSTDC_LONG:     return cctype_stdc_long;
      case cctoken_kSTDC_SHORT:    return cctype_stdc_short;
      case cctoken_kSTDC_DOUBLE:   return cctype_stdc_double;
      case cctoken_kSTDC_FLOAT:    return cctype_stdc_float;
      case cctoken_kSTDC_CHAR:     return cctype_stdc_char;
      case cctoken_kSTDC_BOOL:     return cctype_stdc_bool;
      case cctoken_kSTDC_SIGNED:   return cctype_stdc_signed;
      case cctoken_kSTDC_UNSIGNED: return cctype_stdc_unsigned;
      case cctoken_kMSVC_INT8:     return cctype_msvc_int8;
      case cctoken_kMSVC_INT16:    return cctype_msvc_int16;
      case cctoken_kMSVC_INT32:    return cctype_msvc_int32;
      case cctoken_kMSVC_INT64:    return cctype_msvc_int64;
    }
  }

  ccassert(!"error");
  return ccnull;
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
      ccesse_t *esse=ccseer_entity(seer,tree->name);

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
      if(tree->blob)
      {
        ccarrfor(tree->blob->list,list) ccseer_tree(seer,*list);
      }
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
  if(seer->entity_table) ccdlbdel(seer->entity_table);
  if(seer->symbol_table) ccdlbdel(seer->symbol_table);
}

ccfunc void
ccseer_init(ccseer_t *seer)
{
  memset(seer,0,sizeof(*seer));

  cctype_void          =cctype_specifier(   0,cctoken_kVOID);
  cctype_stdc_int      =cctype_specifier(0x20,cctoken_kSTDC_INT);
  cctype_stdc_long     =cctype_specifier(0x20,cctoken_kSTDC_LONG);
  cctype_stdc_short    =cctype_specifier(0x10,cctoken_kSTDC_SHORT);
  cctype_stdc_double   =cctype_specifier(0x40,cctoken_kSTDC_DOUBLE);
  cctype_stdc_float    =cctype_specifier(0x20,cctoken_kSTDC_FLOAT);
  cctype_stdc_char     =cctype_specifier(0x08,cctoken_kSTDC_CHAR);
  cctype_stdc_bool     =cctype_specifier(0x08,cctoken_kSTDC_BOOL);
  cctype_stdc_signed   =cctype_specifier(0x20,cctoken_kSTDC_SIGNED);
  cctype_stdc_unsigned =cctype_specifier(0x20,cctoken_kSTDC_UNSIGNED);
  cctype_msvc_int8     =cctype_specifier(0x08,cctoken_kMSVC_INT8);
  cctype_msvc_int16    =cctype_specifier(0x10,cctoken_kMSVC_INT16);
  cctype_msvc_int32    =cctype_specifier(0x20,cctoken_kMSVC_INT32);
  cctype_msvc_int64    =cctype_specifier(0x40,cctoken_kMSVC_INT64);

  cctype_stdc_char_ptr =cctype_pointer_modifier(cctype_stdc_char);

  // Todo:!
  ccseer_include_entity(seer,ccesse_builtin(ccbuiltin_kCCASSERT), "ccassert");
  ccseer_include_entity(seer,ccesse_builtin(ccbuiltin_kCCBREAK), "ccbreak");
  ccseer_include_entity(seer,ccesse_builtin(ccbuiltin_kCCERROR), "ccerror");
  ccseer_include_entity(seer,ccesse_builtin(ccbuiltin_kCCPRINTF), "ccprintf");
}

ccfunc void
ccseer_translation_unit(ccseer_t *seer, cctree_t *tree)
{ ccassert(tree!=0);
  ccassert(tree->kind==cctree_kTUNIT);

  cctree_t **decl;
  ccarrfor(tree->list,decl) ccseer_tree(seer,*decl);
}
#endif