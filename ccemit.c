#ifndef _CCEMIT
#define _CCEMIT


// paisley - "a distinctive intricate pattern of curved feather-shaped figures based on an Indian pine-cone design"

// austere - "severe or strict in manner, attitude, or appearance"

ccfunc ccvalue_t *
ccvm_value_init(ccvalue_t *value, ccvalue_K kind, const char *name)
{ memset(value,ccnil,sizeof(*value));
  value->kind=kind;
  value->name=(ccstr_t)name;
  return value;
}

ccfunc ccvalue_t *
ccemit_const_classic_value(ccemit_t *emit, cctype_t *type, ccclassic_t classic)
{
  (void)emit;

  ccvalue_t *value=ccmalloc_T(ccvalue_t);
  value->kind=ccvalue_Ktyped;
  value->type=type;
  value->classic=classic;
  value->is_constant=cctrue;
  return value;
}

// warren - "a densely populated or labyrinthine building or district"

// Todo:
ccfunc ccvalue_t *
ccemit_const_i32(ccemit_t *emit, cci64 value)
{
  ccclassic_t classic={value};
  return ccemit_const_classic_value(emit,ctype_int32,classic);
}

ccfunc ccvalue_t *
ccmapget(ccvalue_t *arr, const char *name)
{ ccvalue_t *it;
  ccarrfor(arr,it)
  { if(strcmp(it->name,name)==0)
    { return it;
    }
  }
  return ccnil;
}

ccfunc ccvalue_t *
ccmapput(ccvalue_t *arr, const char *name)
{ ccvalue_t *value=ccmapget(arr,name);
  ccassert(!value);

  if(!value) value=ccarradd(arr,1);
  value->name=(ccstr_t)name;

  return value;
}

ccfunc ccvalue_t *
ccmapset(ccvalue_t *arr, ccvalue_K kind, const char *name)
{ ccvalue_t *value=ccmapget(arr,name);

  if(!value) value=ccvm_value_init(ccarradd(arr,1),kind,name);
  return value;
}

ccfunc ccblock_t *
ccblock_init(ccblock_t *block, ccblock_t *super, const char *debug_label)
{ memset(block,0,sizeof(*block));
  // Todo:
  ccarrres(block->local,0xff);
  ccarrres(block->instr,0xff);
  block->super=super;
  block->debug_label=debug_label;
  return block;
}

ccfunc ccvalue_t *
ccvm_global_ex(ccemit_t *vm, ccvalue_K kind, const char *name)
{ ccvalue_t *value=ccarradd(vm->globals,1);
  ccvm_value_init(value,kind,name);
  return value;
}

ccfunc ccvalue_t *
ccvm_global(ccemit_t *vm, cctype_t *type, ccstr_t name)
{
  ccvalue_t *val=ccvm_global_ex(vm,ccvalue_Kglobal,name);
  val->type=type;
  return val;
}

ccfunc ccvalue_t *
ccvm_local(ccblock_t *block, ccvalue_K kind, const char *name)
{
  ccvalue_t *val=ccarradd(block->local,1);
  ccvm_value_init(val,kind,name);
  return val;
}

ccfunc ccblock_t *
ccvm_block(ccblock_t *super, const char *debug_label)
{
  ccblock_t *block=(ccblock_t *)ccmalloc(sizeof(*block));
  return ccblock_init(block,super,debug_label);
}

ccfunc ccblock_t *
ccvm_label(ccemit_t *vm, ccblock_t *super, const char *name)
{
  ccvalue_t *value=ccmapget(vm->globals,name);
  ccblock_t *block=ccnil;

  if(!value)
  { block=ccvm_block(super,name);
    value=ccvm_local(super,ccvalue_Kblock,name);
    value->block=block;

    // Todo:
    ccvalue_t *global=ccvm_global_ex(vm,ccvalue_Kblock,name);
    global->block=block;
  } else block=value->block; // ccassert(value->kind==ccvalue_Kblock);

  return block;
}

ccfunc ccvalue_t *
ccvm_instrvalue(ccblock_t *block, ccedict_t *instr)
{ ccvalue_t *value=ccvm_local(block,ccvalue_Kinstr,"$0");
  value->instr=instr;
  return value;
}

ccfunc ccedict_t *
ccvm_addir(ccblock_t *block)
{
  ccedict_t *res=ccarradd(block->instr,1);
  memset(res,0,sizeof(*res));
  return res;
}

ccfunc ccvalue_t *
ccemit_binary(ccblock_t *block, cctoken_t oper, ccvalue_t *lhs, ccvalue_t *rhs)
{ ccedict_t *i=ccvm_addir(block);
  i->type=ccedict_Kbinop;
  i->oper=oper;
  i->lhs =lhs;
  i->rhs =rhs;
  return ccvm_instrvalue(block,i);
}

ccfunc ccvalue_t *
ccemit_store(ccblock_t *block, ccvalue_t *address, ccvalue_t *value)
{ cctracelog("%s] [s:32] %p, %p %i",block->debug_label,address,value,value->classic.as_i32);

  ccedict_t *i=ccvm_addir(block);
  i->type=ccedict_Kstore;
  i->address=address;
  i->value=value;
  // Todo:
  return ccvm_instrvalue(block,i);
}

ccfunc ccvalue_t *
ccemit_condi(ccblock_t *irset, ccvalue_t *check_value, ccblock_t *then_block, ccblock_t *else_block)
{ ccedict_t *i=ccvm_addir(irset);
  i->type=ccedict_Kcondi;
  i->condi=check_value;
  i->block[0]=then_block;
  i->block[1]=else_block;
  return ccvm_instrvalue(irset,i);
}

ccfunc ccvalue_t *
ccemit_block(ccblock_t *irset, ccblock_t *block)
{ ccedict_t *i=ccvm_addir(irset);
  i->type=ccedict_Kblock;
  i->block[0]=block;
  return ccvm_instrvalue(irset,i);
}

ccfunc ccvalue_t *
ccemit_call(ccblock_t *block, ccvalue_t *value)
{
  (void)block;
  (void)value;

  return ccnil;
}

ccfunc ccvalue_t *
ccemit_return(ccblock_t *irset)
{ ccedict_t *i=ccvm_addir(irset);
  i->type=ccedict_Kreturn;
  return ccvm_instrvalue(irset,i);
}

ccfunc ccvalue_t *
ccemit_enter(ccblock_t *irset, ccblock_t *block)
{ ccedict_t *i=ccvm_addir(irset);
  i->type=ccedict_Kenter;
  i->block[0]=block;
  return ccvm_instrvalue(irset,i);
}

ccfunc ccvalue_t *
ccemit_leave(ccblock_t *irset, ccblock_t *block)
{ ccedict_t *i=ccvm_addir(irset);
  i->type=ccedict_Kleave;
  i->block[0]=block;
  return ccvm_instrvalue(irset,i);
}

ccfunc ccvalue_t *
ccsvm_resolvevalue(ccemit_t *emit, ccfunction_t *func, ccblock_t *irset, cctree_t *tree)
{ ccvalue_t *value=ccnil;
  if(tree->kind==cctree_Kbinary)
  { ccvalue_t *lhs=ccsvm_resolvevalue(emit,func,irset,tree->binary.lhs);
    ccvalue_t *rhs=ccsvm_resolvevalue(emit,func,irset,tree->binary.rhs);
    return ccemit_binary(irset,tree->binary.opr,lhs,rhs);
  } else
  if(tree->kind==cctree_Kidentifier)
  {
    value=ccmapget(func->decls->local,tree->constant.token.str);
    if(!value) value=ccmapget(emit->globals,tree->constant.token.str);

  } else
  if(tree->kind==cctree_Kint)
  { value=ccemit_const_i32(emit,tree->constant.token.sig);
  }
  return value;
}

ccfunc ccvalue_t *
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

ccfunc ccvalue_t *
ccemit_tree(
  ccemit_t *emit, ccfunction_t *func, ccblock_t *irset, cctree_t *tree)
{
  ccvalue_t *result=ccnil;

  if(tree->kind==cctree_Kdecl)
  {
    ccassert(!"error");

  } else
  if(tree->kind==cctree_Kblock_stmt)
  {
    ccblock_t *child=ccvm_block(irset,"$block");
    ccemit_treelist(emit,func,child,tree->stmt_list);
    result=ccemit_block(irset,child);

  } else
  if(tree->kind==cctree_Klabel_stmt)
  {
    irset=ccvm_label(emit,irset,tree->label_name);
    ccemit_treelist(emit,func,irset,tree->stmt_list);
    result=ccemit_block(irset,irset);

  } else
  if(tree->kind==cctree_Kreturn_statement)
  {
    ccemit_return(irset);
  } else
  if(tree->kind==cctree_Kgoto_statement)
  {
    ccblock_t *label_block=ccvm_label(emit,irset,tree->label_name);
    ccemit_enter(irset,label_block);
  } else
  if(tree->kind==cctree_Kint)
  {
    result=ccemit_const_i32(emit,tree->constant.token.sig);
  } else
  if(tree->kind==cctree_Kcall_expr)
  {
    ccstr_t func_name=tree->expr_tree->constant.token.str;
    ccvalue_t *func_value=ccmapget(emit->globals,func_name);
    ccemit_call(irset,func_value);
  } else
  if(tree->kind==cctree_Kbinary)
  {
    cctree_t *lhs,*rhs;
    lhs=tree->binary.lhs; ccnotnil(lhs!=0);
    rhs=tree->binary.rhs; ccnotnil(rhs!=0);

    ccvalue_t *lval,*rval;
    lval=ccnil;
    rval=ccnil;
#if 0
    lval=ccsvm_resolvevalue(emit,func,irset,lhs); ccnotnil(lval!=0);
    rval=ccsvm_resolvevalue(emit,func,irset,rhs); ccnotnil(rval!=0);
#endif
    lval=ccmapget(func->decls->local,lhs->constant.token.str);
    if(!lval) lval=ccmapget(emit->globals,lhs->constant.token.str);

    if(rhs->kind==cctree_Kint)
    { rval=ccemit_const_i32(emit,rhs->constant.token.sig);
    }

    ccnotnil(lval);
    ccnotnil(rval);


    cctoken_t tok=tree->binary.opr;
    if(tok.bit==cctoken_Kassign)
    { return ccemit_store(irset,lval,rval);
    } else
    { return ccemit_binary(irset,tok,lval,rval);
    }
  } else
  if(tree->kind==cctree_Kwhile_statement)
  { ccblock_t *cond_block=ccvm_block(irset,"$while::cond");
    ccblock_t *then_block=ccvm_block(cond_block,"$while::then");
    ccblock_t *else_block=ccvm_block(irset,"$while::else");
    ccvalue_t *cond_value=ccemit_tree(emit,func,cond_block,tree->cond_tree);
    ccemit_condi(cond_block,cond_value,then_block,else_block);
    if(tree->then_tree) ccemit_tree(emit,func,then_block,tree->then_tree);
    ccemit_enter(then_block,cond_block);
    ccemit_block(irset,cond_block);
    emit->current=else_block;
    emit->curirix=0;
  } else
  if(tree->kind==cctree_Kconditional_statement)
  { ccvalue_t *cond_value=ccemit_tree(emit,func,irset,tree->cond_tree);
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
  } else
  { ccassert(!"error");
  }

  return result;
}

ccfunc void
ccemit_decl_name(ccemit_t *emit, ccfunction_t *func, cctree_t *decl)
{
  ccnotnil(decl->decl_type);
  ccnotnil(decl->decl_name);
  ccassert(!decl->decl_size);
  ccassert(!decl->decl_list);

  ccvalue_t *local=ccmapput(func->decls->local,decl->decl_name);
  local->kind=ccvalue_Ktyped;
  local->type=decl->decl_type;


  ccedict_t *edict=ccarradd(func->decls->instr,1);
  edict->type=ccedict_Klocal;
  edict->local=local;

  cctree_t *init=decl->decl_init;
  if(init)
  { if(init->kind==cctree_Kint)
    {
      ccvalue_t *value=ccemit_const_i32(emit,init->constant.token.sig);
      ccemit_store(func->enter,local,value);
    } else ccassert(!"error");
  }
}

ccfunc void
ccemit_decl(ccemit_t *emit, ccfunction_t *func, cctree_t *decl)
{
  ccnotnil(decl);
  ccassert(decl->kind==cctree_Kdecl);
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

  ccblock_init(func->decls,ccnil,"$decls");
  ccblock_init(func->enter,ccnil,"$enter");
  ccblock_init(func->leave,ccnil,"$leave");

  cctree_t *decl_name;
  ccarrfor(type->list,decl_name)
  { ccassert(!decl_name->decl_init);
    ccemit_decl_name(emit,func,decl_name);
  }

  ccassert(body->kind==cctree_Kblock_stmt);
  cctree_t *stmt;
  ccarrfor(body->stmt_list,stmt)
  {
    if(stmt->kind==cctree_Kdecl)
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
    if(decl->kind!=cctree_Kdecl)
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

        ccvalue_t *value=ccmapput(emit->globals,decl_name->decl_name);
        value->name=decl_name->decl_name;
        value->kind=ccvalue_Ktyped;
        value->function=func;

      } else
      { ccnotnil(decl_name->decl_type);
        ccnotnil(decl_name->decl_name);
        ccassert(!decl_name->decl_size);
        ccassert(!decl_name->decl_list);
        ccassert(!decl_name->decl_init);

        ccvalue_t *global=ccmapput(emit->globals,decl_name->decl_name);
        global->is_global=cctrue;
      }
    }
  }
}

ccfunc void
ccemit_init(ccemit_t *emit)
{ memset(emit,ccnil,sizeof(*emit));
  ccarrres(emit->globals,0xff);

  emit->current=ccvm_block(ccnil,"main");
  emit->curirix=0;
}



// typedef enum ccedict_K ccedict_K;
// typedef enum ccvalue_K ccvalue_K;
// typedef struct ccvalue_t ccvalue_t;
// typedef struct ccedict_t ccedict_t;
// typedef struct ccblock_t ccblock_t;
// typedef struct ccfunction_t ccfunction_t;
// typedef struct ccvm_frame_t ccvm_frame_t;

// typedef enum ccedict_K
// { ccedict_Kstore = 0,
//   ccedict_Kbinop,
//   ccedict_Kblock,
//   ccedict_Kcondi,
//   ccedict_Kenter,
//   ccedict_Kleave,
// } ccedict_K;
// typedef enum ccvalue_K
// { ccvalue_Kleaf = 0,
//   ccvalue_Kglobal,
//   ccvalue_Kblock,
//   ccvalue_Kinstr,
// } ccvalue_K;
// typedef struct ccvalue_t
// { ccvalue_K    kind;
//   ccstr_t         name;

//   // Todo: replace leaf with an actual pointer, should be address instead ...
//   // Real store/load instructions ...
//   cctype_t       *type;
//   cctoken_t       leaf;

//   ccblock_t    *block;
//   ccedict_t    *instr;
//   ccfunction_t  *routine;
// } ccvalue_t;
// typedef struct ccedict_t
// { ccedict_K  type;
//   ccvalue_t *res;

//   // Todo: make union ...
//   cctoken_t     oper;
//   ccvalue_t *lhs;
//   ccvalue_t *rhs;

//   ccvalue_t *condi;
//   ccblock_t *block[4];
// } ccedict_t;
// // Note: a block itself does not have a label and thus may not be referenced, a value however, may have a label,
// // and it may be a block type ... labeled blocks are value<block> ...
// typedef struct ccblock_t
// { const char     *debug_label;
//   ccfunction_t *owner;
//   ccblock_t   *super;
//   ccvalue_t   *local;
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
//  ccvalue_t   *arguments;
//  ccfunction_t *routine;
//   ccblock_t   *prevblc;
//  ccblock_t   *currblc;
//   int             curirix;
// } ccvm_frame_t;
// typedef struct ccexec_t
// { ccvalue_t   *globals;
//   ccfunction_t *routine;
//  ccvm_frame_t   *current;
// } ccexec_t;

// ccfunc ccstr_t ccvmir_tos(ccedict_t *ir);

// ccfunc ccvalue_t *
// ccmapget(ccexec_t *vm, const char *name)
// {
//   ccvalue_t *it;
//   ccarrfor(vm->globals,it)
//   {
//     if(strcmp(name,it->name)==0)
//     {
//       return it;
//     }
//   }
//   return ccnil;
// }

// ccfunc ccvalue_t *
// ccvm_value_init(ccvalue_t *value, ccvalue_K kind, const char *name)
// { memset(value,ccnil,sizeof(*value));
//   value->kind=kind;
//   value->name=(ccstr_t)name;
//   return value;
// }

// ccfunc ccblock_t *
// ccblock_init(ccblock_t *block, ccblock_t *super, const char *debug_label)
// { memset(block,0,sizeof(*block));
//   // Todo:
//   ccarrres(block->local,0xff);
//   ccarrres(block->instr,0xff);
//   block->super=super;
//   block->debug_label=debug_label;
//   return block;
// }

// ccfunc ccvalue_t *
// ccvm_global_ex(ccexec_t *vm, ccvalue_K kind, const char *name)
// { ccvalue_t *value=ccarradd(vm->globals,1);
//   ccvm_value_init(value,kind,name);
//   return value;
// }

// ccfunc ccvalue_t *
// ccvm_global(ccexec_t *vm, cctype_t *type, ccstr_t name)
// {
//   ccvalue_t *val=ccvm_global_ex(vm,ccvalue_Kglobal,name);
//   val->type=type;
//   return val;
// }

// ccfunc ccvalue_t *
// ccvm_local(ccblock_t *block, ccvalue_K kind, const char *name)
// {
//   ccvalue_t *val=ccarradd(block->local,1);
//   ccvm_value_init(val,kind,name);
//   return val;
// }

// ccfunc ccblock_t *
// ccvm_block(ccblock_t *super, const char *debug_label)
// {
//   ccblock_t *block=(ccblock_t *)ccmalloc(sizeof(*block));
//   return ccblock_init(block,super,debug_label);
// }

// ccfunc ccblock_t *
// ccvm_label(ccexec_t *vm, ccblock_t *super, const char *name)
// {
//   ccvalue_t *value=ccmapget(vm,name);
//   ccblock_t *block=ccnil;

//   if(!value)
//   { block=ccvm_block(super,name);
//     value=ccvm_local(super,ccvalue_Kblock,name);
//     value->block=block;

//     // Todo:
//     ccvalue_t *global=ccvm_global_ex(vm,ccvalue_Kblock,name);
//     global->block=block;
//   } else block=value->block; // ccassert(value->kind==ccvalue_Kblock);

//   return block;
// }

// ccfunc ccvalue_t *
// ccvm_leafvalue(ccblock_t *block, cctoken_t leaf)
// { ccvalue_t *value=ccvm_local(block,ccvalue_Kleaf,"$0");
//   value->leaf=leaf;
//   return value;
// }

// ccfunc ccvalue_t *
// ccvm_intvalue(ccblock_t *block, signed long long int val)
// { ccvalue_t *value=ccvm_local(block,ccvalue_Kleaf,"$0");
//   value->leaf.bit=cctoken_Kliteral_integer;
//   value->leaf.sig=val;
//   return value;
// }

// ccfunc ccvalue_t *
// ccvm_instrvalue(ccblock_t *block, ccedict_t *instr)
// { ccvalue_t *value=ccvm_local(block,ccvalue_Kinstr,"$0");
//   value->instr=instr;
//   return value;
// }

// ccfunc ccedict_t *
// ccvm_addir(ccblock_t *block)
// {
//   ccedict_t *res=ccarradd(block->instr,1);
//   memset(res,0,sizeof(*res));
//   return res;
// }

// ccfunc ccvalue_t *
// ccemit_binary(ccblock_t *block, cctoken_t oper, ccvalue_t *lhs, ccvalue_t *rhs)
// { ccedict_t *i=ccvm_addir(block);
//   i->type=ccedict_Kbinop;
//   i->oper=oper;
//   i->lhs =lhs;
//   i->rhs =rhs;
//   return ccvm_instrvalue(block,i);
// }

// ccfunc ccvalue_t *
// ccemit_store(ccblock_t *block, ccvalue_t *lhs, ccvalue_t *rhs)
// { ccedict_t *i=ccvm_addir(block);
//   i->type=ccedict_Kstore;
//   i->lhs=lhs;
//   i->rhs=rhs;
//   return ccvm_instrvalue(block,i);
// }

// ccfunc ccvalue_t *
// ccemit_condi(ccblock_t *irset, ccvalue_t *check_value, ccblock_t *then_block, ccblock_t *else_block)
// { ccedict_t *i=ccvm_addir(irset);
//   i->type=ccedict_Kcondi;
//   i->condi=check_value;
//   i->block[0]=then_block;
//   i->block[1]=else_block;
//   return ccvm_instrvalue(irset,i);
// }

// ccfunc ccvalue_t *
// ccemit_block(ccblock_t *irset, ccblock_t *block)
// { ccedict_t *i=ccvm_addir(irset);
//   i->type=ccedict_Kblock;
//   i->block[0]=block;
//   return ccvm_instrvalue(irset,i);
// }

// ccfunc ccvalue_t *
// ccemit_enter(ccblock_t *irset, ccblock_t *block)
// { ccedict_t *i=ccvm_addir(irset);
//   i->type=ccedict_Kenter;
//   i->block[0]=block;
//   return ccvm_instrvalue(irset,i);
// }

// ccfunc ccvalue_t *
// ccemit_leave(ccblock_t *irset, ccblock_t *block)
// { ccedict_t *i=ccvm_addir(irset);
//   i->type=ccedict_Kleave;
//   i->block[0]=block;
//   return ccvm_instrvalue(irset,i);
// }

// ccfunc ccvalue_t *
// ccsvm_resolvevalue(ccexec_t *vm, ccblock_t *irset, cctree_t *tree)
// { ccvalue_t *value=ccnil;
//   if(tree->kind==cctree_Kbinary)
//   { ccvalue_t *lhs=ccsvm_resolvevalue(vm,irset,tree->binary.lhs);
//     ccvalue_t *rhs=ccsvm_resolvevalue(vm,irset,tree->binary.rhs);
//     return ccemit_binary(irset,tree->binary.opr,lhs,rhs);
//   } else
//   if(tree->kind==cctree_Kidentifier)
//   { value=ccmapget(vm,tree->constant.token.str);
//   } else
//   if(tree->kind==cctree_Kint)
//   { value=ccvm_leafvalue(irset,tree->constant.token);
//   }
//   return value;
// }

// ccfunc ccvalue_t *
// ccemit_tree(ccexec_t *vm, ccblock_t *irset, cctree_t *tree)
// {
//   ccvalue_t *result=ccnil;

//   if(tree->kind==cctree_Kblock_stmt)
//   {
//     // Note: aside from containing a set of instructions, blocks also contain local values ...
//     // this may change in the future ...
//     ccblock_t *child=ccvm_block(irset,"mixed");
//     cctree_t *it;
//     ccarrfor(tree->mix_statement.stat,it)
//     {
//       ccemit_tree(vm,child,it);
//     }
//     ccemit_block(irset,child);
//   } else
//   if(tree->kind==cctree_Klabel_stmt)
//   {
//     // Note: I'm expecting the parser to generate a hierarchy of labels, as supposed to a list,
//     // so after this label, there will be no more instructions ... therefore I don't have to switch the current block ...

//     irset=ccvm_label(vm,irset,tree->label_statement.name);

//     cctree_t *stat;
//     ccarrfor(tree->label_statement.list,stat)
//     {
//       ccemit_tree(vm,irset,stat);
//     }
//   } else
//   if(tree->kind==cctree_Kgoto_statement)
//   {
//     ccblock_t *label_block=ccvm_label(vm,irset,tree->goto_statement.name);
//     ccemit_enter(irset,label_block);
//   } else
//   if(tree->kind==cctree_Kint)
//   {
//     result=ccvm_leafvalue(irset,tree->constant.token);
//   } else
//   if(tree->kind==cctree_Kdecl)
//   {
//     cctree_t *it;
//    ccnotnil(tree->decl_name);

//     ccarrfor(tree->decl_name,it)
//     {
//      if(it->decl_name_type->kind==cctype_Kfunc)
//      {
//        ccvalue_t *func_value=ccvm_global(vm,it->decl_name_type,it->decl_name_iden);
//        ccassert(it->body_tree);
//        func_value->block=ccvm_block(irset,"procedure::entry");
//        ccemit_tree(vm,func_value->block,it->body_tree);

//      } else
//      {
//        ccvalue_t *address,*value=ccnil;
//        address=ccvm_global(vm,
//          it->decl_name_type,
//          it->decl_name_iden);

//        cctree_t *init=it->decl_name_init;
//        if(init)
//        { if(init->kind==cctree_Kint)
//          { value=ccvm_leafvalue(irset,init->constant.token);
//          } else ccassert(!"error");
//          ccemit_store(irset,address,value);
//        }
//      }
//     }
//     // Note: don't return anything here ...
//   } else
//   if(tree->kind==cctree_Kbinary)
//   {
//     cctree_t *lhs,*rhs;
//     lhs=tree->binary.lhs; ccnotnil(lhs!=0);
//     rhs=tree->binary.rhs; ccnotnil(rhs!=0);

//     ccvalue_t *lval,*rval;
//     lval=ccsvm_resolvevalue(vm,irset,lhs);
//     rval=ccsvm_resolvevalue(vm,irset,rhs);

//     ccnotnil(lval!=0);
//     ccnotnil(rval!=0);
//     cctoken_t tok=tree->binary.opr;
//     if(tok.bit==cctoken_Kassign)
//     { return ccemit_store(irset,lval,rval);
//     } else
//     { return ccemit_binary(irset,tok,lval,rval);
//     }
//   } else
//   if(tree->kind==cctree_Kwhile_statement)
//   { ccblock_t *cond_block=ccvm_block(irset,"$while::cond");
//     ccblock_t *then_block=ccvm_block(cond_block,"$while::then");
//     ccblock_t *else_block=ccvm_block(irset,"$while::else");
//     ccvalue_t *cond_value=ccemit_tree(vm,cond_block,tree->cond_tree);
//     ccemit_condi(cond_block,cond_value,then_block,else_block);
//     if(tree->then_tree) ccemit_tree(vm,then_block,tree->then_tree);
//     ccemit_enter(then_block,cond_block);
//     ccemit_block(irset,cond_block);

//     vm->current=else_block;
//     vm->curirix=0;
//   } else
//   if(tree->kind==cctree_Kconditional_statement)
//   { ccvalue_t *cond_value=ccemit_tree(vm,irset,tree->cond_tree);
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

// ccfunc ccvalue_t *
// ccsvm_execbinary(ccblock_t *irset, cctoken_t oper, ccvalue_t *lval, ccvalue_t *rval)
// {
//   if(!lval) cctraceerr("undefined lhs value"); // Todo: to string ...
//   if(!rval) cctraceerr("undefined rhs value"); // Todo: to string ...

//   ccassert(lval->kind==ccvalue_Kleaf||lval->kind==ccvalue_Kglobal);
//   ccassert(rval->kind==ccvalue_Kleaf||rval->kind==ccvalue_Kglobal);

//   if(oper.bit==cctoken_Kassign)
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
//     case ccedict_Kstore:
//     { ccvalue_t *lhs,*rhs;
//       lhs=instr->lhs;
//       rhs=instr->rhs;
//       ccassert(lhs->kind==ccvalue_Kglobal);
//       if(rhs->kind==ccvalue_Kleaf)
//       { lhs->leaf=rhs->leaf;
//       } else
//       if(rhs->kind==ccvalue_Kinstr)
//       { lhs->leaf=rhs->instr->res->leaf;
//       }
//       instr->res=lhs;
//     } break;
//     case ccedict_Kenter:
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
//     case ccedict_Kbinop:
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
//   { ccvalue_t *it;
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
//   if((ir->type==ccedict_Kenter)||
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