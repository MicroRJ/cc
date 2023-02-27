// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCSEER_C
#define _CCSEER_C

ccfunc int ccseer_register(ccseer_t *seer, ccesse_t *esse, const char *name);

// Note: not sure how to make this better ...
ccfunc void ccsee_register_builtin(ccseer_t *seer, ccbuiltin_k sort, const char *name);

ccfunc void       ccseer_tree(ccseer_t *seer, cctree_t *);
ccfunc cctype_t * ccseer_rvalue(ccseer_t *seer, cctree_t *tree);
ccfunc cctype_t * ccseer_lvalue(ccseer_t *seer, cctree_t *tree);
ccfunc cctype_t * ccseer_value(ccseer_t *seer, cctree_t *tree, cci32_t is_lval);

ccfunc void
ccseer_uninit(ccseer_t *seer)
{
  if(seer->entity_table) ccdlbdel(seer->entity_table);
  if(seer->symbol_table) ccdlbdel(seer->symbol_table);
}

ccfunc void
ccseer_init(ccseer_t *seer)
{
  memset(seer,ccnull,sizeof(*seer));

  // Todo: how to actually do this properly?
  // Todo: long long int
  // Todo: long int
  // Todo: short int
  // Todo: signed
  // Todo: unsigned
  seer->type_void          =cctype_specifier(0,cctoken_kVOID);
  seer->type_stdc_int      =cctype_specifier(4,cctoken_kSTDC_INT);
  seer->type_stdc_long     =cctype_specifier(4,cctoken_kSTDC_LONG);
  seer->type_stdc_short    =cctype_specifier(2,cctoken_kSTDC_SHORT);
  seer->type_stdc_double   =cctype_specifier(8,cctoken_kSTDC_DOUBLE);
  seer->type_stdc_float    =cctype_specifier(4,cctoken_kSTDC_FLOAT);
  seer->type_stdc_char     =cctype_specifier(1,cctoken_kSTDC_CHAR);
  seer->type_stdc_bool     =cctype_specifier(1,cctoken_kSTDC_BOOL);
  seer->type_stdc_signed   =cctype_specifier(4,cctoken_kSTDC_SIGNED);
  seer->type_stdc_unsigned =cctype_specifier(4,cctoken_kSTDC_UNSIGNED);
  seer->type_msvc_int8     =cctype_specifier(1,cctoken_kMSVC_INT8);
  seer->type_msvc_int16    =cctype_specifier(2,cctoken_kMSVC_INT16);
  seer->type_msvc_int32    =cctype_specifier(4,cctoken_kMSVC_INT32);
  seer->type_msvc_int64    =cctype_specifier(8,cctoken_kMSVC_INT64);
  seer->type_void_ptr      =cctype_pointer_modifier(seer->type_void);
  seer->type_stdc_char_ptr =cctype_pointer_modifier(seer->type_stdc_char);

  // Todo:!
  ccsee_register_builtin(seer,ccbuiltin_kCCBREAK,"ccbreak");
  ccsee_register_builtin(seer,ccbuiltin_kCCERROR,"ccerror");
}


// Todo:
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

// Todo: not very good!
ccfunc cctype_t *
ccseer_tree_to_type(ccseer_t *seer, cctree_t *tree)
{ ccassert(tree!=0);

  cctype_t *result=ccnull;

  if(tree->kind==cctree_kSTRUCT)
  {
    cci32_t   size=0;
    ccelem_t *list=ccnull;

    // Todo:
    cctree_t **v;
    ccarrfor(tree->list,v)
    { cctree_t *n=*v;
      ccassert(n->kind==cctree_kDECLNAME);
      ccassert(n->name!=0);
      ccassert(n->size==0); // Note: we don't support this now ...

      // Todo: error message...
      ccelem_t *e=cctblputS(list,n->name);
      ccassert(ccerrnon());

      e->tree=n;
      e->name=n->name;
      e->type=ccseer_tree_to_type(seer,n->type);
      e->slot=size;

      // Todo: this is terrible...
      size+=e->type->size;
    }

    result=cctype_record(tree,list,size,tree->name);

    if(tree->name)
    {
      ccesse_t *e=ccesse(ccesse_kTYPENAME);
      e->name=tree->name;
      e->tree=tree;
      e->type=result;
      ccseer_register(seer,e,tree->name);
    }
  } else
  if(tree->kind==cctree_kARRAY)
  {
    cctype_t *type=ccseer_tree_to_type(seer,tree->type);

    // Todo:
    ccassert(tree->rval->kind==cctree_kCONSTANT);
    ccassert(tree->rval->sort==cctoken_kLITINT);

    ccseer_rvalue(seer,tree->rval);

    // Todo:
    cci32_t length=cctree_casti32(tree->rval);
    ccassert(length!=0);

    result=cctype_array_modifier(type,length*type->size);
  } else
  if(tree->kind==cctree_kPOINTER)
  {
    result=cctype_pointer_modifier(ccseer_tree_to_type(seer,tree->type));
  } else
  if(tree->kind==cctree_kFUNCTION)
  { cctype_t *type=ccseer_tree_to_type(seer,tree->type);
    cctype_t *list=ccnull;

    // Todo: memory leak!
    // cctree_t **i;
    // ccarrfor(tree->list,i)
    //   *ccarradd(list,1)=*ccseer_tree_to_type(seer,(*i)->type);

    result=cctype_function_modifier(type,ccnull,ccfalse);
  } else
  if(tree->kind==cctree_kTYPENAME)
  {
    switch(tree->sort)
    { case cctoken_kVOID:          result=seer->type_void; break;
      case cctoken_kSTDC_INT:      result=seer->type_stdc_int; break;
      case cctoken_kSTDC_LONG:     result=seer->type_stdc_long; break;
      case cctoken_kSTDC_SHORT:    result=seer->type_stdc_short; break;
      case cctoken_kSTDC_DOUBLE:   result=seer->type_stdc_double; break;
      case cctoken_kSTDC_FLOAT:    result=seer->type_stdc_float; break;
      case cctoken_kSTDC_CHAR:     result=seer->type_stdc_char; break;
      case cctoken_kSTDC_BOOL:     result=seer->type_stdc_bool; break;
      case cctoken_kSTDC_SIGNED:   result=seer->type_stdc_signed; break;
      case cctoken_kSTDC_UNSIGNED: result=seer->type_stdc_unsigned; break;
      case cctoken_kMSVC_INT8:     result=seer->type_msvc_int8; break;
      case cctoken_kMSVC_INT16:    result=seer->type_msvc_int16; break;
      case cctoken_kMSVC_INT32:    result=seer->type_msvc_int32; break;
      case cctoken_kMSVC_INT64:    result=seer->type_msvc_int64; break;
    }
  } else
  if(tree->kind==cctree_kIDENTIFIER)
  {
    // Todo: error message ...
    ccesse_t *esse=ccseer_entity(seer,tree->name);
    ccassert(esse!=0);
    ccassert(esse->kind==ccesse_kTYPENAME);

    result=esse->type;
  }

  ccassert(result!=0);
  return result;
}


// Note: register an entity by name...
ccfunc int
ccseer_register(ccseer_t *seer, ccesse_t *esse, const char *name)
{
  ccesse_t **holder=cctblputS(seer->entity_table,name);
  ccesse_t *held=*holder;

  if(ccerrnon())
    *holder=esse;
  else
    cctraceerr("'%s': redefinition, previous definition was '%s'", name,
      ((held->kind==ccesse_kVARIABLE)?"data variable":
      ((held->kind==ccesse_kFUNCTION)?"function":"unknown")));

  ccassert(esse->kind!=0);
  ccassert(esse->name!=0);
  ccassert(esse->type!=0);

  ccassert(!strcmp(esse->name,name) ||
    ccdebuglog("'%s': mismatched names, specified %s", esse->name, name));

  return ccerrnon();
}

ccfunc void
ccsee_register_builtin(ccseer_t *seer, ccbuiltin_k sort, const char *name)
{
  ccesse_t *esse=ccesse(ccesse_kFUNCTION);
  esse->sort=sort;

  // Todo: proper modifier
  esse->type=cctype_function_modifier(seer->type_stdc_int,ccnull,ccfalse);
  esse->name=name;

  int i=ccseer_register(seer,esse,name);
  ccassert(i!=0);(void)i;
}

// Todo: legit scoping
ccfunc ccesse_t *
ccseer_associate(ccseer_t *seer, cctree_t *tree, const char *name)
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
ccfunc ccinle int
cctype_indirect(cctype_t *type)
{
  return (type->kind==cctype_kPOINTER)||(type->kind==cctype_kARRAY);
}

// Note: 'is_lval' meaning that it is specifically on the lhs of an assignment expression ...
ccfunc cctype_t *
ccseer_value(ccseer_t *seer, cctree_t *tree, cci32_t is_lval)
{ cctype_t *result=ccnull;

  switch(tree->kind)
  { case cctree_kIDENTIFIER:
    { ccesse_t *esse=ccseer_associate(seer,tree,tree->name);

      if(!esse)
        cctraceerr("'%s': undeclared identifier",tree->name);

      // Care:
      result=esse->type;
      ccseer_tether(seer,tree,result);
    } break;
    case cctree_kCONSTANT:
    { ccassert(!is_lval);

      // Note: I guess this is how we'd do implicit types?
      if(tree->sort==cctoken_kLITINT)
        result=seer->type_stdc_int;
      else
      if(tree->sort==cctoken_kLITSTR)
        result=seer->type_stdc_char_ptr;
      else
        ccassert(!"internal");

      ccseer_tether(seer,tree,result);
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
    case cctree_kSELECTOR:
    { ccassert(tree->lval!=0);
      ccassert(tree->rval!=0);

      ccassert((tree->lval->kind==cctree_kIDENTIFIER)||(tree->lval->kind==cctree_kSELECTOR));
      ccassert((tree->rval->kind==cctree_kIDENTIFIER));

      cctype_t *type=ccseer_value(seer,tree->lval,0);

      if(type)
      { if(type->kind==cctype_kRECORD)
        {
          ccelem_t *elem=cctblgetS(type->list,tree->rval->name);
          if(ccerrnon())
          {
            result=elem->type;
            ccseer_tether(seer,tree,result);
          } else
            cctraceerr("'%s': is not a member of '%s'", tree->rval->name,type->name?type->name:"unknown");
        } else
          cctraceerr("'.%s': must have struct or union specifier", tree->rval->name);
      }
    } break;
    case cctree_kSIZEOF:
    { // Todo:
      ccassert(!is_lval);

      ccassert(tree->lval!=0);
      result=ccseer_rvalue(seer,tree->lval);

      // Todo: make this be size_t or something similar
      result=seer->type_stdc_long;
      ccseer_tether(seer,tree,result);
    } break;
    case cctree_kADDRESSOF:
    { // Todo:
      ccassert(!is_lval);

      ccassert(tree->lval!=0);

      result=ccseer_value(seer,tree->lval,ccfalse);

      // Care:
      result=cctype_pointer_modifier(result);
      ccseer_tether(seer,tree,result);
    } break;
    case cctree_kDEREFERENCE:
    { ccassert(tree->lval!=0);

      result=ccseer_value(seer,tree->lval,ccfalse);
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
    } break;
    case cctree_kCALL:
    { ccassert(tree->lval!=0);

      result=ccseer_value(seer,tree->lval,ccfalse);

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
        cctype_to_string(ltype,lbuf);
        cctype_to_string(rtype,rbuf);

        for(;;)
        {
          if(cctype_indirect(ltype)!=cctype_indirect(rtype))
          {
            cctraceerr("'=': '%s' differs in levels of indirection from '%s'",lbuf,rbuf);
          } else
          if((ltype->kind!=rtype->kind) ||
             (ltype->sort!=rtype->sort))
          {
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

ccfunc void
ccseer_decl_name(ccseer_t *seer, cctree_t *tree)
{
  ccassert(tree!=0);
  ccassert(tree->kind==cctree_kDECLNAME);
  ccassert(tree->type!=0);
  ccassert(tree->name!=0);

  // Todo: is this proper enough?
  if(tree->type->kind==cctree_kFUNCTION)
  { if(tree->mark&cctree_mEXTERNAL)
    { ccesse_t *esse=ccseer_entity(seer,tree->name);
      if(!esse)
      { esse=ccesse(ccesse_kFUNCTION);
        esse->name=tree->name;
        esse->tree=tree;
        esse->type=ccseer_tree_to_type(seer,tree->type);

        // Todo: ensure return type is not a function or an array!
        ccseer_register(seer,esse,tree->name);
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

    ccesse_t *esse=ccesse(ccesse_kVARIABLE);
    esse->name=tree->name;
    esse->tree=tree;
    esse->type=ccseer_tree_to_type(seer,tree->type);

    if(ccseer_register(seer,esse,tree->name))
    {
      ccseer_associate(seer,tree,tree->name);

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
    case cctree_kCONDITIONAL:
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
ccseer_translation_unit(ccseer_t *seer, cctree_t *tree)
{ ccassert(tree!=0);
  ccassert(tree->kind==cctree_kTUNIT);

  cctree_t **decl;
  ccarrfor(tree->list,decl) ccseer_tree(seer,*decl);
}
#endif