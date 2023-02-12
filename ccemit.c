#ifndef _CCEMIT
#define _CCEMIT


// warren - "a densely populated or labyrinthine building or district"
// paisley - "a distinctive intricate pattern of curved feather-shaped figures based on an Indian pine-cone design"
// austere - "severe or strict in manner, attitude, or appearance"
// stardom - "the state or status of being a famous or exceptionally talented performer in the world of entertainment or sports"

ccfunc ccemit_value_t *
ccemit_lvalue(ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *block, cctree_t *tree);

ccfunc ccemit_value_t *
ccemit_rvalue(ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *block, cctree_t *tree);


// Todo: cctree_to_type will not work with variables, this is because variables are not constant,
// and they are not generated before the decl block adds all the locals, so when a local array
// is added, no other instructions will be present ...

// Todo: this is temporary ...
ccfunc ccinle cctype_t *
cctree_to_type(ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *block, cctree_t *tree)
{ cctype_t *type=ccnil;
 	if(tree->kind==cctree_kARRAY)
	{
		type=cctype(cctype_kARRAY,"array");
		type->type=cctree_to_type(emit,func,block,tree->type);

		ccemit_value_t *v=ccemit_rvalue(emit,func,block,tree->rval);
		type->length=v;

	} else
 	if(tree->kind==cctree_kTYPENAME)
	{ type=cctype(cctype_kINTEGER,"integer");
	} else
		ccassert(!"error");
	return type;
}

ccfunc ccemit_value_t *
ccemit_include_local(ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *block, cctree_t *tree, int is_param)
{
	ccnotnil(tree);
	ccassert(tree->kind==cctree_kDECLNAME);

	cctype_t *type=cctree_to_type(emit,func,block,tree->type);
	ccassert(type!=ccnil);

	ccedict_t *e =is_param?
		ccedict_param(type,tree->name):
		ccedict_local(type,tree->name);

  ccemit_value_t  *i=ccblock_add_edict(func->decls,e);
  ccemit_value_t **v=cctblputP(func->local,tree);
  ccassert(ccerrnon());
  *v=i;
  return i;
}

ccfunc ccemit_value_t *
ccemit_constant(ccemit_t *emit, cctype_t *type, ccclassic_t clsc)
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
  return ccemit_constant(emit,ccnil,classic);
}

ccfunc ccemit_value_t *
ccemit_resolve(ccemit_t *emit, ccemit_procd_t *func, cctree_t *tree)
{ ccnotnil(tree);
  ccassert((tree->kind==cctree_kLITIDE)||
  				 (tree->kind==cctree_kINDEX));

  cctree_t *couple=cctree_resolve_symbol(tree);
  ccnotnil(couple);

  // Note: this will happen until we implement scopes ...
  ccemit_value_t *result=ccprocd_local(func,couple);
  ccnotnil(result);
  return result;
}


ccfunc ccemit_value_t *
ccemit_invoke_easy(ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *block, cctree_t *tree)
{
  cctree_t *ltree=tree->lval;
  cctree_t *rtree=tree->rval;

  ccassert(ltree->kind==cctree_kLITIDE);

  // Todo:
  (void)ltree;

  cctree_t *allude=cctree_resolve_symbol(tree);
  ccnotnil(allude);

  ccemit_value_t *call=ccemit_global(emit,allude);

  ccemit_value_t **rvalue=ccnil;

  cctree_t *list;
  ccarrfor(rtree,list)
    *ccarrone(rvalue)=ccemit_rvalue(emit,func,block,list);

  return ccblock_invoke(block,call->procd,rvalue);
}

ccfunc ccemit_value_t *
ccemit_lvalue(ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *block, cctree_t *tree)
{ ccemit_value_t *value=ccnil;
  switch(tree->kind)
  { case cctree_kLITIDE:
    { value=ccemit_resolve(emit,func,tree);
    } break;

  	case cctree_kINDEX:
    { ccassert(tree->lval!=ccnil);
    	ccassert(tree->rval!=ccnil);

    	ccemit_value_t *lval,*rval;
    	lval=ccemit_lvalue(emit,func,block,tree->lval);
    	rval=ccemit_rvalue(emit,func,block,tree->rval);

    	value=ccblock_address(block,lval,rval);
    } break;

    default: ccassert(!"internal");
  }
  return value;
}

ccfunc ccemit_value_t *
ccemit_rvalue(ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *block, cctree_t *tree)
{ ccemit_value_t *value=ccnil;
  switch(tree->kind)
  { case cctree_kLITINT:
    { value=ccemit_const_i32(emit,tree->as_i32);
    } break;
    case cctree_kBINARY:
    { value=
        ccblock_arith(block,tree->oper,
          ccemit_rvalue(emit,func,block,tree->lval),
          ccemit_rvalue(emit,func,block,tree->rval));
    } break;
    case cctree_kLITIDE:
    { value=ccblock_fetch(block,ccemit_resolve(emit,func,tree),ccnil);
    } break;
    case cctree_kCALL:
    { value=ccemit_invoke_easy(emit,func,block,tree);
    } break;
  	case cctree_kINDEX:
    { ccassert(tree->lval!=ccnil);
    	ccassert(tree->rval!=ccnil);

    	ccemit_value_t *lval,*rval;
    	lval=ccemit_lvalue(emit,func,block,tree->lval);
    	rval=ccemit_rvalue(emit,func,block,tree->rval);

    	value=ccblock_fetch(block,lval,rval);
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
{ ccemit_value_t *lval=ccemit_include_local(emit,func,block,tree,ccfalse);

  if(tree->init)
  { ccemit_value_t *rval=ccemit_rvalue(emit,func,block,tree->init);
    ccblock_store(block,lval,rval);
  }
  return lval;
}

ccfunc ccinle void
ccemit_decl(ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *block, cctree_t *decl)
{ cctree_t **list;
  ccarrfor(decl->list,list) ccemit_decl_name(emit,func,block,*list);
}

ccfunc ccemit_value_t *
ccemit_param(ccemit_t *emit, ccemit_procd_t *func, ccemit_block_t *block, cctree_t *tree)
{ ccemit_value_t *lval=ccemit_include_local(emit,func,block,tree,cctrue);
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
    // return ccblock_enter(irset,irset);
  } else
  if(tree->kind==cctree_kRETURN)
  {
    ccemit_value_t *rval=ccemit_rvalue(emit,func,irset,tree->rval);

    return ccblock_return(irset,rval);
  } else
  if(tree->kind==cctree_kGOTO)
  { // ccemit_block_t *label_block=ccemit_label(irset,tree->label_name);
    // return ccblock_enter(irset,label_block);
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
      return ccblock_store(irset,lval,rval);
    }
    else
    {
    	// Todo: proper fetch
      lval=ccblock_fetch(irset,lval,ccnil);

      return ccblock_arith(irset,tree->oper,lval,rval);
    }

  } else
  if(tree->kind==cctree_kTERNARY)
  { ccemit_value_t *cvalue=ccemit_tree(emit,func,irset,tree->init);

  	ccemit_value_t *j;
    j=ccblock_fjump(irset,{},cvalue);

    if(tree->lval) ccemit_tree(emit,func,irset,tree->lval);

    ccvalue_retarget(j,
    	ccblock_label(irset,".JP-E"));

    if(tree->rval) ccemit_tree(emit,func,irset,tree->rval);

    return ccnil;
  } else
  if(tree->kind==cctree_kWHILE)
  {
  	ccemit_value_t *l,*c,*v;
  	ccjump_point_t p;

    p=ccblock_label(irset,".JP-WL");
  	v=ccemit_rvalue(emit,func,irset,tree->init);
    c=ccblock_fjump(irset,{},v);

  	ccemit_tree(emit,func,irset,tree->lval);

    l=ccblock_jump(irset,p);

    ccvalue_retarget(c,
    	ccblock_label(irset,".JP-WE"));

    return ccnil;
  }

  ccassert(!"error");
  return {};
}



ccfunc void
ccemit_function(ccemit_t *emit, ccemit_procd_t *procd, cctree_t *tree)
{
  ccnotnil(tree);

  ccnotnil(tree->type);
  ccassert(tree->type->kind==cctree_kFUNC);

  ccnotnil(tree->blob);
  ccassert(tree->blob->kind==cctree_kBLOCK);

  // Todo: is this flawed?
  emit->current=procd->enter;

  // Note: emit all the parameters first ...
  cctree_t **list;
  ccarrfor(tree->type->list,list)
    ccemit_param(emit,procd,emit->current,*list);

  ccarrfor(tree->blob->list,list)
    ccemit_tree(emit,procd,emit->current,ccdref(list));

  ccblock_enter(procd->decls,procd->enter);
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
    	ccemit_procd_t *p=ccemit_global_procd(emit,decl,decl->name);
      ccemit_function(emit,p,decl);

      if(!strcmp(decl->name,"main"))
        emit->entry=p;

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
  cctree_solve_translation_unit(tree);

  cctree_t **decl;
  ccarrfor(tree->list,decl) ccemit_external_decl(emit,*decl);
}

ccfunc void
ccemit_init(ccemit_t *emit)
{ memset(emit,ccnil,sizeof(*emit));
}

#endif