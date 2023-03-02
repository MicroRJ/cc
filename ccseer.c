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

  ccesse_t **e=cctblgetP(seer->symbol_table,tree);

  ccassert(ccerrnon() ||
   cctraceerr("'%s': symbol not found", cctree_string(tree,ccnull)));

  return *e;
}

ccfunc cctype_t *
ccseer_tree_type(ccseer_t *seer, cctree_t *tree)
{
  cctype_t **holder=cctblgetP(seer->tether_table,tree);
  cctype_t *held=ccerrnon()? *holder :0;

  ccassert(ccerrnon());

  return held;
}

// Todo: rename this!
ccfunc void
ccseer_tether(ccseer_t *seer, cctree_t *tree, cctype_t *type)
{
  cctype_t **holder=cctblputP(seer->tether_table,tree);
  ccassert(ccerrnon());

  *holder=type;
}

// Todo: rename this!
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

// Note: register an entity by name...
// Todo: legit scoping...
ccfunc int
ccseer_register(ccseer_t *seer, ccesse_t *esse, const char *name)
{
  ccesse_t **holder=cctblputS(seer->entity_table,name);
  ccesse_t *held=*holder;

  if(ccerrnon())
    *holder=esse;
  else
    cctraceerr("'%s': redefinition, previous definition was '%s'",name,
      ccesse_kind_string(held));

  ccassert(esse->kind!=0);
  ccassert(esse->name!=0);
  ccassert(esse->type!=0);

  ccassert(!strcmp(esse->name,name) ||
    ccdebuglog("'%s': mismatched names, specified %s", esse->name, name));

  return ccerrnon();
}

// Todo: this is terrible!
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

// Todo: not very good!
ccfunc cctype_t *
ccseer_typespec(ccseer_t *seer, cctree_t *tree)
{ ccassert(tree!=0);

  cctype_t *result=ccnull;
  switch(tree->kind)
  { case cctree_kIDENTIFIER:
    {
      ccesse_t *esse=ccseer_entity(seer,tree->name);

      if(!esse)
        cctraceerr("'%s': undeclared identifier",tree->name);

      if(esse->kind!=ccesse_kTYPENAME)
       cctraceerr("'%s': declaration expected type specifier, you specified %s",
        ccesse_kind_string(esse));

      ccassert(esse!=0);
      ccassert(esse->kind==ccesse_kTYPENAME);

      result=esse->type;
    } break;
    case cctree_kSTRUCT:
    {
      cci32_t size=0;
      ccelem_t *list=ccnull;

      // Todo: this is terrible...
      cctree_t **v;
      ccarrfor(tree->list,v)
      { cctree_t *n=*v;
        ccassert(n->kind==cctree_kDECL);
        ccassert(n->type!=0);
        ccassert(n->name!=0);
        ccassert(n->init==0); // Note: we don't support this now ...
        ccassert(n->size==0); // Note: we don't support this now ...

        // Todo: error message...
        ccelem_t *e=cctblputS(list,n->name);
        ccassert(ccerrnon());

        cctype_t *t=ccseer_typespec(seer,n->type);

        e->tree=n;
        e->type=t;
        e->name=n->name;
        e->slot=size;

        size+=e->type->size;
      }

      result=cctype_record(tree,list,size,tree->name);

      // Todo: we shouldn't do this here...
      if(tree->name)
      {
        ccesse_t *e=ccesse(ccesse_kTYPENAME);
        e->name=tree->name;
        e->tree=tree;
        e->type=result;
        ccseer_register(seer,e,tree->name);
      }
    } break;
    case cctree_kARRAY:
    { cctype_t *type=ccseer_typespec(seer,tree->type);

      // Todo:
      ccassert(tree->rval->kind==cctree_kCONSTANT);
      ccassert(tree->rval->sort==cctoken_kLITINT);

      ccseer_rvalue(seer,tree->rval);

      // Todo:
      cci32_t length=cctree_casti32(tree->rval);
      ccassert(length!=0);

      result=cctype_array_modifier(type,length*type->size);
    } break;
    case cctree_kPOINTER:
    {
      result=cctype_pointer_modifier(ccseer_typespec(seer,tree->type));
    } break;
    case cctree_kFUNCTION:
    {
      cctype_t  *r=ccseer_typespec(seer,tree->type);
      ccelem_t  *l=ccnull;
      cctree_t **i;

      int c=0;

      ccarrfor(tree->list,i)
      { cctree_t *p=*i;

        ccassert(!p->next);
        ccassert(!p->size);
        ccassert(!p->init);
        ccassert(p->type!=0);

        cctype_t *type=ccseer_typespec(seer,p->type);
        ccassert(type!=0);

        ccelem_t *e=cctblputS(l,p->name);
        ccassert(ccerrnon());
        e->tree=p;
        e->name=p->name;
        e->type=type;
        e->slot=c++;
      }

      result=cctype_function_modifier(r,l,ccfalse);
    } break;
    case cctree_kTYPENAME:
    { switch(tree->sort)
      { case cctoken_kVOID:          result=seer->type_void;         break;
        case cctoken_kSTDC_INT:      result=seer->type_stdc_int;     break;
        case cctoken_kSTDC_LONG:     result=seer->type_stdc_long;    break;
        case cctoken_kSTDC_SHORT:    result=seer->type_stdc_short;   break;
        case cctoken_kSTDC_DOUBLE:   result=seer->type_stdc_double;  break;
        case cctoken_kSTDC_FLOAT:    result=seer->type_stdc_float;   break;
        case cctoken_kSTDC_CHAR:     result=seer->type_stdc_char;    break;
        case cctoken_kSTDC_BOOL:     result=seer->type_stdc_bool;    break;
        case cctoken_kSTDC_SIGNED:   result=seer->type_stdc_signed;  break;
        case cctoken_kSTDC_UNSIGNED: result=seer->type_stdc_unsigned;break;
        case cctoken_kMSVC_INT8:     result=seer->type_msvc_int8;    break;
        case cctoken_kMSVC_INT16:    result=seer->type_msvc_int16;   break;
        case cctoken_kMSVC_INT32:    result=seer->type_msvc_int32;   break;
        case cctoken_kMSVC_INT64:    result=seer->type_msvc_int64;   break;
      }
    } break;
  }

  ccassert(result!=0);
  return result;
}


// Todo:
ccfunc cctype_t *
ccseer_typeof(ccseer_t *seer, cctree_t *tree)
{ cctype_t *result=ccnull;
  switch(tree->kind)
  { case cctree_kTYPENAME:
    case cctree_kSTRUCT:
    case cctree_kENUM:
    case cctree_kARRAY:
    case cctree_kPOINTER:
    {
      result=ccseer_typespec(seer,tree);
    } break;
    case cctree_kIDENTIFIER:
    { ccesse_t *esse=ccseer_entity(seer,tree->name);

      if(!esse)
        cctraceerr("'%s': undeclared identifier",tree->name);

      if((esse->kind==ccesse_kVARIABLE) ||
         (esse->kind==ccesse_kTYPENAME))
      {
        result=esse->type;
      } else
       cctraceerr("invalid entity for sizeof, expected type-name or variable-name",0);
    } break;
    default:
    {
      result=ccseer_value(seer,tree,ccfalse);
    } break;
  }
  return result;
}

// Todo: flag to know whether this is a parameter or not ...
ccfunc cctype_t *
ccseer_typecheck(ccseer_t *seer, cctype_t *ltype, cctype_t *rtype)
{
  cctype_t *result=ltype;

  cctype_t *_ltype=ltype;
  cctype_t *_rtype=rtype;

  for(;;)
  { if(cctype_indirect(ltype)!=cctype_indirect(rtype))
    {
      cctraceerr("'=': '%s' differs in levels of indirection from '%s'",
        cctype_string(_ltype,ccnull),cctype_string(_rtype,ccnull));
    } else
    if((ltype->kind!=rtype->kind) ||
       (ltype->sort!=rtype->sort))
    {
      cctraceerr("'=': incompatible types - from '%s' to '%s'",
        cctype_string(_ltype,ccnull),cctype_string(_rtype,ccnull));
    } else
    if((ltype->kind==cctype_kFUNCTION) &&
       (rtype->kind==cctype_kFUNCTION))
    {

      int l=ccarrlen(ltype->list);
      int r=ccarrlen(rtype->list);

      if(l != r)
      {
        cctraceerr("number of parameters is different than declaration",0);
      }

      int m=l<r?l:r;

      for(int i=0; i<m; ++i)
      {
        ccseer_typecheck(seer,
          (ltype->list+i)->type,
          (rtype->list+i)->type);
      }
    }

    ccassert(!cctype_indirect(ltype)||ltype->type!=0);
    ccassert(!cctype_indirect(rtype)||rtype->type!=0);
    ltype=ltype->type;
    rtype=rtype->type;
    if(!ltype) break;
    if(!rtype) break;
  }

  return result;
}

// Note: 'is_lval' meaning that it is specifically on the lhs of an assignment expression ...
ccfunc cctype_t *
ccseer_value(ccseer_t *seer, cctree_t *tree, cci32_t is_lval)
{ cctype_t *result=ccnull;

  switch(tree->kind)
  { case cctree_kIDENTIFIER:
    { ccesse_t *esse=ccseer_associate(seer,tree,tree->name);

      if(esse)
      { result=esse->type;
        ccseer_tether(seer,tree,result);
      } else
        cctraceerr("'%s': undeclared identifier",tree->name);

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
      { cctraceerr("'%s': subscript requires array or pointer type",
          cctype_string(result,ccnull));
      }

      ccseer_value(seer,tree->rval,0);

      // Care:
      result=result->type;
      ccseer_tether(seer,tree,result);
    } break;
    case cctree_kSELECTOR:
    { ccassert(tree->lval!=0);
      ccassert(tree->rval!=0);

      cctype_t *type=ccseer_value(seer,tree->lval,0);

      // Todo: how do we do this properly, should ccseer_value return an entity instead?
      ccesse_t *esse=ccseer_symbol(seer,tree->lval);

      ccassert(esse->type==type);

      if(ccerrnon())
      { if(esse->type->kind==cctype_kRECORD)
        { ccesse_t **e=cctblgetS(esse->list,tree->rval->name);
          if(ccerrnon())
          {
            result=(*e)->type;
            ccseer_tether(seer,tree,result);

            ccesse_t **k=cctblputP(seer->symbol_table,tree);
            ccassert(ccerrnon());

            *k=(*e);
          } else
            cctraceerr("'%s': is not a member of '%s'",
              tree->rval->name,esse->name?esse->name:"unknown");
        } else
          cctraceerr("'.%s': must have struct or union specifier", tree->rval->name);
      }
    } break;
    case cctree_kSIZEOF:
    { // Todo:
      ccassert(!is_lval);

      ccassert(tree->lval!=0);

      // Note: there's a difference between sizeof() and sizeof.
      // Sizeof() works for everything but sizeof is only for expressions, this is what makes the language unambiguous...
      // We assume the parser has done its job...

      result=ccseer_typeof(seer,tree->lval);
      ccseer_tether(seer,tree->lval,result);

      // Todo:
      result=seer->type_stdc_int;
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
        cctraceerr("'%s': illegal indirection",
          cctype_string(result,ccnull));
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

      cctype_t *ltype,*rtype;
      if(cctoken_is_assignment(tree->sort))
      { ltype=ccseer_value(seer,tree->lval,1);
        rtype=ccseer_value(seer,tree->rval,0);
        ccassert(ltype!=0);
        ccassert(rtype!=0);
        // Todo:
        result=ccseer_typecheck(seer,ltype,rtype);
        ccseer_tether(seer,tree,result);
      } else
      { // Todo:
        ltype=ccseer_value(seer,tree->lval,ccfalse);
        rtype=ccseer_value(seer,tree->rval,ccfalse);
        ccassert(ltype!=0);
        ccassert(rtype!=0);
        result=ltype; // ccseer_typecheck(seer,ltype,rtype);
        ccseer_tether(seer,tree,result);
      }
    } break;


    default: ccassert(!"internal");
  }

  return result;
}

ccfunc ccinle cctype_t *
ccseer_rvalue(ccseer_t *seer, cctree_t *tree)
{
  return ccseer_value(seer,tree,ccfalse);
}

ccfunc ccesse_t *
ccseer_implicit_decl(ccseer_t *seer, ccesse_t *root, ccelem_t *elem)
{
  // Note: creates implicit entities, these are never registered under any lookup scope nor associated
  // with any tree yet...
  // Note: the selector tree will trigger a look-up by name under the parent struct-typed
  // variable entity and associate the selector tree with the implicit entity...

  ccesse_t *result=ccesse(ccesse_kVARIABLE);
  result->root=root;
  result->name=elem->name;
  result->tree=elem->tree;
  result->type=elem->type;

  if(elem->type->kind==cctype_kRECORD)
  {
    ccelem_t *e;
    ccarrfor(elem->type->list,e)
    {
      ccesse_t **i=cctblputS(result->list,e->name);
      ccassert(ccerrnon());

      ccesse_t *v=ccseer_implicit_decl(seer,result,e);
      ccassert(v!=0);

      *i=v;
    }
  }

  return result;
}

ccfunc ccesse_t *
ccseer_decl(ccseer_t *seer, cctree_t *tree)
{
  cctype_t *type=ccseer_typespec(seer,tree->type);

  ccesse_t *result=ccesse(ccesse_kVARIABLE);
  result->name=tree->name;
  result->tree=tree;
  result->type=type;

  if(type->kind==cctype_kRECORD)
  {
    ccelem_t *e;
    ccarrfor(type->list,e)
    {
      ccesse_t **i=cctblputS(result->list,e->name);
      ccassert(ccerrnon());

      *i=ccseer_implicit_decl(seer,result,e);
    }
  }

  if(ccseer_register(seer,result,tree->name))
  {
    ccseer_associate(seer,tree,tree->name);
    ccseer_tether(seer,tree,result->type);

    if(tree->init)
    {
      cctype_t *value=ccseer_rvalue(seer,tree->init);

      ccseer_typecheck(seer,result->type,value);
    }
  }

  return result;
}

ccfunc void
ccseer_tree(ccseer_t *seer, cctree_t *tree)
{
  switch(tree->kind)
  { case cctree_kBLOCK:
    { cctree_t **list;
      ccarrfor(tree->list,list) ccseer_tree(seer,*list);
    } break;
    case cctree_kRETURN:
      if(tree->rval)
        ccseer_rvalue(seer,tree->rval);
    break;
    case cctree_kITERATOR:
      if(tree->init) ccseer_tree(seer,tree->init);
      if(tree->lval) ccseer_value(seer,tree->lval,ccfalse);
      if(tree->rval) ccseer_value(seer,tree->rval,ccfalse);
      if(tree->blob) ccseer_tree(seer,tree->blob);
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
    case cctree_kDECL:
    {
      cctree_t *next;
      for(next=tree;next;next=next->next)
      {
        ccassert(next->kind==cctree_kDECL);
        ccassert(next->type!=0);
        ccassert(next->name!=0);

        if(next->type->kind!=cctree_kFUNCTION)
        {
          ccseer_decl(seer,next);
        } else
        // Todo: is this proper enough?
        if(next->mark&cctree_mEXTERNAL)
        {
          ccesse_t *esse=ccseer_entity(seer,next->name);

          if(!esse)
          {
            esse=ccesse(ccesse_kFUNCTION);
            esse->tree=next;
            esse->name=next->name;
            esse->type=ccseer_typespec(seer,tree->type);
            // Todo: ensure return type is not a function or an array!

            ccseer_register(seer,esse,next->name);
          } else
          {
            // Todo: leak!
            cctype_t *type=ccseer_typespec(seer,tree->type);

            ccseer_typecheck(seer,esse->type,type);

            // Todo: better debug info
            if(esse->tree->blob && tree->blob)
            {
              cctraceerr("%s: already has a body", next->name);
            }
          }

          // Todo: do we need to do this at definition time necessarily? I guess
          // type-checking would include the name of the parameter too for named
          // arguments... but by that time we won't need nor support pre-declarations...
          if(next->blob)
          {
            // Todo:
            esse->tree=next;

            cctree_t **i;
            ccarrfor(next->type->list,i)
            {
              ccesse_t *e=ccseer_decl(seer,*i);
              ccassert(e!=0);

              *ccarradd(esse->list,1)=e;
            }

            ccarrfor(next->blob->list,i)
              ccseer_tree(seer,*i);
          }
        } else
          cctraceerr("'%s': local function definitions are illegal", next->name);

      }
    } break;

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