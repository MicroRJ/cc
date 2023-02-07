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
ccemit_value_const(cctype_t *type, ccclassic_t clsc)
{ ccemit_value_t value={};
  value.kind=ccvalue_kCONST;
  value.constant.type=type;
  value.constant.clsc=clsc;
  return value;
}

ccfunc ccemit_value_t
ccemit_value_const_i32(cci32 val)
{
  return ccemit_value_const(ctype_int32,{val});
}



ccfunc ccemit_value_t *
ccvm_value_init(ccemit_value_t *value, ccvalue_K kind, const char *name)
{ memset(value,ccnil,sizeof(*value));
  value->kind=kind;
  value->name=(ccstr_t)name;
  return value;
}

ccfunc ccemit_value_t *
ccemit_constant(ccemit_t *emit, cctype_t *type, ccclassic_t clsc)
{
  (void)emit;
  ccemit_value_t *value=ccmalloc_T(ccemit_value_t);
  value->kind=ccvalue_kCONST;
  value->constant.type=type;
  value->constant.clsc=clsc;
  return value;
}

ccfunc ccemit_value_t *
ccemit_edict_value(ccemit_t *emit, ccedict_t *edict)
{ (void)emit;
  ccemit_value_t *value=ccmalloc_T(ccemit_value_t);
  *value=ccemit_value_edict(edict);
  return value;
}

// warren - "a densely populated or labyrinthine building or district"

// Todo:
ccfunc ccemit_value_t *
ccemit_const_i32(ccemit_t *emit, cci64 value)
{
  ccclassic_t classic={value};
  return ccemit_constant(emit,ctype_int32,classic);
}

ccfunc ccblock_t *
ccblock_ini(ccblock_t *block, ccblock_t *super, const char *debug_label)
{ memset(block,ccnil,sizeof(*block));
  // Todo:
  block->local=ccnil;

  ccarrres(block->instr,0xff);
  ccarrfix(block->instr);

  block->super=super;
  block->debug_label=debug_label;
  return block;
}

ccfunc ccblock_t *
ccvm_block(ccblock_t *super, const char *debug_label)
{
  ccblock_t *block=(ccblock_t *)ccmalloc(sizeof(*block));
  return ccblock_ini(block,super,debug_label);
}

ccfunc ccblock_t *
ccemit_label(ccblock_t *irset, const char *name)
{
  int ait;
  ccemit_value_t *value=cctblsetS(irset->local,name,&ait);

  if(!ait)
  { value->kind=ccvalue_Kblock;
    value->block=ccvm_block(irset,name);
  }
  return value->block;
}

ccfunc ccedict_t *
ccemit_edict(ccblock_t *block)
{
  ccedict_t *res=ccarradd(block->instr,1);
  memset(res,0,sizeof(*res));
  return res;
}

ccfunc ccedict_t *
ccemit_local(ccblock_t *block, cctype_t *type, const char *name)
{
  ccedict_t *i=ccemit_edict(block);
  i->kind=ccedict_kLOCAL;
  i->local.type=type;
  // Todo:
  i->local.debug_label=ccnil;
  ccstrputS((char*)i->local.debug_label,name);

  int already_in_table;
  ccemit_value_t *v=cctblputS(block->local,name,&already_in_table);
  v->kind=ccvalue_kEDICT;
  v->edict=i;

  ccassert(!already_in_table);

  return i;
}

ccfunc ccedict_t *
ccemit_store(ccblock_t *block, ccedict_t *adr, ccemit_value_t *val)
{ ccedict_t *i=ccemit_edict(block);
  i->kind=ccedict_kSTORE;
  i->store.adr=adr;
  i->store.val=val;
  return i;
}

ccfunc ccedict_t *
ccemit_load(ccblock_t *block, ccedict_t *adr)
{ ccedict_t *i=ccemit_edict(block);
  i->kind=ccedict_kLOAD;
  i->load.adr=adr;
  return i;
}

ccfunc ccedict_t *
ccemit_binary(ccblock_t *block, cctoken_k opr, ccemit_value_t *lhs, ccemit_value_t *rhs)
{ ccedict_t *i=ccemit_edict(block);
  i->kind=ccedict_kBINARY;
  i->binary.opr=opr;
  i->binary.lhs=lhs;
  i->binary.rhs=rhs;
  return i;
}

ccfunc ccedict_t *
ccemit_condi(ccblock_t *block, ccemit_value_t *cnd, ccblock_t *then_blc, ccblock_t *else_blc)
{ ccedict_t *i=ccemit_edict(block);
  i->kind=ccedict_Kcondi;
  i->condi.cnd=cnd;
  i->condi.then_blc=then_blc;
  i->condi.else_blc=else_blc;
  return i;
}

ccfunc ccedict_t *
ccemit_enter(ccblock_t *block, ccblock_t *blc)
{ ccedict_t *i=ccemit_edict(block);
  i->kind=ccedict_kENTER;
  i->enter.blc=blc;
  return i;
}

ccfunc ccedict_t *
ccemit_return(ccblock_t *irset)
{ ccedict_t *i=ccemit_edict(irset);
  i->kind=ccedict_Kreturn;
  return i;
}

ccfunc ccedict_t *
ccemit_call(ccblock_t *block, ccemit_value_t *value)
{
  (void)block;
  (void)value;

  return ccnil;
}

ccfunc ccemit_value_t *
ccsvm_resolve_identifier(ccemit_t *emit, ccfunction_t *func, const char *iden)
{
  ccemit_value_t *value=ccnil;

  int in_table;
  value=cctblgetS(func->decls->local,iden,&in_table);
  if(!in_table)
    value=cctblgetS(emit->globals,iden,&in_table);

  return in_table?value:ccnil;
}

ccfunc ccemit_value_t *
ccsvm_resolvevalue(ccemit_t *emit, ccfunction_t *func, ccblock_t *irset, cctree_t *tree)
{ ccemit_value_t *value=ccnil;
  if(tree->kind==cctree_kBINARY)
  {
    ccemit_value_t *lhs=ccsvm_resolvevalue(emit,func,irset,tree->binary.lhs);
    ccemit_value_t *rhs=ccsvm_resolvevalue(emit,func,irset,tree->binary.rhs);

    return ccemit_edict_value(emit,ccemit_binary(irset,tree->binary.opr.bit,lhs,rhs));
  } else
  if(tree->kind==cctree_kIDENTIFIER)
  {
    return ccsvm_resolve_identifier(emit,func,tree->constant.token.str);
  } else
  if(tree->kind==cctree_kINTEGER)
  { value=ccemit_const_i32(emit,tree->constant.token.sig);
  }
  return value;
}

ccfunc ccemit_value_t
ccemit_tree(
  ccemit_t *emit, ccfunction_t *func, ccblock_t *irset, cctree_t *tree);

ccfunc void
ccemit_treelist(
  ccemit_t *emit, ccfunction_t *func, ccblock_t *irset, cctree_t *list)
{
  cctree_t *it;
  ccarrfor(list,it) ccemit_tree(emit,func,irset,it);
}

// stardom - "the state or status of being a famous or exceptionally talented performer in the world of entertainment or sports"

ccfunc ccemit_value_t
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
  { irset=ccemit_label(irset,tree->label_name);
    ccemit_treelist(emit,func,irset,tree->stmt_list);
    return ccemit_value_edict(ccemit_enter(irset,irset));
  } else
  if(tree->kind==cctree_kRETRN)
  { return ccemit_value_edict(ccemit_return(irset));
  } else
  if(tree->kind==cctree_Kgoto_statement)
  { ccblock_t *label_block=ccemit_label(irset,tree->label_name);
    return ccemit_value_edict(ccemit_enter(irset,label_block));
  } else
  if(tree->kind==cctree_kINTEGER)
  {
    return ccemit_value_const_i32((cci32)tree->constant.token.sig);
  } else
  if(tree->kind==cctree_Kcall_expr)
  {
#if 0
    ccstr_t func_name=tree->expr_tree->constant.token.str;
    int in_table;
    ccemit_value_t *func_value=cctblgetS(emit->globals,func_name,&in_table);
    if(in_table)
    { ccemit_call(irset,func_value);
    } else cctraceerr("undefined function");
#endif
  } else
  if(tree->kind==cctree_kBINARY)
  {
    cctree_t *lhs,*rhs;
    lhs=tree->binary.lhs; ccnotnil(lhs!=0);
    rhs=tree->binary.rhs; ccnotnil(rhs!=0);

    ccemit_value_t *lval,*rval;
    lval=ccnil;
    rval=ccnil;
#if 0
    lval=ccsvm_resolvevalue(emit,func,irset,lhs); ccnotnil(lval!=0);
    rval=ccsvm_resolvevalue(emit,func,irset,rhs); ccnotnil(rval!=0);
#endif
    lval=ccsvm_resolve_identifier(emit,func,lhs->constant.token.str);

    if(rhs->kind==cctree_kINTEGER)
    { rval=ccemit_const_i32(emit,rhs->constant.token.sig);
    }

    ccnotnil(lval);
    ccnotnil(rval);


    cctoken_t tok=tree->binary.opr;

    if(tok.bit==cctoken_kASSIGN)
    {
#if 0
      return ccemit_value_edict(ccemit_store(irset,lval,rval));
#endif
    } else
    {
      return ccemit_value_edict(ccemit_binary(irset,tok.bit,lval,rval));
    }

  } else
  if(tree->kind==cctree_Kwhile_statement)
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
  if(tree->kind==cctree_Kconditional_statement)
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
ccemit_rvalue(ccemit_t *emit, ccfunction_t *func, ccblock_t *block, ccedict_t *lval, cctree_t *rtree)
{
  ccemit_value_t *value=ccnil;
  switch(rtree->kind)
  {
    case cctree_kINTEGER:
    {
      value=ccemit_const_i32(emit,rtree->constant.token.sig);
    } break;
    case cctree_kBINARY:
    { value=
        ccemit_edict_value(emit,
          ccemit_binary(block,rtree->binary.opr.bit,
            ccemit_rvalue(emit,func,block,lval,rtree->binary.lhs),
            ccemit_rvalue(emit,func,block,lval,rtree->binary.rhs)));
    } break;
    case cctree_kIDENTIFIER:
    {
      ccemit_value_t *raddr;
      raddr=
        ccsvm_resolve_identifier(emit,func,rtree->constant.token.str);
      value=
        ccemit_edict_value(emit,ccemit_load(block,raddr->edict));
    } break;
  }
  return value;
}

ccfunc void
ccemit_decl_name(ccemit_t *emit, ccfunction_t *func, cctree_t *decl)
{
  ccnotnil(decl->decl_type);
  ccnotnil(decl->decl_name);
  ccassert(!decl->decl_size);
  ccassert(!decl->decl_list);

  ccedict_t *local=ccemit_local(func->decls,decl->decl_type,decl->decl_name);
  ccemit_value_t *value=ccemit_rvalue(emit,func,func->enter,local,decl->decl_init);

  ccemit_store(func->enter,local,value);
}

ccfunc void
ccemit_decl(ccemit_t *emit, ccfunction_t *func, cctree_t *decl)
{
  ccnotnil(decl);
  ccassert(decl->kind==cctree_kDECL);
  ccnotnil(decl->decl_type);
  ccnotnil(decl->decl_list);
  ccassert(!decl->decl_name);
  ccassert(!decl->decl_size);

  cctree_t *decl_list;
  ccarrfor(decl->decl_list,decl_list)
  {
    ccemit_decl_name(emit,func,decl_list);
  }
}

ccfunc ccfunction_t *
ccemit_function(ccemit_t *emit, cctype_t *type, const char *name, cctree_t *body)
{
  ccnotnil(type);
  ccassert(type->kind==cctype_Kfunc);

  ccfunction_t *func=ccmalloc_T(ccfunction_t);
  memset(func,ccnil,sizeof(*func));
  func->debug_label=name;
  func->type=type;

  // Todo:
  ccarrres(func->block,0xff);
  func->decls=ccarradd(func->block,1);
  func->enter=ccarradd(func->block,1);
  func->leave=ccarradd(func->block,1);

  ccnotnil(func);
  ccnotnil(func->block);
  ccnotnil(func->decls);
  ccnotnil(func->enter);
  ccnotnil(func->leave);

  ccblock_ini(func->decls,ccnil,"$decls");
  ccblock_ini(func->enter,ccnil,"$enter");
  ccblock_ini(func->leave,ccnil,"$leave");

  cctree_t *decl_name;
  ccarrfor(type->list,decl_name)
  { ccassert(!decl_name->decl_init);
    ccemit_decl_name(emit,func,decl_name);
  }

  ccassert(body->kind==cctree_kBLOCK);
  cctree_t *stmt;
  ccarrfor(body->stmt_list,stmt)
  {
    if(stmt->kind==cctree_kDECL)
    {
      ccemit_decl(emit,func,stmt);
    } else
    {
      ccemit_tree(emit,func,func->enter,stmt);
    }
  }
  ccemit_enter(func->decls,func->enter);

  return func;
}

ccfunc void
ccemit_translation_unit(ccemit_t *emit, cctree_t *tree)
{
  for(cctree_t *decl=tree; decl<ccarrend(tree); decl++)
  {
    if(decl->kind!=cctree_kDECL)
    {
      cctraceerr("invalid external declaration");
      continue;
    }

    ccnotnil(decl->decl_list);
    ccnotnil(decl->decl_type);

    for(cctree_t *decl_name=decl->decl_list;decl_name<ccarrend(decl->decl_list);++decl_name)
    {
      ccassert(decl_name->kind==cctree_Kdecl_name);
      ccnotnil(decl_name->decl_type);
      ccnotnil(decl_name->decl_name);
      ccnotnil(!decl_name->decl_size);

      if(decl_name->decl_type->kind==cctype_Kfunc)
      {
        ccfunction_t *func=ccemit_function(emit,decl_name->decl_type,decl_name->decl_name,decl_name->body_tree);

        int already_in_table;
        ccemit_value_t *value=cctblputS(emit->globals,decl_name->decl_name,&already_in_table);
        if(already_in_table) cctraceerr("function re-definition");

        value->name=decl_name->decl_name;
        value->kind=ccvalue_kFUNC;
        value->function=func;
      } else
      { ccnotnil(decl_name->decl_type);
        ccnotnil(decl_name->decl_name);
        ccassert(!decl_name->decl_size);
        ccassert(!decl_name->decl_list);
        ccassert(!decl_name->decl_init);

        int already_in_table;
        ccemit_value_t *global=cctblputS(emit->globals,decl_name->decl_name,&already_in_table);
        if(already_in_table) cctraceerr("variable re-definition");
        (void) global;

      }
    }
  }
}

ccfunc void
ccemit_init(ccemit_t *emit)
{ memset(emit,ccnil,sizeof(*emit));

  emit->current=ccvm_block(ccnil,"main");
  emit->curirix=0;
}



// typedef enum ccedict_K ccedict_K;
// typedef enum ccvalue_K ccvalue_K;
// typedef struct ccemit_value_t ccemit_value_t;
// typedef struct ccedict_t ccedict_t;
// typedef struct ccblock_t ccblock_t;
// typedef struct ccfunction_t ccfunction_t;
// typedef struct ccvm_frame_t ccvm_frame_t;

// typedef enum ccedict_K
// { ccedict_kSTORE = 0,
//   ccedict_kBINARY,
//   ccedict_Kblock,
//   ccedict_Kcondi,
//   ccedict_kENTER,
//   ccedict_Kleave,
// } ccedict_K;
// typedef enum ccvalue_K
// { ccvalue_Kleaf = 0,
//   ccvalue_kGLOBAL,
//   ccvalue_Kblock,
//   ccvalue_kEDICT,
// } ccvalue_K;
// typedef struct ccemit_value_t
// { ccvalue_K    kind;
//   ccstr_t         name;

//   // Todo: replace leaf with an actual pointer, should be address instead ...
//   // Real store/load instructions ...
//   cctype_t       *type;
//   cctoken_t       leaf;

//   ccblock_t    *block;
//   ccedict_t    *instr;
//   ccfunction_t  *routine;
// } ccemit_value_t;
// typedef struct ccedict_t
// { ccedict_K  type;
//   ccemit_value_t *res;

//   // Todo: make union ...
//   cctoken_t     oper;
//   ccemit_value_t *lhs;
//   ccemit_value_t *rhs;

//   ccemit_value_t *condi;
//   ccblock_t *block[4];
// } ccedict_t;
// // Note: a block itself does not have a label and thus may not be referenced, a value however, may have a label,
// // and it may be a block type ... labeled blocks are value<block> ...
// typedef struct ccblock_t
// { const char     *debug_label;
//   ccfunction_t *owner;
//   ccblock_t   *super;
//   ccemit_value_t   *local;
//   ccedict_t   *instr;
// } ccblock_t;
// typedef struct ccfunction_t
// { const char   *debug_label;
//  cctype_t     *type;
//  ccblock_t *block;
//  ccblock_t *enter;
//   ccblock_t *leave;
// } ccfunction_t;
// typedef struct ccvm_frame_t
// { const char     *debug_label;
//  ccvm_frame_t   *caller;
//  ccemit_value_t   *arguments;
//  ccfunction_t *routine;
//   ccblock_t   *prevblc;
//  ccblock_t   *currblc;
//   int             curirix;
// } ccvm_frame_t;
// typedef struct ccexec_t
// { ccemit_value_t   *globals;
//   ccfunction_t *routine;
//  ccvm_frame_t   *current;
// } ccexec_t;

// ccfunc ccstr_t ccvmir_tos(ccedict_t *ir);

// ccfunc ccemit_value_t *
// ccmapget(ccexec_t *vm, const char *name)
// {
//   ccemit_value_t *it;
//   ccarrfor(vm->globals,it)
//   {
//     if(strcmp(name,it->name)==0)
//     {
//       return it;
//     }
//   }
//   return ccnil;
// }

// ccfunc ccemit_value_t *
// ccvm_value_init(ccemit_value_t *value, ccvalue_K kind, const char *name)
// { memset(value,ccnil,sizeof(*value));
//   value->kind=kind;
//   value->name=(ccstr_t)name;
//   return value;
// }

// ccfunc ccblock_t *
// ccblock_ini(ccblock_t *block, ccblock_t *super, const char *debug_label)
// { memset(block,0,sizeof(*block));
//   // Todo:
//   ccarrres(block->local,0xff);
//   ccarrres(block->instr,0xff);
//   block->super=super;
//   block->debug_label=debug_label;
//   return block;
// }

// ccfunc ccemit_value_t *
// ccvm_global_ex(ccexec_t *vm, ccvalue_K kind, const char *name)
// { ccemit_value_t *value=ccarradd(vm->globals,1);
//   ccvm_value_init(value,kind,name);
//   return value;
// }

// ccfunc ccemit_value_t *
// ccvm_global(ccexec_t *vm, cctype_t *type, ccstr_t name)
// {
//   ccemit_value_t *val=ccvm_global_ex(vm,ccvalue_kGLOBAL,name);
//   val->type=type;
//   return val;
// }

// ccfunc ccemit_value_t *
// ccvm_local(ccblock_t *block, ccvalue_K kind, const char *name)
// {
//   ccemit_value_t *val=ccarradd(block->local,1);
//   ccvm_value_init(val,kind,name);
//   return val;
// }

// ccfunc ccblock_t *
// ccvm_block(ccblock_t *super, const char *debug_label)
// {
//   ccblock_t *block=(ccblock_t *)ccmalloc(sizeof(*block));
//   return ccblock_ini(block,super,debug_label);
// }

// ccfunc ccblock_t *
// ccemit_label(ccexec_t *vm, ccblock_t *super, const char *name)
// {
//   ccemit_value_t *value=ccmapget(vm,name);
//   ccblock_t *block=ccnil;

//   if(!value)
//   { block=ccvm_block(super,name);
//     value=ccvm_local(super,ccvalue_Kblock,name);
//     value->block=block;

//     // Todo:
//     ccemit_value_t *global=ccvm_global_ex(vm,ccvalue_Kblock,name);
//     global->block=block;
//   } else block=value->block; // ccassert(value->kind==ccvalue_Kblock);

//   return block;
// }

// ccfunc ccemit_value_t *
// ccvm_leafvalue(ccblock_t *block, cctoken_t leaf)
// { ccemit_value_t *value=ccvm_local(block,ccvalue_Kleaf,"$0");
//   value->leaf=leaf;
//   return value;
// }

// ccfunc ccemit_value_t *
// ccvm_intvalue(ccblock_t *block, signed long long int val)
// { ccemit_value_t *value=ccvm_local(block,ccvalue_Kleaf,"$0");
//   value->leaf.bit=cctoken_Kliteral_integer;
//   value->leaf.sig=val;
//   return value;
// }

// ccfunc ccemit_value_t *
// ccemit_edictvalue(ccblock_t *block, ccedict_t *instr)
// { ccemit_value_t *value=ccvm_local(block,ccvalue_kEDICT,"$0");
//   value->instr=instr;
//   return value;
// }

// ccfunc ccedict_t *
// ccemit_edict(ccblock_t *block)
// {
//   ccedict_t *res=ccarradd(block->instr,1);
//   memset(res,0,sizeof(*res));
//   return res;
// }

// ccfunc ccemit_value_t *
// ccemit_binary(ccblock_t *block, cctoken_t oper, ccemit_value_t *lhs, ccemit_value_t *rhs)
// { ccedict_t *i=ccemit_edict(block);
//   i->type=ccedict_kBINARY;
//   i->oper=oper;
//   i->lhs =lhs;
//   i->rhs =rhs;
//   return ccemit_edictvalue(block,i);
// }

// ccfunc ccemit_value_t *
// ccemit_store(ccblock_t *block, ccemit_value_t *lhs, ccemit_value_t *rhs)
// { ccedict_t *i=ccemit_edict(block);
//   i->type=ccedict_kSTORE;
//   i->lhs=lhs;
//   i->rhs=rhs;
//   return ccemit_edictvalue(block,i);
// }

// ccfunc ccemit_value_t *
// ccemit_condi(ccblock_t *irset, ccemit_value_t *check_value, ccblock_t *then_block, ccblock_t *else_block)
// { ccedict_t *i=ccemit_edict(irset);
//   i->type=ccedict_Kcondi;
//   i->condi=check_value;
//   i->block[0]=then_block;
//   i->block[1]=else_block;
//   return ccemit_edictvalue(irset,i);
// }

// ccfunc ccemit_value_t *
// ccemit_enter(ccblock_t *irset, ccblock_t *block)
// { ccedict_t *i=ccemit_edict(irset);
//   i->type=ccedict_Kblock;
//   i->block[0]=block;
//   return ccemit_edictvalue(irset,i);
// }

// ccfunc ccemit_value_t *
// ccemit_enter(ccblock_t *irset, ccblock_t *block)
// { ccedict_t *i=ccemit_edict(irset);
//   i->type=ccedict_kENTER;
//   i->block[0]=block;
//   return ccemit_edictvalue(irset,i);
// }

// ccfunc ccemit_value_t *
// ccemit_leave(ccblock_t *irset, ccblock_t *block)
// { ccedict_t *i=ccemit_edict(irset);
//   i->type=ccedict_Kleave;
//   i->block[0]=block;
//   return ccemit_edictvalue(irset,i);
// }

// ccfunc ccemit_value_t *
// ccsvm_resolvevalue(ccexec_t *vm, ccblock_t *irset, cctree_t *tree)
// { ccemit_value_t *value=ccnil;
//   if(tree->kind==cctree_kBINARY)
//   { ccemit_value_t *lhs=ccsvm_resolvevalue(vm,irset,tree->binary.lhs);
//     ccemit_value_t *rhs=ccsvm_resolvevalue(vm,irset,tree->binary.rhs);
//     return ccemit_binary(irset,tree->binary.opr,lhs,rhs);
//   } else
//   if(tree->kind==cctree_kIDENTIFIER)
//   { value=ccmapget(vm,tree->constant.token.str);
//   } else
//   if(tree->kind==cctree_kINTEGER)
//   { value=ccvm_leafvalue(irset,tree->constant.token);
//   }
//   return value;
// }

// ccfunc ccemit_value_t *
// ccemit_tree(ccexec_t *vm, ccblock_t *irset, cctree_t *tree)
// {
//   ccemit_value_t *result=ccnil;

//   if(tree->kind==cctree_kBLOCK)
//   {
//     // Note: aside from containing a set of instructions, blocks also contain local values ...
//     // this may change in the future ...
//     ccblock_t *child=ccvm_block(irset,"mixed");
//     cctree_t *it;
//     ccarrfor(tree->mix_statement.stat,it)
//     {
//       ccemit_tree(vm,child,it);
//     }
//     ccemit_enter(irset,child);
//   } else
//   if(tree->kind==cctree_kLABEL)
//   {
//     // Note: I'm expecting the parser to generate a hierarchy of labels, as supposed to a list,
//     // so after this label, there will be no more instructions ... therefore I don't have to switch the current block ...

//     irset=ccemit_label(vm,irset,tree->label_statement.name);

//     cctree_t *stat;
//     ccarrfor(tree->label_statement.list,stat)
//     {
//       ccemit_tree(vm,irset,stat);
//     }
//   } else
//   if(tree->kind==cctree_Kgoto_statement)
//   {
//     ccblock_t *label_block=ccemit_label(vm,irset,tree->goto_statement.name);
//     ccemit_enter(irset,label_block);
//   } else
//   if(tree->kind==cctree_kINTEGER)
//   {
//     result=ccvm_leafvalue(irset,tree->constant.token);
//   } else
//   if(tree->kind==cctree_kDECL)
//   {
//     cctree_t *it;
//    ccnotnil(tree->decl_name);

//     ccarrfor(tree->decl_name,it)
//     {
//      if(it->decl_name_type->kind==cctype_Kfunc)
//      {
//        ccemit_value_t *func_value=ccvm_global(vm,it->decl_name_type,it->decl_name_iden);
//        ccassert(it->body_tree);
//        func_value->block=ccvm_block(irset,"procedure::entry");
//        ccemit_tree(vm,func_value->block,it->body_tree);

//      } else
//      {
//        ccemit_value_t *address,*value=ccnil;
//        address=ccvm_global(vm,
//          it->decl_name_type,
//          it->decl_name_iden);

//        cctree_t *init=it->decl_name_init;
//        if(init)
//        { if(init->kind==cctree_kINTEGER)
//          { value=ccvm_leafvalue(irset,init->constant.token);
//          } else ccassert(!"error");
//          ccemit_store(irset,address,value);
//        }
//      }
//     }
//     // Note: don't return anything here ...
//   } else
//   if(tree->kind==cctree_kBINARY)
//   {
//     cctree_t *lhs,*rhs;
//     lhs=tree->binary.lhs; ccnotnil(lhs!=0);
//     rhs=tree->binary.rhs; ccnotnil(rhs!=0);

//     ccemit_value_t *lval,*rval;
//     lval=ccsvm_resolvevalue(vm,irset,lhs);
//     rval=ccsvm_resolvevalue(vm,irset,rhs);

//     ccnotnil(lval!=0);
//     ccnotnil(rval!=0);
//     cctoken_t tok=tree->binary.opr;
//     if(tok.bit==cctoken_kASSIGN)
//     { return ccemit_store(irset,lval,rval);
//     } else
//     { return ccemit_binary(irset,tok,lval,rval);
//     }
//   } else
//   if(tree->kind==cctree_Kwhile_statement)
//   { ccblock_t *cond_block=ccvm_block(irset,"$while::cond");
//     ccblock_t *then_block=ccvm_block(cond_block,"$while::then");
//     ccblock_t *else_block=ccvm_block(irset,"$while::else");
//     ccemit_value_t *cond_value=ccemit_tree(vm,cond_block,tree->cond_tree);
//     ccemit_condi(cond_block,cond_value,then_block,else_block);
//     if(tree->then_tree) ccemit_tree(vm,then_block,tree->then_tree);
//     ccemit_enter(then_block,cond_block);
//     ccemit_enter(irset,cond_block);

//     vm->current=else_block;
//     vm->curirix=0;
//   } else
//   if(tree->kind==cctree_Kconditional_statement)
//   { ccemit_value_t *cond_value=ccemit_tree(vm,irset,tree->cond_tree);
//     ccblock_t *then_block=ccnil,*else_block=ccnil;
//     ccblock_t *done_block=ccvm_block(irset,"$local");
//     if(tree->then_tree)
//     { then_block=ccvm_block(irset,"$if::then");
//       ccemit_tree(vm,then_block,tree->then_tree);
//       ccemit_enter(then_block,done_block);
//     }
//     if(tree->else_tree)
//     { irset=else_block=ccvm_block(irset,"$if::else");
//       ccemit_tree(vm,else_block,tree->else_tree);
//       ccemit_enter(else_block,done_block);
//     }
//     ccemit_condi(irset,cond_value,then_block,else_block);
//     vm->current=done_block;
//     vm->curirix=0;
//   } else
//   { ccassert(!"error");
//   }

//   return result;
// }

// ccfunc ccemit_value_t *
// ccsvm_execbinary(ccblock_t *irset, cctoken_t oper, ccemit_value_t *lval, ccemit_value_t *rval)
// {
//   if(!lval) cctraceerr("undefined lhs value"); // Todo: to string ...
//   if(!rval) cctraceerr("undefined rhs value"); // Todo: to string ...

//   ccassert(lval->kind==ccvalue_Kleaf||lval->kind==ccvalue_kGLOBAL);
//   ccassert(rval->kind==ccvalue_Kleaf||rval->kind==ccvalue_kGLOBAL);

//   if(oper.bit==cctoken_kASSIGN)
//   { lval->leaf=rval->leaf;
//     return lval;
//   } else
//   if(oper.bit==cctoken_Kequals)
//   { return ccvm_intvalue(irset,lval->leaf.sig==rval->leaf.sig);
//   } else
//   if(oper.bit==cctoken_Kgreater_than)
//   { return ccvm_intvalue(irset,lval->leaf.sig>rval->leaf.sig);
//   } else
//   if(oper.bit==cctoken_Kgreater_than_eql)
//   { return ccvm_intvalue(irset,lval->leaf.sig>=rval->leaf.sig);
//   } else
//   if(oper.bit==cctoken_Kless_than)
//   { return ccvm_intvalue(irset,lval->leaf.sig<rval->leaf.sig);
//   } else
//   if(oper.bit==cctoken_Kless_than_eql)
//   { return ccvm_intvalue(irset,lval->leaf.sig<=rval->leaf.sig);
//   } else
//   if(oper.bit==cctoken_Kadd)
//   { return ccvm_intvalue(irset,lval->leaf.sig+rval->leaf.sig);
//   } else
//   { ccassert(!"error");
//     return ccnil;
//   }
// }

// ccfunc void
// ccvm_enter(ccexec_t *vm, ccblock_t *block)
// {
//   ccnotnil(block);

//   vm->prevblc=block;
//   vm->current=block;
//   vm->curirix=0;
// }

// ccfunc void
// ccvm_leave(ccexec_t *vm, ccblock_t *block)
// {
//   ccnotnil(block);

//   vm->current=block->super;
// }

// ccfunc void
// ccvm_exec_instr(ccexec_t *vm, ccblock_t *irset, ccedict_t *instr)
// {
//   ccvmir_tos(instr);

//   switch(instr->type)
//   { case ccedict_Kblock:
//     { ccedict_t *it;
//       ccarrfor(instr->block[0]->instr,it)
//       {
//         ccvm_exec_instr(vm,irset,it);
//       }
//     } break;
//     case ccedict_kSTORE:
//     { ccemit_value_t *lhs,*rhs;
//       lhs=instr->lhs;
//       rhs=instr->rhs;
//       ccassert(lhs->kind==ccvalue_kGLOBAL);
//       if(rhs->kind==ccvalue_Kleaf)
//       { lhs->leaf=rhs->leaf;
//       } else
//       if(rhs->kind==ccvalue_kEDICT)
//       { lhs->leaf=rhs->instr->res->leaf;
//       }
//       instr->res=lhs;
//     } break;
//     case ccedict_kENTER:
//     { ccvm_enter(vm,instr->block[0]);
//     } break;
//     case ccedict_Kcondi:
//     {
//       if(instr->condi->instr->res->leaf.sig)
//       {
//         ccvm_enter(vm,instr->block[0]);
//       } else
//       {
//         ccvm_enter(vm,instr->block[1]);
//       }
//     } break;
//     case ccedict_kBINARY:
//     {
//       instr->res=ccsvm_execbinary(irset,instr->oper,instr->lhs,instr->rhs);
//     } break;
//     default: ccassert(!"error");
//   }
// }

// ccfunc void
// ccvm_exit(ccexec_t *vm)
// {
//   vm->current=ccnil;
// }

// ccfunc void
// ccvm_exec(ccexec_t *vm, ccblock_t *block)
// { ccvm_enter(vm,block);
//   do
//   { if(vm->curirix<ccarrlen(vm->current->instr))
//     { ccedict_t *ir=vm->current->instr+vm->curirix++;
//       ccvm_exec_instr(vm,vm->current,ir);
//     } else ccvm_exit(vm);
//   } while(vm->current);
// }

// ccfunc int
// ccvm_init(ccexec_t *vm)
// { memset(vm,ccnil,sizeof(*vm));

//   ccarrres(vm->globals,0xff);

//   vm->current=ccmalloc_T(ccvm_frame_t);

//   // vm->entrybl=ccvm_block(ccnil,"main");
//   // vm->current=vm->entrybl;
//   // vm->curirix=0;
//   return 1;
// }

// ccfunc int
// ccsvm_exectree(cctree_t *tree)
// {
//   ccexec_t vm;
//   ccvm_init(&vm);

//   for(cctree_t *stat=tree; stat<ccarrend(tree); stat++)
//   {
//    ccemit_tree(&vm,vm.current,stat);
//   }

//   ccvm_exec(&vm,vm.entrybl);

//   ccout("Globals\n");
//   { ccemit_value_t *it;
//     ccarrfor(vm.globals,it) ccout(ccformat("%s=%i\n",it->name,it->leaf.sig));
//   }
//   return 1;
// }

// const char *ccvm_instr_S[]=
// { "STORE","BINOP","BLOCK","CONDI","ENTER","LEAVE"
// };
// const char *ccvm_value_S[]=
// { "LEAF","GLOBAL","BLOCK","INSTR",
// };

// ccfunc void
// ccvmir_tos_(ccstr_t *buf, ccedict_t *ir)
// {
//   if((ir->type==ccedict_kENTER)||
//      (ir->type==ccedict_Kleave))
//   {
//     ccstrcatf(*buf,"%s: %s::%s", ccvm_instr_S[ir->type],
//       ir->block[0]->super?ir->block[0]->super->debug_label:"",ir->block[0]->debug_label);
//   } else
//   if((ir->type==ccedict_Kblock))
//   {
//     ccstrcatf(*buf,"%s: %s", ccvm_instr_S[ir->type], ir->block[0]->debug_label);
//   } else
//   { ccstrcatS(*buf,ccvm_instr_S[ir->type]);
//   }

// }

// ccfunc ccstr_t
// ccvmir_tos(ccedict_t *ir)
// {
//   ccstr_t buf=ccnil;
//   ccvmir_tos_(&buf,ir);
//   ccstrcatnl(buf);

//   ccout(buf);

//   ccstrdel(buf);

//   return buf;
// }
#endif