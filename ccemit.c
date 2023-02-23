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

  ccesse_t *esse=ccseer_allusion(emit->seer,tree);
  ccassert(esse!=0);

  ccedict_t *e =is_param?
    ccedict_param(esse->type,tree->name):
    ccedict_local(esse->type,tree->name);

  ccvalue_t  *i=ccblock_add_edict(procd->decls,e);
  ccvalue_t **v=cctblputP(procd->local,tree);
  ccassert(ccerrnon());
  *v=i;
  return i;
}

// Todo:
ccfunc ccvalue_t *
ccemit_constant(ccemit_t *emit, cctype_t *type, ccclassic_t clsc)
{
  (void)emit;
  ccvalue_t *value=ccvalue("constant");
  value->kind=ccvalue_kCONST;
  value->constant.type=type;
  value->constant.clsc=clsc;
  return value;
}

// Todo:
ccfunc ccvalue_t *
ccemit_const_int(ccemit_t *emit, cci64_t value)
{ ccclassic_t classic;
  classic.asi64=value;
  return ccemit_constant(emit,ccnil,classic);
}

ccfunc ccvalue_t *
ccemit_const_str(ccemit_t *emit, ccstr_t value)
{
  ccclassic_t classic;
  classic.value=value;
  return ccemit_constant(emit,ccnil,classic);
}

ccfunc ccvalue_t *
ccemit_resolve(ccemit_t *emit, ccprocd_t *procd, cctree_t *tree)
{ ccassert(tree!=0);
  ccassert(tree->kind==cctree_kLITIDE);

  ccesse_t *esse=ccseer_allusion(emit->seer,tree);
  ccassert(esse!=0);

  // Note: this will happen until we proper implement scopes ...
  ccvalue_t *result=ccprocd_local(procd,esse->tree);
  ccassert(result!=0);

  return result;
}


ccfunc ccvalue_t *
ccemit_invoke_easy(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *tree)
{
  // Todo:
  cctree_t *ltree=tree->lval;
  cctree_t *rtree=tree->rval;

  ccassert(ltree->kind==cctree_kLITIDE);

  ccesse_t *esse=ccseer_allusion(emit->seer,ltree);
  switch(esse->builtin)
  { case ccbuiltin_kCCBREAK: return ccblock_dbgbreak(block);
    case ccbuiltin_kCCERROR: return ccblock_dbgerror(block);
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
  {
    *ccarrone(rval)=ccemit_rvalue(emit,func,block,list);
  }

  return ccblock_invoke(block,lval,rval);
}

ccfunc int
ccemit_variable_of_expression_is_array(ccemit_t *emit, cctree_t *tree)
{
  if(tree->kind==cctree_kLITIDE)
  { ccesse_t *allusion=ccseer_allusion(emit->seer,tree);
    ccassert(allusion!=0);

    ccassert(allusion->kind==ccesse_kVARIABLE);

    return allusion->type->kind==cctype_kARRAY;
  } else
  if(tree->kind==cctree_kINDEX)
  {
    return ccemit_variable_of_expression_is_array(emit,tree->lval);
  } else
  {
    ccassert(!"error");
    return 0;
  }
}

ccfunc ccvalue_t *
ccemit_index(ccemit_t *emit, ccprocd_t *procd, ccblock_t *block, cctree_t *tree, int is_rval)
{
  ccassert(tree->lval!=0);
  ccassert(tree->rval!=0);

  ccvalue_t *value;
  ccvalue_t *lvalue=ccemit_lvalue(emit,procd,block,tree->lval);
  ccvalue_t *rvalue=ccemit_rvalue(emit,procd,block,tree->rval);

  value=ccblock_aaddr(block,lvalue,rvalue);

  if(is_rval || !ccemit_variable_of_expression_is_array(emit,tree))
    value=ccblock_fetch(block,value,ccnull);

  return value;
}

ccfunc ccvalue_t *
ccemit_lvalue(ccemit_t *emit, ccprocd_t *procd, ccblock_t *block, cctree_t *tree)
{ ccvalue_t *value=ccnil;

  switch(tree->kind)
  { case cctree_kLITIDE:
      value=ccemit_resolve(emit,procd,tree); break;
    case cctree_kINDEX:
      value=ccemit_index(emit,procd,block,tree,ccfalse); break;
    default:
      ccassert(!"internal");
  }
  return value;
}

ccfunc ccvalue_t *
ccemit_rvalue(ccemit_t *emit, ccprocd_t *procd, ccblock_t *block, cctree_t *tree)
{ ccvalue_t *value=ccnil;
  switch(tree->kind)
  { case cctree_kLITIDE:
    { value=ccblock_fetch(block,ccemit_resolve(emit,procd,tree),ccnil);
    } break;
    case cctree_kLITINT:
    { value=ccemit_const_int(emit,tree->as_i32);
    } break;
    case cctree_kLITSTR:
    { value=ccemit_const_str(emit,tree->as_str);
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
    case cctree_kCALL:
    { value=ccemit_invoke_easy(emit,procd,block,tree);
    } break;
    case cctree_kINDEX:
    { value=ccemit_index(emit,procd,block,tree,cctrue);
    } break;
    case cctree_kUNARY:
    { ccassert(tree->rval!=ccnil);
      // int ?=&b;
      //       ^^
      if(tree->oper==cctoken_kADDRESSOF)
      { value=ccblock_laddr(block,ccemit_lvalue(emit,procd,block,tree->rval));
      } else
        ccassert(!"internal");
    } break;
    default: ccassert(!"internal");
  }
  return value;
}

ccfunc ccvalue_t *
ccemit_decl_name(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *tree)
{
  ccvalue_t *lval=ccemit_include_local(emit,func,block,tree,ccfalse);

  if(tree->init)
    ccblock_store(block,lval,ccemit_rvalue(emit,func,block,tree->init));

  return lval;
}

ccfunc ccinle void
ccemit_decl(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *decl)
{ cctree_t **list;
  ccarrfor(decl->list,list) ccemit_decl_name(emit,func,block,*list);
}

ccfunc ccvalue_t *
ccemit_param(ccemit_t *emit, ccprocd_t *func, ccblock_t *block, cctree_t *tree)
{
  return ccemit_include_local(emit,func,block,tree,cctrue);
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
ccemit_external_decl(ccemit_t *emit, ccesse_t *esse)
{
  ccassert(esse!=0);

  // Todo:
  if(esse->builtin!=ccbuiltin_kINVALID)
    return;
  if(!(esse->tree->mark&cctree_mEXTERNAL))
    return;

  ccassert(esse->tree!=0);
  ccassert(esse->type!=0);
  ccassert(esse->name!=0);

  if(esse->kind==ccesse_kFUNCTION)
  {
    ccprocd_t *p=ccmalloc_T(ccprocd_t);
    memset(p,0,sizeof(*p));

    p->esse=esse;
    p->label=esse->name;

    // Todo:
    *ccarradd(p->block,1)=p->decls=ccblock("$decls");
    *ccarradd(p->block,1)=p->enter=ccblock("$enter");
    *ccarradd(p->block,1)=p->leave=ccblock("$leave");

    // Todo:
    ccvalue_t *v=ccemit_include_global(emit,esse->name);
    v->kind=ccvalue_kPROCD;
    v->procd=p;

    // Todo: is this flawed?
    emit->current=p->enter;

    // Note: emit all the parameters first ...
    cctree_t **list;
    ccarrfor(esse->tree->type->list,list)
      ccemit_param(emit,p,emit->current,*list);

    ccarrfor(esse->tree->blob->list,list)
      ccemit_tree(emit,p,emit->current,*list);

    ccblock_jump(p->decls,ccblock_label_ex(p->enter,0,".E"));

    if(!strcmp(esse->tree->name,"main")) emit->entry=v;

  } else
    ccassert(!"error");
}

ccfunc void
ccemit_translation_unit(ccemit_t *emit, ccseer_t *seer)
{
  emit->seer=seer;

  ccesse_t **tale;
  ccarrfor(seer->entity_tale,tale)
    ccemit_external_decl(emit,*tale);


  ccassert(emit->entry!=0);
}

ccfunc void
ccemit_init(ccemit_t *emit)
{
  memset(emit,ccnil,sizeof(*emit));
}

#endif