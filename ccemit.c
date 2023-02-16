// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCEMIT
#define _CCEMIT

ccfunc ccvalue_t *
ccemit_lvalue(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *tree);

ccfunc ccvalue_t *
ccemit_rvalue(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *tree);


// Todo: cctree_to_type will not work with variables, this is because variables are not constant,
// and they are not generated before the decl block adds all the locals, so when a local array
// is added, no other instructions will be present ...

// Todo: this is temporary ...
ccfunc ccinle cctype_t *
cctree_to_type(ccemit_t *emit, ccprocd_t *procd, ccblock_t *block, cctree_t *tree)
{
  cctype_t *type=ccnil;

  if(tree->kind==cctree_kFUNC)
  { type=cctype(cctype_kPROCD,"procd");

    type->type=cctree_to_type(emit,procd,block,tree->rval);

    cctree_t **l;
    ccarrfor(tree->list,l)
    {
      // Todo:
      cctype_t *t=ccarrone(type->list);
      *t=*cctree_to_type(emit,procd,block,*l);
      t->label=(*l)->name;
    }

  } else
  if(tree->kind==cctree_kARRAY)
  {
    type=cctype(cctype_kARRAY,"array");
    type->type=cctree_to_type(emit,procd,block,tree->type);

    ccvalue_t *v=ccemit_rvalue(emit,procd,block,tree->rval);
    type->length=v;

  } else
  if(tree->kind==cctree_kTYPENAME)
  { type=cctype(cctype_kINTEGER,"integer");
  } else
    ccassert(!"error");
  return type;
}

ccfunc ccvalue_t *
ccemit_include_local(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *tree, int is_param)
{
  ccassert(tree!=0);
  ccassert(tree->kind==cctree_kDECLNAME);

  cctype_t *type=cctree_to_type(emit,func,block,tree->type);
  ccassert(type!=ccnil);

  ccedict_t *e =is_param?
    ccedict_param(type,tree->name):
    ccedict_local(type,tree->name);

  ccvalue_t  *i=ccblock_add_edict(func->decls,e);
  ccvalue_t **v=cctblputP(func->local,tree);
  ccassert(ccerrnon());
  *v=i;
  return i;
}

ccfunc ccvalue_t *
ccemit_constant(ccemit_t *emit, cctype_t *type, ccclassic_t clsc)
{
  (void)emit;
  ccvalue_t *value=ccmalloc_T(ccvalue_t);
  memset(value,ccnil,sizeof(*value));

  value->kind=ccvalue_kCONST;
  value->constant.type=type;
  value->constant.clsc=clsc;
  return value;
}

// Todo:
ccfunc ccvalue_t *
ccemit_const_i32(ccemit_t *emit, cci64_t value)
{
  ccclassic_t classic={value};
  return ccemit_constant(emit,ccnil,classic);
}

ccfunc ccvalue_t *
ccemit_resolve(ccemit_t *emit, ccprocd_t *func, cctree_t *tree)
{ ccassert(tree!=0);
  ccassert((tree->kind==cctree_kLITIDE)||
           (tree->kind==cctree_kINDEX));

  cctree_t *couple=ccseer_allusion(emit->seer,tree);
  ccassert(couple!=0);

  // Note: this will happen until we implement scopes ...
  ccvalue_t *result=ccprocd_local(func,couple);
  ccassert(result!=0);
  return result;
}


ccfunc ccvalue_t *
ccemit_invoke_easy(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *tree)
{
  cctree_t *ltree=tree->lval;
  cctree_t *rtree=tree->rval;

  ccassert(ltree->kind==cctree_kLITIDE);

  // Todo:
  (void)ltree;

  // Todo: this is the dumbest thing ever ...
  if(!strcmp(tree->name,"ccbreak"))
  { return ccblock_dbgbreak(block);
  } else
  if(!strcmp(tree->name,"ccerror"))
  { return ccblock_dbgerror(block);
  }

#if 0
  cctree_t *allude=ccseer_allusion(emit->seer,tree);
  ccassert(allude!=0);

  ccvalue_t  *lval=ccemit_global(emit,allude->name);
  ccvalue_t **rval=ccnil;
#endif
  ccvalue_t  *lval=ccemit_global(emit,tree->name);
  ccvalue_t **rval=ccnil;


  cctree_t *list;
  ccarrfor(rtree,list)
    *ccarrone(rval)=ccemit_rvalue(emit,func,block,list);

  return ccblock_invoke(block,lval,rval);
}

ccfunc ccvalue_t *
ccemit_lvalue(ccemit_t *emit, ccprocd_t *procd, ccblock_t *block, cctree_t *tree)
{ ccvalue_t *value=ccnil;
  switch(tree->kind)
  { case cctree_kLITIDE:
    { value=ccemit_resolve(emit,procd,tree);
    } break;

    case cctree_kINDEX:
    { ccassert(tree->lval!=ccnil);
      ccassert(tree->rval!=ccnil);

      ccvalue_t *lval,*rval;
      lval=ccemit_lvalue(emit,procd,block,tree->lval);
      rval=ccemit_rvalue(emit,procd,block,tree->rval);

      value=ccblock_address(block,lval,rval);
    } break;

    default: ccassert(!"internal");
  }
  return value;
}

ccfunc ccvalue_t *
ccemit_rvalue(ccemit_t *emit, ccprocd_t *procd, ccblock_t *block, cctree_t *tree)
{ ccvalue_t *value=ccnil;
  switch(tree->kind)
  { case cctree_kLITINT:
    { value=ccemit_const_i32(emit,tree->as_i32);
    } break;
    case cctree_kBINARY:
    { if(tree->oper==cctoken_kASSIGN)
      { ccvalue_t *lval=ccemit_lvalue(emit,procd,block,tree->lval);
        ccvalue_t *rval=ccemit_rvalue(emit,procd,block,tree->rval);
        value=ccblock_store(block,lval,rval);
      } else
      if(tree->oper==cctoken_kCMA)
      { ccemit_rvalue(emit,procd,block,tree->lval);
        ccemit_rvalue(emit,procd,block,tree->rval);
      } else
      { ccvalue_t *lhs=ccemit_rvalue(emit,procd,block,tree->lval);
        ccvalue_t *rhs=ccemit_rvalue(emit,procd,block,tree->rval);
        value=ccblock_arith(block,tree->oper,lhs,rhs);
      }
    } break;
    case cctree_kLITIDE:
    { value=ccblock_fetch(block,ccemit_resolve(emit,procd,tree),ccnil);
    } break;
    case cctree_kCALL:
    { value=ccemit_invoke_easy(emit,procd,block,tree);
    } break;
    case cctree_kINDEX:
    { ccassert(tree->lval!=ccnil);
      ccassert(tree->rval!=ccnil);

      ccvalue_t *lval,*rval;
      lval=ccemit_lvalue(emit,procd,block,tree->lval);
      rval=ccemit_rvalue(emit,procd,block,tree->rval);

      value=ccblock_fetch(block,lval,rval);
    } break;
    default: ccassert(!"internal");
  }
  return value;
}

ccfunc ccvalue_t *
ccemit_tree(
  ccemit_t *emit, ccprocd_t *func, ccblock_t *irset, cctree_t *tree);

ccfunc void
ccemit_treelist(
  ccemit_t *emit, ccprocd_t *func, ccblock_t *irset, cctree_t **list)
{
  cctree_t **it;
  ccarrfor(list,it) ccemit_tree(emit,func,irset,*it);
}

ccfunc ccvalue_t *
ccemit_decl_name(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *tree)
{ ccvalue_t *lval=ccemit_include_local(emit,func,block,tree,ccfalse);

  if(tree->init)
  { ccvalue_t *rval=ccemit_rvalue(emit,func,block,tree->init);
    ccblock_store(block,lval,rval);
  }
  return lval;
}

ccfunc ccinle void
ccemit_decl(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *decl)
{ cctree_t **list;
  ccarrfor(decl->list,list) ccemit_decl_name(emit,func,block,*list);
}

ccfunc ccvalue_t *
ccemit_param(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *tree)
{ ccvalue_t *lval=ccemit_include_local(emit,func,block,tree,cctrue);
  return lval;
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
    ccjump_point_t p;

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
ccemit_function(ccemit_t *emit, ccprocd_t *procd, cctree_t *tree)
{
  ccassert(tree!=0);

  ccassert(tree->type!=0);
  ccassert(tree->type->kind==cctree_kFUNC);

  ccassert(tree->blob!=0);
  ccassert(tree->blob->kind==cctree_kBLOCK);

  // Todo: is this flawed?
  emit->current=procd->enter;

  // Note: emit all the parameters first ...
  cctree_t **list;
  ccarrfor(tree->type->list,list)
    ccemit_param(emit,procd,emit->current,*list);

  ccarrfor(tree->blob->list,list)
    ccemit_tree(emit,procd,emit->current,ccdref(list));

  ccblock_jump(procd->decls,ccblock_label_ex(procd->enter,0,".E"));
}


ccfunc void
ccemit_external_decl(ccemit_t *emit, cctree_t *tree)
{
  cctree_t **decl_;
  cctree_t  *decl;

  ccarrfor(tree->list,decl_)
  {
    decl=*decl_;

    if(decl->type->kind==cctree_kFUNC)
    {
      ccvalue_t *v=ccemit_global_procd(emit,decl,decl->name);
      ccemit_function(emit,v->procd,decl);

      if(!strcmp(decl->name,"main"))
        emit->entry=v;

    } else
    {
      // Todo:
      ccassert(!"error");
    }
  }
}

ccfunc void
ccemit_translation_unit(ccemit_t *emit, ccseer_t *seer, cctree_t *tree)
{
	emit->seer=seer;

  cctree_t **decl;
  ccarrfor(tree->list,decl)
  	ccemit_external_decl(emit,*decl);
}

ccfunc void
ccemit_init(ccemit_t *emit)
{
	memset(emit,ccnil,sizeof(*emit));
}

#endif