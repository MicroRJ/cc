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

ccfunc ccvalue_t *
ccemit_include_local(
  ccemit_t *emit, ccprocd_t *procd, ccblock_t *block, cctree_t *tree, int is_param)
{
  ccassert(tree!=0);
  ccassert(tree->kind==cctree_kDECL);

  ccesse_t *esse=ccseer_symbol(emit->seer,tree);
  ccassert(esse!=0);

  ccedict_t *e =is_param?
    ccedict_param(tree,esse->type):
    ccedict_local(tree,esse->type);

  ccvalue_t  *i=ccblock_add_edict(procd->decls,e);
  ccvalue_t **v=cctblputP(procd->local,tree);
  ccassert(ccerrnon());
  *v=i;

  if(tree->init)
    ccblock_store(block,tree->init,esse->type,i,
      ccemit_rvalue(emit,procd,block,tree->init));

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
  { case cctree_kIDENTIFIER:
    { ccesse_t *esse=ccseer_symbol(emit->seer,tree);
      ccassert(esse!=0);

      // Todo: lookup by entity instead ...
      result=ccprocd_local(procd,esse->tree);
      ccassert(result!=0);

      if(!is_lval)
        result=ccblock_fetch(block,tree,esse->type,result);
    } break;
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

      result=ccemit_value(emit,procd,block,tree->lval,1);

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

      // Todo: should we use a specific instruction for this?
      result=ccblock_aaddr(block,tree,emit->seer->type_stdc_char,lvalue,rvalue);

      if(!is_lval)
        result=ccblock_fetch(block,tree,elem->type,result);

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

ccfunc ccinle ccvalue_t *
ccemit_param(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *tree)
{
  return ccemit_include_local(emit,func,block,tree,cctrue);
}

ccfunc ccvalue_t *
ccemit_tree(
  ccemit_t *emit, ccprocd_t *procd, ccblock_t *block, cctree_t *tree)
{
  ccvalue_t *result=ccnull;
  if(tree->kind==cctree_kDECL)
  {
    cctree_t *next;
    for(next=tree;next;next=next->next)
    {
      ccemit_include_local(emit,procd,block,next,ccfalse);
    }

    return ccnull;
  } else
  if(tree->kind==cctree_kBLOCK)
  {
    cctree_t **list;
    ccarrfor(tree->list,list) ccemit_tree(emit,procd,block,*list);

    return ccnil;
  } else
  if(tree->kind==cctree_kLABEL)
  {
    // block=ccemit_label(block,tree->name);
    // ccemit_treelist(emit,procd,block,tree->list);
    // return ccblock_enter(block,block);
  } else
  if(tree->kind==cctree_kRETURN)
  {
    ccvalue_t *rval=ccemit_rvalue(emit,procd,block,tree->rval);

    return ccblock_return(block,tree,rval);
  } else
  if(tree->kind==cctree_kGOTO)
  { // ccblock_t *label_block=ccemit_label(block,tree->label_name);
    // return ccblock_enter(block,label_block);
  } else
  if(tree->kind==cctree_kCONDITIONAL)
  { ccvalue_t *cvalue=ccemit_tree(emit,procd,block,tree->init);

    ccvalue_t *j;
    j=ccblock_fjump(block,tree,ccblock_label(block,"."),cvalue);

    if(tree->lval)
      ccemit_tree(emit,procd,block,tree->lval);

    ccvalue_retarget(j,
      ccblock_label(block,".JP-E"));

    if(tree->rval) ccemit_tree(emit,procd,block,tree->rval);

    return ccnil;
  } else
  if(tree->kind==cctree_kITERATOR)
  {
    // Todo:
    ccassert(tree->init!=0);
    ccassert(tree->lval!=0);
    ccassert(tree->rval!=0);
    ccassert(tree->blob!=0);
    ccvalue_t *l,*c,*v;
    ccleap_t p;
    v=ccemit_tree(emit,procd,block,tree->init);
    p=ccblock_label(block,".JP-FL");
    v=ccemit_value(emit,procd,block,tree->lval,ccfalse);
    c=ccblock_fjump(block,tree,ccblock_label(block,"."),v);
    ccemit_tree(emit,procd,block,tree->blob);
    ccemit_tree(emit,procd,block,tree->rval);
    l=ccblock_jump(block,tree,p);
    ccvalue_retarget(c,ccblock_label(block,".JP-FE"));
  } else
  if(tree->kind==cctree_kWHILE)
  {
    ccvalue_t *l,*c,*v;
    ccleap_t p;

    p=ccblock_label(block,".JP-WL");
    v=ccemit_rvalue(emit,procd,block,tree->init);
    c=ccblock_fjump(block,tree,ccblock_label(block,"."),v);

    ccemit_tree(emit,procd,block,tree->lval);

    l=ccblock_jump(block,tree,p);

    ccvalue_retarget(c,ccblock_label(block,".JP-WE"));
  } else
  {
    result=ccemit_value(emit,procd,block,tree,ccfalse);
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
    ccemit_param(emit,p,emit->current,*list);

  ccarrfor(esse->tree->blob->list,list)
    ccemit_tree(emit,p,emit->current,*list);

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