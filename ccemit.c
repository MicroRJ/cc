// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCEMIT
#define _CCEMIT

ccfunc void
ccemit_uninit(ccemit_t *emit)
{
  ccdlbdel(emit->globals);
}

ccfunc void
ccemit_init(ccemit_t *emit)
{
  memset(emit,ccnull,sizeof(*emit));
}

ccfunc ccvalue_t *
ccemit_lvalue(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *tree);

ccfunc ccvalue_t *
ccemit_rvalue(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *tree);

// Todo: is_param?
ccfunc ccvalue_t *
ccemit_include_local(
  ccemit_t *emit, ccprocd_t *procd, ccblock_t *block, ccesse_t *esse, int is_param)
{
  ccassert(esse!=0);


  ccedict_t *e=is_param?
    ccedict_param(esse->tree,esse->type):
    ccedict_local(esse->tree,esse->type);

  ccvalue_t **v=cctblputP(procd->local,esse);
  ccassert(ccerrnon());

  ccvalue_t *i=ccblock_add_edict(procd->decls,e);
  *v=i;

  ccdebuglog("'%s': included local {%s}, local %p",
    ccesse_name_string(esse,ccnull),cctree_string(esse->tree,ccnull),i);
  return i;
}

// Todo: wrong, get the entity....
ccfunc ccvalue_t *
ccemit_constant(ccemit_t *emit, cctype_t *type, ccclassic_t clsc)
{
  (void)emit;

  ccassert(type!=0);

  ccvalue_t *value=ccvalue();
  value->kind=ccvalue_kCONST;
  value->constant.type=type;
  value->constant.clsc=clsc;
  return value;
}

// Todo: wrong, get the entity....
ccfunc ccvalue_t *
ccemit_const_int(ccemit_t *emit, cci64_t value)
{ ccclassic_t classic;
  classic.asi64=value;
  return ccemit_constant(emit,emit->seer->type_stdc_int,classic);
}

// Todo: wrong, get the entity....
ccfunc ccvalue_t *
ccemit_const_str(ccemit_t *emit, ccstr_t value)
{
  ccclassic_t classic;
  classic.value=value;
  return ccemit_constant(emit,emit->seer->type_stdc_char_ptr,classic);
}

ccfunc ccvalue_t *
ccemit_value(ccemit_t *emit, ccprocd_t *procd, ccblock_t *block, cctree_t *tree, int is_lval)
{
  ccvalue_t *result=ccnull;
  switch(tree->kind)
  { case cctree_kSELECTOR:
    case cctree_kIDENTIFIER:
    { ccesse_t *esse=ccseer_symbol(emit->seer,tree);
      ccassert(esse!=0);

      ccvalue_t **local=cctblgetP(procd->local,esse);
      ccassert(local!=0);

      result=*local;

      // Note: debugger helper...
      ccdebuglog("'%s': symbol from tree {%s}, local %p",
        ccesse_name_string(esse,ccnull),cctree_string(tree,ccnull),result);

      if(!is_lval)
        result=ccblock_fetch(block,tree,esse->type,result);
    } break;
#if 0
    case cctree_kSELECTOR:
    {
      cctype_t *type=ccseer_tree_type(emit->seer,tree->lval);
      ccassert(type!=0);

      ccelem_t *elem=cctblgetS(type->list,tree->rval->name);
      ccassert(ccerrnon());

      ccvalue_t *lvalue,*rvalue;
      lvalue=ccemit_value(emit,procd,block,tree->lval,1);
      rvalue=ccemit_const_int(emit,elem->slot);

      // Note: A selector expression could be '->' or '.', we assume the user chose the right
      // selector token, so we just concern ourselves here with emitting the right instruction ...

      if(type->kind==cctype_kPOINTER)
        lvalue=ccblock_fetch(block,tree,type,lvalue);

      // Todo: should we use a specific instruction for this or should all instructions be raw byte offsets??
      // Todo: or, should we get rid of the struct concept itself from here and consider all these as new
      // entities each time...
      result=ccblock_aaddr(block,tree,emit->seer->type_stdc_char,lvalue,rvalue);

      if(!is_lval)
        result=ccblock_fetch(block,tree,elem->type,result);
    } break;
#endif
    case cctree_kBINARY:
    {
      if(cctoken_is_assignment(tree->sort))
      { ccvalue_t *lval=ccemit_value(emit,procd,block,tree->lval,1);
        ccvalue_t *rval=ccemit_value(emit,procd,block,tree->rval,0);
        cctype_t  *type=ccseer_tree_type(emit->seer,tree);
        result=ccblock_store(block,tree,type,lval,rval);
      } else
      if(tree->sort==cctoken_kCMA)
      { ccemit_rvalue(emit,procd,block,tree->lval);
        ccemit_rvalue(emit,procd,block,tree->rval);
      } else
      { ccvalue_t *lhs=ccemit_rvalue(emit,procd,block,tree->lval);
        ccvalue_t *rhs=ccemit_rvalue(emit,procd,block,tree->rval);
        result=ccblock_arith(block,tree,tree->sort,lhs,rhs);
      }
    } break;
    case cctree_kDEREFERENCE:
    { cctype_t *type=ccseer_tree_type(emit->seer,tree->lval);
      ccassert(type!=0);

      result=ccemit_value(emit,procd,block,tree->lval,cctrue);

      if(type->kind==cctype_kPOINTER)
        result=ccblock_fetch(block,tree,type,result);

      if(!is_lval)
        result=ccblock_fetch(block,tree,type->type,result);
    } break;
    case cctree_kINDEX:
    {
      cctype_t *type=ccseer_tree_type(emit->seer,tree->lval);
      ccassert(type!=0);

      ccvalue_t *lvalue,*rvalue;
      lvalue=ccemit_value(emit,procd,block,tree->lval,1);
      rvalue=ccemit_value(emit,procd,block,tree->rval,0);

      // Note:
      if(type->kind==cctype_kPOINTER)
        lvalue=ccblock_fetch(block,tree,type,lvalue);

      result=ccblock_aaddr(block,tree,type->type,lvalue,rvalue);

      if(!is_lval)
        result=ccblock_fetch(block,tree,type->type,result);
    } break;
    case cctree_kSIZEOF:
    {
      // Note: seer should have bound the 'sizeof' expression to the type of its operand...
      cctype_t *type=ccseer_tree_type(emit->seer,tree);
      ccassert(type!=0);

      result=ccemit_const_int(emit,type->size);
    } break;
    case cctree_kADDRESSOF:
    { // Todo:
      ccassert(!is_lval);

      cctype_t *type=ccseer_tree_type(emit->seer,tree);
      result=ccblock_laddr(block,tree,type,ccemit_lvalue(emit,procd,block,tree->lval));
    } break;
    case cctree_kCALL:
    { cctree_t *ltree=tree->lval;
      cctree_t *rtree=tree->rval;

      // Todo: when we key into globals by entity do this ..
      ccassert(ltree->kind==cctree_kIDENTIFIER);

      ccesse_t *esse=ccseer_symbol(emit->seer,ltree);

      switch(esse->sort)
      { case ccbuiltin_kCCBREAK: return ccblock_dbgbreak(block,tree);
        case ccbuiltin_kCCERROR: return ccblock_dbgerror(block,tree);
      }

      ccvalue_t  *lval=ccemit_global(emit,ltree->name);
      ccvalue_t **rval=ccnull;

      cctree_t *list;
      ccarrfor(rtree,list)
        *ccarrone(rval)=ccemit_rvalue(emit,procd,block,list);

      result=ccblock_invoke(block,tree,lval,rval);
    } break;
    // Todo: these have already been generated...
    case cctree_kCONSTANT:
    { if(tree->sort==cctoken_kLITINT)
      { result=ccemit_const_int(emit,cctree_casti64(tree));
      } else
      if(tree->sort==cctoken_kLITSTR)
      {
        ccassert(tree->name!=0);
        result=ccemit_const_str(emit,tree->name);
      } else
        ccassert(!"internal");
    } break;
  }

  ccassert(result!=0);
  return result;
}

ccfunc ccvalue_t *
ccemit_rvalue(ccemit_t *emit, ccprocd_t *procd, ccblock_t *block, cctree_t *tree)
{
  return ccemit_value(emit,procd,block,tree,0);
}

ccfunc ccvalue_t *
ccemit_lvalue(ccemit_t *emit, ccprocd_t *procd, ccblock_t *block, cctree_t *tree)
{
  return ccemit_value(emit,procd,block,tree,1);
}


ccfunc ccvalue_t *
ccemit_implicit_decl(
  ccemit_t *emit, ccprocd_t *procd, ccblock_t *block, ccesse_t *esse, int is_param)
{
  ccassert(esse!=0);
  ccassert(esse->kind==ccesse_kVARIABLE);

  ccassert(esse->root!=0);
  ccassert(esse->root->kind==ccesse_kVARIABLE);

  ccassert(esse->tree!=0);

  ccassert(!esse->tree->init);
  ccassert(!esse->tree->next);

  if(esse->type->kind==cctype_kRECORD)
  { ccesse_t **e;
    ccarrfor(esse->list,e)
      ccemit_implicit_decl(emit,procd,block,*e,is_param);
  }

  ccvalue_t *local=ccemit_include_local(emit,procd,block,esse,is_param);
  ccassert(local!=0);

  return local;
}


ccfunc ccvalue_t *
ccemit_decl(
  ccemit_t *emit, ccprocd_t *procd, ccblock_t *block, cctree_t *tree, int is_param)
{
  ccesse_t *esse=ccseer_symbol(emit->seer,tree);

  if(esse->type->kind==cctype_kRECORD)
  { ccesse_t **e;
    ccarrfor(esse->list,e)
      ccemit_implicit_decl(emit,procd,block,*e,is_param);
  }

  ccvalue_t *local=ccemit_include_local(emit,procd,block,esse,is_param);
  ccassert(local!=0);

  if(tree->init)
  { ccvalue_t *init=ccemit_rvalue(emit,procd,block,tree->init);
    ccassert(init!=0);

    ccblock_store(block,tree,esse->type,local,init);
  }

  return local;
}

// Todo: make the seer output all locals per function to avoid having to
// rely on the tree so much?
// Todo: have each entity also store whether it is a local or not?
ccfunc ccvalue_t *
ccemit_tree(
  ccemit_t *emit, ccprocd_t *procd, ccblock_t *block, cctree_t *tree, int is_param)
{
  ccvalue_t *result=ccnull;

  switch(tree->kind)
  { case cctree_kDECL:
    { cctree_t *n;
      for(n=tree;n;n=n->next)
        ccemit_decl(emit,procd,block,n,is_param);
    } break;
    case cctree_kBLOCK:
    { cctree_t **list;
      ccarrfor(tree->list,list)
        ccemit_tree(emit,procd,block,*list,ccfalse);
    } break;
    case cctree_kCONDITIONAL:
    {
      // Todo:
      ccvalue_t *c=ccemit_tree(emit,procd,block,tree->init,ccfalse);
      ccassert(c!=0);

      ccvalue_t *j=ccblock_fjump(block,tree,ccblock_label(block,"."),c);

      if(tree->lval)
        ccemit_tree(emit,procd,block,tree->lval,ccfalse);

      ccvalue_retarget(j,
        ccblock_label(block,".JP-E"));

      if(tree->rval)
        ccemit_tree(emit,procd,block,tree->rval,ccfalse);

    } break;
    case cctree_kRETURN:
    {
      ccvalue_t *rval=ccemit_rvalue(emit,procd,block,tree->rval);
      result=ccblock_return(block,tree,rval);
    } break;
    case cctree_kITERATOR:
    { ccassert(tree->init!=0);
      ccassert(tree->lval!=0);
      ccassert(tree->rval!=0);
      ccassert(tree->blob!=0);
      ccvalue_t *l,*c,*v;
      ccleap_t p;
      v=ccemit_tree(emit,procd,block,tree->init,ccfalse);
      p=ccblock_label(block,".JP-FL");
      v=ccemit_value(emit,procd,block,tree->lval,ccfalse);
      c=ccblock_fjump(block,tree,ccblock_label(block,"."),v);
      ccemit_tree(emit,procd,block,tree->blob,ccfalse);
      ccemit_tree(emit,procd,block,tree->rval,ccfalse);
      l=ccblock_jump(block,tree,p);
      ccvalue_retarget(c,ccblock_label(block,".JP-FE"));
    } break;
    case cctree_kWHILE:
    {
      ccvalue_t *l,*c,*v;
      ccleap_t p;

      p=ccblock_label(block,".JP-WL");
      v=ccemit_rvalue(emit,procd,block,tree->init);
      c=ccblock_fjump(block,tree,ccblock_label(block,"."),v);

      ccemit_tree(emit,procd,block,tree->lval,ccfalse);

      l=ccblock_jump(block,tree,p);

      ccvalue_retarget(c,ccblock_label(block,".JP-WE"));
    } break;
    default:
    {
      result=ccemit_value(emit,procd,block,tree,ccfalse);
    } break;
  }
  return result;
}

// Todo: Temporary!
ccfunc void
ccemit_external_decl(ccemit_t *emit, ccesse_t *esse)
{
  ccassert(esse!=0);

  if(esse->kind!=ccesse_kFUNCTION)
    return;

  // Todo:
  if(esse->sort!=ccbuiltin_kINVALID)
    return;

  if(!(esse->tree->mark&cctree_mEXTERNAL))
    return;

  if(!esse->tree->blob)
    return;

  ccassert(esse->tree!=0);
  ccassert(esse->type!=0);
  ccassert(esse->name!=0);

  // Todo:
  ccprocd_t *p=ccmalloc_T(ccprocd_t);
  memset(p,ccnull,sizeof(*p));

  p->esse=esse;

  // Todo:
  *ccarradd(p->block,1)=p->decls=ccblock();
  *ccarradd(p->block,1)=p->enter=ccblock();
  *ccarradd(p->block,1)=p->leave=ccblock();

  // Todo:
  ccvalue_t *v=ccemit_include_global(emit,esse->name);
  v->kind=ccvalue_kPROCD;
  v->procd=p;

  // Todo:
  emit->current=p->enter;

  // Note: emit all the parameters first ...
  cctree_t **list;
  ccarrfor(esse->tree->type->list,list)
    ccemit_tree(emit,p,emit->current,*list,cctrue);

  ccarrfor(esse->tree->blob->list,list)
    ccemit_tree(emit,p,emit->current,*list,ccfalse);

  ccblock_jump(p->decls,esse->tree,ccblock_label_ex(p->enter,0,".E"));

  if(!strcmp(esse->name,"main"))
    emit->entry=v;
}




ccfunc void
ccemit_translation_unit(ccemit_t *emit, ccseer_t *seer)
{
  emit->seer=seer;

  ccesse_t **tale;
  ccarrfor(seer->entity_table,tale)
    ccemit_external_decl(emit,*tale);

  // Todo: why would the emitter care? let the exec stage figure this out...
  if(!emit->entry)
    cctraceerr("'main': not found",0);
}
#endif