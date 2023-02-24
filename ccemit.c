// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCEMIT
#define _CCEMIT

ccfunc ccvalue_t *
ccemit_lvalue(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *tree);

ccfunc ccvalue_t *
ccemit_rvalue(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *tree);

ccfunc ccvalue_t *
ccemit_include_local(
  ccemit_t *emit, ccprocd_t *procd, ccblock_t *block, cctree_t *tree, int is_param)
{
  ccassert(tree!=0);
  ccassert(tree->kind==cctree_kDECLNAME);

  ccesse_t *esse=ccseer_symbol(emit->seer,tree);
  ccassert(esse!=0);

  ccedict_t *e =is_param?
    ccedict_param(esse->type):
    ccedict_local(esse->type);

  ccvalue_t  *i=ccblock_add_edict(procd->decls,e);
  ccvalue_t **v=cctblputP(procd->local,tree);
  ccassert(ccerrnon());
  *v=i;

  if(tree->init)
    ccblock_store(block,esse->type,i,ccemit_rvalue(emit,procd,block,tree->init));

  return i;
}

// Todo: wrong, get the entity....
ccfunc ccvalue_t *
ccemit_constant(ccemit_t *emit, cctype_t *type, ccclassic_t clsc)
{
  (void)emit;

  ccassert(type!=0);

  ccvalue_t *value=ccvalue("constant");
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
  return ccemit_constant(emit,cctype_stdc_int,classic);
}

// Todo: wrong, get the entity....
ccfunc ccvalue_t *
ccemit_const_str(ccemit_t *emit, ccstr_t value)
{
  ccclassic_t classic;
  classic.value=value;
  return ccemit_constant(emit,cctype_stdc_char_ptr,classic);
}

ccfunc ccvalue_t *
ccemit_invoke_easy(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *tree)
{
  // Todo:
  cctree_t *ltree=tree->lval;
  cctree_t *rtree=tree->rval;

  ccassert(ltree->kind==cctree_kLITIDE);

  ccesse_t *esse=ccseer_symbol(emit->seer,ltree);

  switch(esse->sort)
  { case ccbuiltin_kCCBREAK: return ccblock_dbgbreak(block);
    case ccbuiltin_kCCERROR: return ccblock_dbgerror(block);
  }

#if 0
  cctree_t *allude=ccseer_symbol(emit->seer,tree);
  ccassert(allude!=0);
  ccvalue_t  *lval=ccemit_global(emit,allude->name);
  ccvalue_t **rval=ccnil;
#endif
  ccvalue_t  *lval=ccemit_global(emit,tree->name);
  ccvalue_t **rval=ccnil;


  cctree_t *list;
  ccarrfor(rtree,list)
  {
    *ccarrone(rval)=ccemit_rvalue(emit,func,block,list);
  }

  return ccblock_invoke(block,lval,rval);
}

ccfunc ccvalue_t *
ccemit_value(ccemit_t *emit, ccprocd_t *procd, ccblock_t *block, cctree_t *tree, int is_lval)
{
  ccvalue_t *result=ccnull;
  switch(tree->kind)
  { case cctree_kLITIDE:
    { ccesse_t *esse=ccseer_symbol(emit->seer,tree);
      ccassert(esse!=0);

      // Todo:
      result=ccprocd_local(procd,esse->tree);
      ccassert(result!=0);

      if(!is_lval)
        result=ccblock_fetch(block,esse->type,result);
    } break;
    case cctree_kINDEX:
    { ccassert(tree->lval!=0);
      ccassert(tree->rval!=0);
      // Note:
      //
      // int *a;
      // a[0]=1;
      //
      // >> local * int[1]
      // >> fetch a
      // >> aaddr a, 0
      //
      // int a[256];
      // a[0]=1
      //
      // >> local int[256]
      // >> aaddr a, 0

      cctype_t *type=ccseer_tree_type(emit->seer,tree->lval);
      ccassert(type!=0);

      ccvalue_t *lvalue,*rvalue;
      lvalue=ccemit_value(emit,procd,block,tree->lval,1);
      rvalue=ccemit_value(emit,procd,block,tree->rval,0);

      // Note:
      if(type->kind==cctype_kPOINTER)
        lvalue=ccblock_fetch(block,type,lvalue);

      result=ccblock_aaddr(block,type->type,lvalue,rvalue);

      if(!is_lval)
        result=ccblock_fetch(block,type->type,result);

    } break;
    case cctree_kBINARY:
    { ccassert(tree->sort!=cctoken_kINVALID);
      ccassert(tree->lval!=0);
      ccassert(tree->rval!=0);

      if(cctoken_is_assignment(tree->sort))
      { ccvalue_t *lval=ccemit_value(emit,procd,block,tree->lval,1);
        ccvalue_t *rval=ccemit_value(emit,procd,block,tree->rval,0);
        cctype_t  *type=ccseer_tree_type(emit->seer,tree);
        result=ccblock_store(block,type,lval,rval);
      } else
      if(tree->sort==cctoken_kCMA)
      { ccemit_rvalue(emit,procd,block,tree->lval);
        ccemit_rvalue(emit,procd,block,tree->rval);
      } else
      { ccvalue_t *lhs=ccemit_rvalue(emit,procd,block,tree->lval);
        ccvalue_t *rhs=ccemit_rvalue(emit,procd,block,tree->rval);
        result=ccblock_arith(block,tree->sort,lhs,rhs);
      }
    } break;
    case cctree_kUNARY:
    { ccassert(tree->sort!=cctoken_kINVALID);
      ccassert(tree->rval!=0);

      if(tree->sort==cctoken_kDEREFERENCE)
      {
        cctype_t *type=ccseer_tree_type(emit->seer,tree->rval);

        result=ccemit_value(emit,procd,block,tree->rval,1);

        if(type->kind==cctype_kPOINTER)
          result=ccblock_fetch(block,type,result);

        if(!is_lval)
          result=ccblock_fetch(block,type->type,result);
      } else
      if(tree->sort==cctoken_kADDRESSOF)
      { ccassert(!is_lval);

        // Care:
        cctype_t *type=ccseer_tree_type(emit->seer,tree);
        result=ccblock_laddr(block,type,ccemit_lvalue(emit,procd,block,tree->rval));
      } else
        ccassert(!"internal");
    } break;
    case cctree_kCALL:
    { result=ccemit_invoke_easy(emit,procd,block,tree);
    } break;
    case cctree_kLITINT:
    { result=ccemit_const_int(emit,tree->as_i32);
    } break;
    case cctree_kLITSTR:
    { result=ccemit_const_str(emit,tree->as_str);
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

ccfunc ccinle ccvalue_t *
ccemit_decl_name(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *tree)
{
  return ccemit_include_local(emit,func,block,tree,ccfalse);
}

ccfunc ccinle void
ccemit_decl(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *decl)
{ cctree_t **list;
  ccarrfor(decl->list,list) ccemit_decl_name(emit,func,block,*list);
}

ccfunc ccvalue_t *
ccemit_tree(
  ccemit_t *emit, ccprocd_t *func, ccblock_t *irset, cctree_t *tree)
{
  if(tree->kind==cctree_kDECL)
  {
    ccemit_decl(emit,func,irset,tree);

    return ccnil;
  } else
  if(tree->kind==cctree_kBLOCK)
  {
    cctree_t **list;
    ccarrfor(tree->list,list) ccemit_tree(emit,func,irset,*list);

    return ccnil;
  } else
  if(tree->kind==cctree_kLABEL)
  {
    // irset=ccemit_label(irset,tree->name);
    // ccemit_treelist(emit,func,irset,tree->list);
    // return ccblock_enter(irset,irset);
  } else
  if(tree->kind==cctree_kRETURN)
  {
    ccvalue_t *rval=ccemit_rvalue(emit,func,irset,tree->rval);

    return ccblock_return(irset,rval);
  } else
  if(tree->kind==cctree_kGOTO)
  { // ccblock_t *label_block=ccemit_label(irset,tree->label_name);
    // return ccblock_enter(irset,label_block);
  } else
  if(tree->kind==cctree_kCALL)
  {
    return ccemit_invoke_easy(emit,func,irset,tree);
  } else
  if(tree->kind==cctree_kBINARY)
  {
    return ccemit_rvalue(emit,func,irset,tree);
  } else
  if(tree->kind==cctree_kTERNARY)
  { ccvalue_t *cvalue=ccemit_tree(emit,func,irset,tree->init);

    ccvalue_t *j;
    j=ccblock_fjump(irset,ccblock_label(irset,"."),cvalue);

    if(tree->lval) ccemit_tree(emit,func,irset,tree->lval);

    ccvalue_retarget(j,
      ccblock_label(irset,".JP-E"));

    if(tree->rval) ccemit_tree(emit,func,irset,tree->rval);

    return ccnil;
  } else
  if(tree->kind==cctree_kWHILE)
  {
    ccvalue_t *l,*c,*v;
    ccleap_t p;

    p=ccblock_label(irset,".JP-WL");
    v=ccemit_rvalue(emit,func,irset,tree->init);
    c=ccblock_fjump(irset,ccblock_label(irset,"."),v);

    ccemit_tree(emit,func,irset,tree->lval);

    l=ccblock_jump(irset,p);

    ccvalue_retarget(c,
      ccblock_label(irset,".JP-WE"));

    return ccnil;
  }

  ccassert(!"error");
  return ccnil;
}

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

  ccprocd_t *p=ccmalloc_T(ccprocd_t);
  memset(p,0,sizeof(*p));

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

  ccblock_jump(p->decls,ccblock_label_ex(p->enter,0,".E"));

  if(!strcmp(esse->tree->name,"main"))
    emit->entry=v;

  cctracelog("built: %s",esse->tree->name);
}

ccfunc void
ccemit_translation_unit(ccemit_t *emit, ccseer_t *seer)
{
  emit->seer=seer;

  ccesse_t **tale;
  ccarrfor(seer->entity_table,tale)
    ccemit_external_decl(emit,*tale);

  ccassert(emit->entry!=0);
}

ccfunc void
ccemit_init(ccemit_t *emit)
{
  memset(emit,ccnil,sizeof(*emit));
}
#endif