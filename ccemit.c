#ifndef _CCEMIT
#define _CCEMIT

// paisley - "a distinctive intricate pattern of curved feather-shaped figures based on an Indian pine-cone design"
// austere - "severe or strict in manner, attitude, or appearance"


ccfunc ccemit_value_t
ccemit_value_edict(ccedict_t *edict)
{ ccemit_value_t value={};
  value.kind=ccvalue_kEDICT;
  value.edict=edict;
  return value;
}

ccfunc ccemit_value_t
ccemit_value_const(cctree_t *type, ccclassic_t clsc)
{ ccemit_value_t value={};
  value.kind=ccvalue_kCONST;
  value.constant.type=type;
  value.constant.clsc=clsc;
  return value;
}

ccfunc ccemit_value_t
ccemit_value_const_i32(cci32_t val)
{
  return ccemit_value_const(ctype_int32,{val});
}

ccfunc ccemit_value_t *
ccvm_value_init(ccemit_value_t *value, ccvalue_k kind, const char *name)
{ memset(value,ccnil,sizeof(*value));
  value->kind=kind;
  return value;
}

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

// warren - "a densely populated or labyrinthine building or district"

// Todo:
ccfunc ccemit_value_t *
ccemit_const_i32(ccemit_t *emit, cci64_t value)
{
  ccclassic_t classic={value};
  return ccemit_constant(emit,ctype_int32,classic);
}

ccfunc ccblock_t *
ccvm_block(const char *debug_label)
{
  ccblock_t *block=(ccblock_t *)ccmalloc(sizeof(*block));
  return ccblock_I(block,debug_label);
}

ccfunc ccblock_t *
ccemit_label(ccblock_t *irset, const char *name)
{
#if 0
  int ait;
  ccemit_value_t *value=cctblsetS(irset->local,name,&ait);

  if(!ait)
  { value->kind=ccvalue_Kblock;
    value->block=ccvm_block(name);
  }
  return value->block;
#endif

	return ccnil;
}

ccfunc ccinle ccemit_value_t *
ccemit_store(ccblock_t *block, ccemit_value_t *lval, ccemit_value_t *rval)
{
  return ccblock_add_edict(block,ccedict_store(lval,rval));
}

ccfunc ccinle ccemit_value_t *
ccemit_fetch(ccblock_t *block, ccemit_value_t *rval)
{
  return ccblock_add_edict(block,ccedict_fetch(rval));
}

ccfunc ccinle ccemit_value_t *
ccemit_arith(ccblock_t *block, cctoken_k opr, ccemit_value_t *lhs, ccemit_value_t *rhs)
{
  return ccblock_add_edict(block,ccedict_arith(opr,lhs,rhs));
}

ccfunc ccemit_value_t *
ccemit_enter(ccblock_t *block, ccblock_t *blc)
{
  return ccblock_add_edict(block,ccedict_enter(blc));
}


#if 0
ccfunc ccedict_t *
ccemit_condi(ccblock_t *block, ccemit_value_t *cnd, ccblock_t *then_blc, ccblock_t *else_blc)
{ ccedict_t *i=ccblock_add(block);
  i->kind=ccedict_kCONDI;
  i->condi.cnd=cnd;
  i->condi.then_blc=then_blc;
  i->condi.else_blc=else_blc;
  return i;
}
ccfunc ccedict_t *
ccemit_return(ccblock_t *irset)
{ ccedict_t *i=ccblock_add(irset);
  i->kind=ccedict_kRETURN;
  return i;
}
#endif

ccfunc ccemit_value_t *
ccemit_call(ccblock_t *block, ccemit_value_t *value)
{
	return ccblock_add_edict(block,ccedict_call(value));
}


ccfunc ccemit_value_t *
ccemit_resolve(ccemit_t *emit, ccfunction_t *func, cctree_t *tree)
{ ccnotnil(tree);
	ccnotnil(tree->kind==cctree_kIDENTIFIER);

	cctree_t *lvalue=cctree_resolve_symbol(tree);
	ccnotnil(lvalue);

	ccemit_value_t *result=ccfunc_local(func,lvalue);
	ccnotnil(result);
  return result;
}

ccfunc ccemit_value_t *
ccemit_lvalue(ccemit_t *emit, ccfunction_t *func, ccblock_t *block, cctree_t *tree)
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
ccemit_rvalue(ccemit_t *emit, ccfunction_t *func, ccblock_t *block, cctree_t *tree)
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
  }
  return value;
}

ccfunc ccemit_value_t *
ccemit_tree(
  ccemit_t *emit, ccfunction_t *func, ccblock_t *irset, cctree_t *tree);

ccfunc void
ccemit_treelist(
  ccemit_t *emit, ccfunction_t *func, ccblock_t *irset, cctree_t **list)
{
  cctree_t **it;
  ccarrfor(list,it) ccemit_tree(emit,func,irset,*it);
}

// stardom - "the state or status of being a famous or exceptionally talented performer in the world of entertainment or sports"

ccfunc ccemit_value_t *
ccemit_tree(
  ccemit_t *emit, ccfunction_t *func, ccblock_t *irset, cctree_t *tree)
{
  if(tree->kind==cctree_kDECL)
  {
    ccassert(!"error");
  } else
  if(tree->kind==cctree_kBLOCK)
  {
    ccassert(!"error");

    // ccblock_t *child=ccvm_block(irset,"$block");
    // ccemit_treelist(emit,func,child,tree->stmt_list);
    // ccemit_enter(irset,child);
    // return ccemit_value_edict(child);
  } else
  if(tree->kind==cctree_kLABEL)
  { irset=ccemit_label(irset,tree->name);
    ccemit_treelist(emit,func,irset,tree->list);
    return ccemit_enter(irset,irset);
  } else
  if(tree->kind==cctree_kRETURN)
  { // return ccemit_return(irset);
  } else
  if(tree->kind==cctree_kGOTO)
  { // ccblock_t *label_block=ccemit_label(irset,tree->label_name);
    // return ccemit_enter(irset,label_block);
  } else
  if(tree->kind==cctree_kCALL)
  {
  	cctree_t *lval=tree->lval;
  	// cctree_t *rval=tree->rval;

  	cctree_t *allude=cctree_resolve_symbol(lval);
  	ccemit_value_t **value=cctblgetP(emit->globals,allude);
  	ccassert(ccerrnon());

   	return ccemit_call(func->enter,*value);

    // ccstr_t func_name=tree->expr_tree->constant.token.str;
    // int in_table;
    // if(in_table)
    // { ccemit_call(irset,func_value);
    // } else cctraceerr("undefined function");

  } else
  if(tree->kind==cctree_kBINARY)
  {
  	ccemit_value_t *lval=ccemit_lvalue(emit,func,func->enter,tree->lval);
  	ccemit_value_t *rval=ccemit_rvalue(emit,func,func->enter,tree->rval);

    if(tree->oper==cctoken_kASSIGN)
    {
    	return ccemit_store(func->enter,lval,rval);
    }
    else
    {
    	lval=ccemit_fetch(func->enter,lval);
    	return ccemit_arith(irset,tree->oper,lval,rval);
    }

  } else
  if(tree->kind==cctree_kWHILE)
  {
#if 0
    ccblock_t *cond_block=ccvm_block(irset,"$while::cond");
    ccblock_t *then_block=ccvm_block(cond_block,"$while::then");
    ccblock_t *else_block=ccvm_block(irset,"$while::else");
    ccemit_value_t cond_value=ccemit_tree(emit,func,cond_block,tree->cond_tree);
    ccemit_condi(cond_block,cond_value,then_block,else_block);
    if(tree->then_tree) ccemit_tree(emit,func,then_block,tree->then_tree);
    ccemit_enter(then_block,cond_block);
    ccemit_enter(irset,cond_block);
    emit->current=else_block;
    emit->curirix=0;
#endif

  } else
  if(tree->kind==cctree_kTERNARY)
  {
#if 0
    ccemit_value_t *cond_value=ccemit_tree(emit,func,irset,tree->cond_tree);
    ccblock_t *then_block=ccnil,*else_block=ccnil;
    ccblock_t *done_block=ccvm_block(irset,"$local");
    if(tree->then_tree)
    { then_block=ccvm_block(irset,"$if::then");
      ccemit_tree(emit,func,then_block,tree->then_tree);
      ccemit_enter(then_block,done_block);
    }
    if(tree->else_tree)
    { irset=else_block=ccvm_block(irset,"$if::else");
      ccemit_tree(emit,func,else_block,tree->else_tree);
      ccemit_enter(else_block,done_block);
    }
    ccemit_condi(irset,cond_value,then_block,else_block);
    emit->current=done_block;
    emit->curirix=0;
#endif
  }

  ccassert(!"error");
  return {};
}

ccfunc ccemit_value_t *
ccemit_decl_name(ccemit_t *emit, ccfunction_t *func, cctree_t *tree)
{ ccemit_value_t *lval=ccfunc_include_local(func,tree,ccfalse);
  if(tree->init)
  { ccemit_value_t *rval=ccemit_rvalue(emit,func,func->enter,tree->init);
	  ccemit_store(func->enter,lval,rval);
  }
  return lval;
}

ccfunc ccinle void
ccemit_decl(ccemit_t *emit, ccfunction_t *func, cctree_t *decl)
{ cctree_t **list;
  ccarrfor(decl->list,list) ccemit_decl_name(emit,func,*list);
}

ccfunc ccemit_value_t *
ccemit_param(ccemit_t *emit, ccfunction_t *func, cctree_t *tree)
{ ccemit_value_t *lval=ccfunc_include_local(func,tree,cctrue);
	return lval;
}

ccfunc ccfunction_t *
ccemit_function(ccemit_t *emit, cctree_t *tree)
{
  ccnotnil(tree);

  ccnotnil(tree->type);
  ccassert(tree->type->kind==cctree_kFUNC);

  ccfunction_t *func=ccmalloc_T(ccfunction_t);
  memset(func,ccnil,sizeof(*func));
  func->tree=tree;

  // Todo:
  ccarrres(func->block,8);
  ccarrfix(func->block);
  func->decls=ccarradd(func->block,1);
  func->enter=ccarradd(func->block,1);
  func->leave=ccarradd(func->block,1);

  ccblock_I(func->decls,"$decls");
  ccblock_I(func->enter,"$enter");
  ccblock_I(func->leave,"$leave");

  cctree_t **list;
  ccarrfor(tree->type->list,list)
  { ccemit_param(emit,func,*list);
  }


  ccnotnil(tree->blob);
  ccassert(tree->blob->kind==cctree_kBLOCK);

  cctree_t **stmt;
  ccarrfor(tree->blob->list,stmt)
  {
    if(ccdref(stmt)->kind==cctree_kDECL)
    {
      ccemit_decl(emit,func,ccdref(stmt));
    } else
    {
      ccemit_tree(emit,func,func->enter,ccdref(stmt));
    }
  }

  ccemit_enter(func->decls,func->enter);

  return func;
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

    	ccfunction_t *func=ccemit_function(emit,decl);

      *value=ccmalloc_T(ccemit_value_t);
      (*value)->kind=ccvalue_kFUNC;
      (*value)->func=func;

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

  emit->current=ccvm_block("main");
  emit->curirix=0;
}

#endif