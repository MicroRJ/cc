#ifndef _CCEMIT
#define _CCEMIT

// warren - "a densely populated or labyrinthine building or district"
// paisley - "a distinctive intricate pattern of curved feather-shaped figures based on an Indian pine-cone design"
// austere - "severe or strict in manner, attitude, or appearance"
// stardom - "the state or status of being a famous or exceptionally talented performer in the world of entertainment or sports"

ccfunc ccemit_value_t *
ccemit_constant(ccemit_t *emit, cctree_t *type, ccclassic_t clsc)
{
  (void)emit;
  ccemit_value_t *value=ccmalloc_T(ccemit_value_t);
  memset(value,ccnil,sizeof(*value));

  value->kind=ccvalue_kCONST;
  value->constant.type=type;
  value->constant.clsc=clsc;
  return value;
}

// Todo:
ccfunc ccemit_value_t *
ccemit_const_i32(ccemit_t *emit, cci64_t value)
{
  ccclassic_t classic={value};
  return ccemit_constant(emit,ctype_int32,classic);
}



ccfunc ccemit_value_t *
ccemit_resolve(ccemit_t *emit, ccemit_procd_t *func, cctree_t *tree)
{ ccnotnil(tree);
  ccnotnil(tree->kind==cctree_kIDENTIFIER);

  cctree_t *lvalue=cctree_resolve_symbol(tree);
  ccnotnil(lvalue);

  ccemit_value_t *result=ccfunc_local(func,lvalue);
  ccnotnil(result);
  return result;
}

ccfunc ccemit_value_t *
ccemit_lvalue(ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *block, cctree_t *tree);

ccfunc ccemit_value_t *
ccemit_rvalue(ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *block, cctree_t *tree);

ccfunc ccemit_value_t *
ccemit_invoke_easy(ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *block, cctree_t *tree)
{
  cctree_t *ltree=tree->lval;
  cctree_t *rtree=tree->rval;

  ccassert(ltree->kind==cctree_kIDENTIFIER);

  cctree_t *allude=cctree_resolve_symbol(ltree);
  ccnotnil(allude);

  ccemit_value_t **call=cctblgetP(emit->globals,allude);
  ccassert(ccerrnon());

  ccemit_value_t **rvalue=ccnil;

  cctree_t *list;
  ccarrfor(rtree,list)
    *ccarrone(rvalue)=ccemit_rvalue(emit,func,block,list);

  return ccemit_invoke(block,*call,rvalue);
}

ccfunc ccemit_value_t *
ccemit_lvalue(ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *block, cctree_t *tree)
{ ccemit_value_t *value=ccnil;
  switch(tree->kind)
  { case cctree_kIDENTIFIER:
    { value=ccemit_resolve(emit,func,tree);
    } break;
    default: ccassert(!"internal");
  }
  return value;
}

ccfunc ccemit_value_t *
ccemit_rvalue(ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *block, cctree_t *tree)
{ ccemit_value_t *value=ccnil;
  switch(tree->kind)
  { case cctree_kINTEGER:
    { value=ccemit_const_i32(emit,tree->as_i32);
    } break;
    case cctree_kBINARY:
    { value=
        ccemit_arith(block,tree->oper,
          ccemit_rvalue(emit,func,block,tree->lval),
          ccemit_rvalue(emit,func,block,tree->rval));
    } break;
    case cctree_kIDENTIFIER:
    { value=ccemit_fetch(block,ccemit_resolve(emit,func,tree));
    } break;
    case cctree_kCALL:
    { value=ccemit_invoke_easy(emit,func,block,tree);
    } break;
    default: ccassert(!"internal");
  }
  return value;
}

ccfunc ccemit_value_t *
ccemit_tree(
  ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *irset, cctree_t *tree);

ccfunc void
ccemit_treelist(
  ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *irset, cctree_t **list)
{
  cctree_t **it;
  ccarrfor(list,it) ccemit_tree(emit,func,irset,*it);
}


ccfunc ccemit_value_t *
ccemit_decl_name(ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *block, cctree_t *tree)
{ ccemit_value_t *lval=ccfunc_include_local(func,tree,ccfalse);

  if(tree->init)
  { ccemit_value_t *rval=ccemit_rvalue(emit,func,block,tree->init);
    ccemit_store(block,lval,rval);
  }
  return lval;
}

ccfunc ccinle void
ccemit_decl(ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *block, cctree_t *decl)
{ cctree_t **list;
  ccarrfor(decl->list,list) ccemit_decl_name(emit,func,block,*list);
}

ccfunc ccemit_value_t *
ccemit_param(ccemit_t *emit, ccemit_procd_t *func, cctree_t *tree)
{ ccemit_value_t *lval=ccfunc_include_local(func,tree,cctrue);
  return lval;
}

ccfunc ccemit_value_t *
ccemit_tree(
  ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *irset, cctree_t *tree)
{
  if(tree->kind==cctree_kDECL)
  {
    ccemit_decl(emit,func,irset,tree);

    return ccnil;
  } else
  if(tree->kind==cctree_kBLOCK)
  {
    ccemit_treelist(emit,func,irset,tree->list);
    return ccnil;
  } else
  if(tree->kind==cctree_kLABEL)
  {
  	// irset=ccemit_label(irset,tree->name);
    // ccemit_treelist(emit,func,irset,tree->list);
    // return ccemit_enter(irset,irset);
  } else
  if(tree->kind==cctree_kRETURN)
  {
    ccemit_value_t *rval=ccemit_rvalue(emit,func,irset,tree->rval);

    return ccemit_return(irset,rval);
  } else
  if(tree->kind==cctree_kGOTO)
  { // ccemit_block_t *label_block=ccemit_label(irset,tree->label_name);
    // return ccemit_enter(irset,label_block);
  } else
  if(tree->kind==cctree_kCALL)
  {
    return ccemit_invoke_easy(emit,func,irset,tree);
  } else
  if(tree->kind==cctree_kBINARY)
  {
  	// Todo: not always an lval, could just be one of those lvalue-less statements ...
    ccemit_value_t *lval=ccemit_lvalue(emit,func,irset,tree->lval);
    ccemit_value_t *rval=ccemit_rvalue(emit,func,irset,tree->rval);

    if(tree->oper==cctoken_kASSIGN)
    {
      return ccemit_store(irset,lval,rval);
    }
    else
    {
      lval=ccemit_fetch(irset,lval);
      return ccemit_arith(irset,tree->oper,lval,rval);
    }

  } else
  if(tree->kind==cctree_kTERNARY)
  { ccemit_value_t *cvalue=ccemit_tree(emit,func,irset,tree->init);

  	ccemit_value_t *jump;
    jump=ccemit_jumpF(irset,irset,0xffff,cvalue);

    if(tree->lval) ccemit_tree(emit,func,irset,tree->lval);
    jump->edict->jump.tar=ccarrlen(irset->edict);
    if(tree->rval) ccemit_tree(emit,func,irset,tree->rval);

    return ccnil;
  } else
  if(tree->kind==cctree_kWHILE)
  {
#if 0
    ccemit_block_t *cond_block=ccvm_block(irset,"$while::cond");
    ccemit_block_t *then_block=ccvm_block(cond_block,"$while::then");
    ccemit_block_t *else_block=ccvm_block(irset,"$while::else");
    ccemit_value_t cond_value=ccemit_tree(emit,func,cond_block,tree->cond_tree);
    ccemit_condi(cond_block,cond_value,then_block,else_block);
    if(tree->then_tree) ccemit_tree(emit,func,then_block,tree->then_tree);
    ccemit_enter(then_block,cond_block);
    ccemit_enter(irset,cond_block);
    emit->current=else_block;
    emit->curirix=0;
#endif

  }

  ccassert(!"error");
  return {};
}



ccfunc void
ccemit_function(ccemit_t *emit, ccemit_procd_t *func, cctree_t *tree)
{
  ccnotnil(tree);

  ccnotnil(tree->type);
  ccassert(tree->type->kind==cctree_kFUNC);

  func->tree=tree;
  func->block=ccnil;

  // Todo:
  *ccarradd(func->block,1)=func->decls=ccblock("$decls");
  *ccarradd(func->block,1)=func->enter=ccblock("$enter");
  *ccarradd(func->block,1)=func->leave=ccblock("$leave");

  ccnotnil(tree->type);
  ccassert(tree->type->kind==cctree_kFUNC);

  ccnotnil(tree->blob);
  ccassert(tree->blob->kind==cctree_kBLOCK);

  // Note: emit all the parameters first ...
  cctree_t **list;
  ccarrfor(tree->type->list,list)
    ccemit_param(emit,func,*list);

  // Todo: is this flawed?
  emit->current=func->enter;
  ccarrfor(tree->blob->list,list)
    ccemit_tree(emit,func,emit->current,ccdref(list));

  ccemit_enter(func->decls,func->enter);
}


ccfunc void
ccemit_external_decl(ccemit_t *emit, cctree_t *tree)
{
  cctree_t **decl_;
  cctree_t  *decl;
  ccarrfor(tree->list,decl_)
  { decl=*decl_;

    if(decl->type->kind==cctree_kFUNC)
    {
      ccemit_value_t **value=cctblputP(emit->globals,decl);
      ccassert(ccerrnon());
      ccemit_procd_t *func=ccmalloc_T(ccemit_procd_t);
  		memset(func,ccnil,sizeof(*func));
      *value=ccmalloc_T(ccemit_value_t);
      (*value)->kind=ccvalue_kFUNC;
      (*value)->func=func;

      ccemit_function(emit,func,decl);

      if(!strcmp(decl->name,"main"))
        emit->entry=*value;
    } else
    {
      // Todo:
      ccassert(!"error");
    }
  }
}

ccfunc void
ccemit_translation_unit(ccemit_t *emit, cctree_t *tree)
{
  cctree_solve(tree);

  cctree_t **decl;
  ccarrfor(tree->list,decl) ccemit_external_decl(emit,*decl);
}

ccfunc void
ccemit_init(ccemit_t *emit)
{ memset(emit,ccnil,sizeof(*emit));
}

#endif