#ifndef _CCSVM
#define _CCSVM

typedef enum ccvm_instr_K ccvm_instr_K;
typedef enum ccvm_value_K ccvm_value_K;
typedef struct ccvm_value_t ccvm_value_t;
typedef struct ccvm_instr_t ccvm_instr_t;
typedef struct ccvm_block_t ccvm_block_t;
typedef struct ccvm_routine_t ccvm_routine_t;

typedef enum ccvm_instr_K
{ ccvm_instr_Kstore = 0,
  ccvm_instr_Kbinop,
  ccvm_instr_Kblock,
  ccvm_instr_Kcondi,
  ccvm_instr_Kenter,
  ccvm_instr_Kleave,
} ccvm_instr_K;
typedef enum ccvm_value_K
{ ccvm_value_Kleaf = 0,
  ccvm_value_Kglobal,
  ccvm_value_Kblock,
  ccvm_value_Kinstr,
} ccvm_value_K;
typedef struct ccvm_value_t
{ ccvm_value_K    kind;
  ccstr_t         name;
  cctype_t       *type;
  ccvm_block_t   *block;
  ccvm_instr_t   *instr;
  cctoken_t       leaf;
} ccvm_value_t;
typedef struct ccvm_instr_t
{ ccvm_instr_K  type;
  ccvm_value_t *res;

  // Todo: make union ...
  cctoken_t     oper;
  ccvm_value_t *lhs;
  ccvm_value_t *rhs;

  ccvm_value_t *condi;
  ccvm_block_t *block[4];
} ccvm_instr_t;
// Note: a block itself does not have a label and thus may not be referenced, a value however, may have a label,
// and it may be a block type ... labeled blocks are value<block> ...
typedef struct ccvm_block_t
{ const char   *debug_label;
  ccvm_block_t *super;
  ccvm_value_t *local;
  ccvm_instr_t *instr;
} ccvm_block_t;
typedef struct ccvm_routine_t
{ ccvm_block_t *block;
  ccvm_block_t *enter;
  ccvm_block_t *leave;
  ccvm_block_t *current;
} ccvm_routine_t;
typedef struct ccvm_t
{ ccvm_value_t   *globals;
  ccvm_routine_t *routine;
  ccvm_block_t   *prevblc;
  ccvm_block_t   *entrybl;
  ccvm_block_t   *current;
  int             curirix;
} ccvm_t;

ccfunc ccstr_t ccvmir_tos(ccvm_instr_t *ir);

ccfunc ccvm_value_t *
ccvm_findglobal(ccvm_t *vm, const char *name)
{
  ccvm_value_t *it;
  ccarrfor(vm->globals,it)
  {
    if(strcmp(name,it->name)==0)
    {
      return it;
    }
  }
  return ccnil;
}

ccfunc ccvm_value_t *
ccvm_value_init(ccvm_value_t *value, ccvm_value_K kind, const char *name)
{ memset(value,ccnil,sizeof(*value));
  value->kind=kind;
  value->name=(ccstr_t)name;
  return value;
}

ccfunc ccvm_block_t *
ccvm_block_init(ccvm_block_t *block, ccvm_block_t *super, const char *debug_label)
{ memset(block,0,sizeof(*block));
  // Todo:
  ccarrres(block->local,0xff);
  ccarrres(block->instr,0xff);
  block->super=super;
  block->debug_label=debug_label;
  return block;
}

ccfunc ccvm_value_t *
ccvm_global_ex(ccvm_t *vm, ccvm_value_K kind, const char *name)
{ ccvm_value_t *value=ccarradd(vm->globals,1);
  ccvm_value_init(value,kind,name);
  return value;
}

ccfunc ccvm_value_t *
ccvm_global(ccvm_t *vm, cctype_t *type, ccstr_t name)
{
  ccvm_value_t *val=ccvm_global_ex(vm,ccvm_value_Kglobal,name);
  val->type=type;
  return val;
}

ccfunc ccvm_value_t *
ccvm_local(ccvm_block_t *block, ccvm_value_K kind, const char *name)
{
  ccvm_value_t *val=ccarradd(block->local,1);
  ccvm_value_init(val,kind,name);
  return val;
}

ccfunc ccvm_block_t *
ccvm_block(ccvm_block_t *super, const char *debug_label)
{
  ccvm_block_t *block=(ccvm_block_t *)ccmalloc(sizeof(*block));
  return ccvm_block_init(block,super,debug_label);
}

ccfunc ccvm_block_t *
ccvm_label(ccvm_t *vm, ccvm_block_t *super, const char *name)
{
  ccvm_value_t *value=ccvm_findglobal(vm,name);
  ccvm_block_t *block=ccnil;

  if(!value)
  { block=ccvm_block(super,name);
    value=ccvm_local(super,ccvm_value_Kblock,name);
    value->block=block;

    // Todo:
    ccvm_value_t *global=ccvm_global_ex(vm,ccvm_value_Kblock,name);
    global->block=block;
  } else block=value->block; // ccassert(value->kind==ccvm_value_Kblock);

  return block;
}

ccfunc ccvm_value_t *
ccvm_leafvalue(ccvm_block_t *block, cctoken_t leaf)
{ ccvm_value_t *value=ccvm_local(block,ccvm_value_Kleaf,"$0");
  value->leaf=leaf;
  return value;
}

ccfunc ccvm_value_t *
ccvm_intvalue(ccvm_block_t *block, signed long long int val)
{ ccvm_value_t *value=ccvm_local(block,ccvm_value_Kleaf,"$0");
  value->leaf.bit=cctoken_Kliteral_integer;
  value->leaf.sig=val;
  return value;
}

ccfunc ccvm_value_t *
ccvm_instrvalue(ccvm_block_t *block, ccvm_instr_t *instr)
{ ccvm_value_t *value=ccvm_local(block,ccvm_value_Kinstr,"$0");
  value->instr=instr;
  return value;
}

ccfunc ccvm_instr_t *
ccvm_addir(ccvm_block_t *block)
{
  ccvm_instr_t *res=ccarradd(block->instr,1);
  memset(res,0,sizeof(*res));
  return res;
}

ccfunc ccvm_value_t *
ccemit_binary(ccvm_block_t *block, cctoken_t oper, ccvm_value_t *lhs, ccvm_value_t *rhs)
{ ccvm_instr_t *i=ccvm_addir(block);
  i->type=ccvm_instr_Kbinop;
  i->oper=oper;
  i->lhs =lhs;
  i->rhs =rhs;
  return ccvm_instrvalue(block,i);
}

ccfunc ccvm_value_t *
ccemit_store(ccvm_block_t *block, ccvm_value_t *lhs, ccvm_value_t *rhs)
{ ccvm_instr_t *i=ccvm_addir(block);
  i->type=ccvm_instr_Kstore;
  i->lhs=lhs;
  i->rhs=rhs;
  return ccvm_instrvalue(block,i);
}

ccfunc ccvm_value_t *
ccemit_condi(ccvm_block_t *irset, ccvm_value_t *check_value, ccvm_block_t *then_block, ccvm_block_t *else_block)
{ ccvm_instr_t *i=ccvm_addir(irset);
  i->type=ccvm_instr_Kcondi;
  i->condi=check_value;
  i->block[0]=then_block;
  i->block[1]=else_block;
  return ccvm_instrvalue(irset,i);
}

ccfunc ccvm_value_t *
ccemit_block(ccvm_block_t *irset, ccvm_block_t *block)
{ ccvm_instr_t *i=ccvm_addir(irset);
  i->type=ccvm_instr_Kblock;
  i->block[0]=block;
  return ccvm_instrvalue(irset,i);
}

ccfunc ccvm_value_t *
ccemit_enter(ccvm_block_t *irset, ccvm_block_t *block)
{ ccvm_instr_t *i=ccvm_addir(irset);
  i->type=ccvm_instr_Kenter;
  i->block[0]=block;
  return ccvm_instrvalue(irset,i);
}

ccfunc ccvm_value_t *
ccemit_leave(ccvm_block_t *irset, ccvm_block_t *block)
{ ccvm_instr_t *i=ccvm_addir(irset);
  i->type=ccvm_instr_Kleave;
  i->block[0]=block;
  return ccvm_instrvalue(irset,i);
}

ccfunc ccvm_value_t *
ccsvm_resolvevalue(ccvm_t *vm, ccvm_block_t *irset, cctree_t *tree)
{ ccvm_value_t *value=ccnil;
  if(tree->kind==cctree_Kbinary)
  { ccvm_value_t *lhs=ccsvm_resolvevalue(vm,irset,tree->binary.lhs);
    ccvm_value_t *rhs=ccsvm_resolvevalue(vm,irset,tree->binary.rhs);
    return ccemit_binary(irset,tree->binary.opr,lhs,rhs);
  } else
  if(tree->kind==cctree_Kidentifier)
  { value=ccvm_findglobal(vm,tree->constant.token.str);
  } else
  if(tree->kind==cctree_Kint)
  { value=ccvm_leafvalue(irset,tree->constant.token);
  }
  return value;
}

ccfunc ccvm_value_t *
ccemit_tree(ccvm_t *vm, ccvm_block_t *irset, cctree_t *tree)
{
  ccvm_value_t *result=ccnil;

  if(tree->kind==cctree_Kmixed_statement)
  {
    // Note: aside from containing a set of instructions, blocks also contain local values ...
    // this may change in the future ...
    ccvm_block_t *child=ccvm_block(irset,"mixed");
    cctree_t *it;
    ccarrfor(tree->mix_statement.stat,it)
    {
      ccemit_tree(vm,child,it);
    }
    ccemit_block(irset,child);
  } else
  if(tree->kind==cctree_Klabel_statement)
  {
    // Note: I'm expecting the parser to generate a hierarchy of labels, as supposed to a list,
    // so after this label, there will be no more instructions ... therefore I don't have to switch the current block ...

    irset=ccvm_label(vm,irset,tree->label_statement.name);

    cctree_t *stat;
    ccarrfor(tree->label_statement.list,stat)
    {
      ccemit_tree(vm,irset,stat);
    }
  } else
  if(tree->kind==cctree_Kgoto_statement)
  {
    ccvm_block_t *label_block=ccvm_label(vm,irset,tree->goto_statement.name);
    ccemit_enter(irset,label_block);
  } else
  if(tree->kind==cctree_Kint)
  {
    result=ccvm_leafvalue(irset,tree->constant.token);
  } else
  if(tree->kind==cctree_Kdecl)
  {
    cctree_t *it;
  	ccnotnil(tree->decl_name);

    ccarrfor(tree->decl_name,it)
    {
    	if(it->decl_name_type->kind==cctype_Kfunc)
    	{
    		// ccvm_value_t *func_value=ccvm_global(vm,it->decl_name_type,it->decl_name_iden);
    		// ccassert(it->body_tree);
  			// ccemit_tree()

    	} else
    	{
	      ccvm_value_t *address,*value=ccnil;
	      address=ccvm_global(vm,
	        it->decl_name_type,
	        it->decl_name_iden);

	      cctree_t *init=it->decl_name_init;
	      if(init)
	      { if(init->kind==cctree_Kint)
	        { value=ccvm_leafvalue(irset,init->constant.token);
	        } else ccassert(!"error");
	        ccemit_store(irset,address,value);
	      }
    	}
    }
    // Note: don't return anything here ...
  } else
  if(tree->kind==cctree_Kbinary)
  {
    cctree_t *lhs,*rhs;
    lhs=tree->binary.lhs; ccnotnil(lhs!=0);
    rhs=tree->binary.rhs; ccnotnil(rhs!=0);

    ccvm_value_t *lval,*rval;
    lval=ccsvm_resolvevalue(vm,irset,lhs);
    rval=ccsvm_resolvevalue(vm,irset,rhs);

    ccnotnil(lval!=0);
    ccnotnil(rval!=0);
    cctoken_t tok=tree->binary.opr;
    if(tok.bit==cctoken_Kassign)
    { return ccemit_store(irset,lval,rval);
    } else
    { return ccemit_binary(irset,tok,lval,rval);
    }
  } else
  if(tree->kind==cctree_Kwhile_statement)
  { ccvm_block_t *cond_block=ccvm_block(irset,"$while::cond");
    ccvm_block_t *then_block=ccvm_block(cond_block,"$while::then");
    ccvm_block_t *else_block=ccvm_block(irset,"$while::else");
    ccvm_value_t *cond_value=ccemit_tree(vm,cond_block,tree->cond_tree);
    ccemit_condi(cond_block,cond_value,then_block,else_block);
    if(tree->then_tree) ccemit_tree(vm,then_block,tree->then_tree);
    ccemit_enter(then_block,cond_block);
    ccemit_block(irset,cond_block);
    vm->current=else_block;
    vm->curirix=0;
  } else
  if(tree->kind==cctree_Kconditional_statement)
  { ccvm_value_t *cond_value=ccemit_tree(vm,irset,tree->cond_tree);
    ccvm_block_t *then_block=ccnil,*else_block=ccnil;
    ccvm_block_t *done_block=ccvm_block(irset,"$local");
    if(tree->then_tree)
    { then_block=ccvm_block(irset,"$if::then");
      ccemit_tree(vm,then_block,tree->then_tree);
      ccemit_enter(then_block,done_block);
    }
    if(tree->else_tree)
    { irset=else_block=ccvm_block(irset,"$if::else");
      ccemit_tree(vm,else_block,tree->else_tree);
      ccemit_enter(else_block,done_block);
    }
    ccemit_condi(irset,cond_value,then_block,else_block);
    vm->current=done_block;
    vm->curirix=0;
  } else
  { ccassert(!"error");
  }

  return result;
}

ccfunc ccvm_value_t *
ccsvm_execbinary(ccvm_block_t *irset, cctoken_t oper, ccvm_value_t *lval, ccvm_value_t *rval)
{
  if(!lval) cctraceerr("undefined lhs value"); // Todo: to string ...
  if(!rval) cctraceerr("undefined rhs value"); // Todo: to string ...

  ccassert(lval->kind==ccvm_value_Kleaf||lval->kind==ccvm_value_Kglobal);
  ccassert(rval->kind==ccvm_value_Kleaf||rval->kind==ccvm_value_Kglobal);

  if(oper.bit==cctoken_Kassign)
  { lval->leaf=rval->leaf;
    return lval;
  } else
  if(oper.bit==cctoken_Kequals)
  { return ccvm_intvalue(irset,lval->leaf.sig==rval->leaf.sig);
  } else
  if(oper.bit==cctoken_Kgreater_than)
  { return ccvm_intvalue(irset,lval->leaf.sig>rval->leaf.sig);
  } else
  if(oper.bit==cctoken_Kgreater_than_eql)
  { return ccvm_intvalue(irset,lval->leaf.sig>=rval->leaf.sig);
  } else
  if(oper.bit==cctoken_Kless_than)
  { return ccvm_intvalue(irset,lval->leaf.sig<rval->leaf.sig);
  } else
  if(oper.bit==cctoken_Kless_than_eql)
  { return ccvm_intvalue(irset,lval->leaf.sig<=rval->leaf.sig);
  } else
  if(oper.bit==cctoken_Kadd)
  { return ccvm_intvalue(irset,lval->leaf.sig+rval->leaf.sig);
  } else
  { ccassert(!"error");
    return ccnil;
  }
}

ccfunc void
ccvm_enter(ccvm_t *vm, ccvm_block_t *block)
{
  ccnotnil(block);

  vm->prevblc=block;
  vm->current=block;
  vm->curirix=0;
}

ccfunc void
ccvm_leave(ccvm_t *vm, ccvm_block_t *block)
{
  ccnotnil(block);

  vm->current=block->super;
}

ccfunc void
ccvm_exec_instr(ccvm_t *vm, ccvm_block_t *irset, ccvm_instr_t *instr)
{
  ccvmir_tos(instr);

  switch(instr->type)
  { case ccvm_instr_Kblock:
    { ccvm_instr_t *it;
      ccarrfor(instr->block[0]->instr,it)
      {
        ccvm_exec_instr(vm,irset,it);
      }
    } break;
    case ccvm_instr_Kstore:
    { ccvm_value_t *lhs,*rhs;
      lhs=instr->lhs;
      rhs=instr->rhs;
      ccassert(lhs->kind==ccvm_value_Kglobal);
      if(rhs->kind==ccvm_value_Kleaf)
      { lhs->leaf=rhs->leaf;
      } else
      if(rhs->kind==ccvm_value_Kinstr)
      { lhs->leaf=rhs->instr->res->leaf;
      }
      instr->res=lhs;
    } break;
    case ccvm_instr_Kenter:
    { ccvm_enter(vm,instr->block[0]);
    } break;
    case ccvm_instr_Kcondi:
    {
      if(instr->condi->instr->res->leaf.sig)
      {
        ccvm_enter(vm,instr->block[0]);
      } else
      {
        ccvm_enter(vm,instr->block[1]);
      }
    } break;
    case ccvm_instr_Kbinop:
    {
      instr->res=ccsvm_execbinary(irset,instr->oper,instr->lhs,instr->rhs);
    } break;
    default: ccassert(!"error");
  }
}

ccfunc void
ccvm_exit(ccvm_t *vm)
{
  vm->current=ccnil;
}

ccfunc void
ccvm_exec(ccvm_t *vm, ccvm_block_t *block)
{ ccvm_enter(vm,block);
  do
  { if(vm->curirix<ccarrlen(vm->current->instr))
    { ccvm_instr_t *ir=vm->current->instr+vm->curirix++;
      ccvm_exec_instr(vm,vm->current,ir);
    } else ccvm_exit(vm);
  } while(vm->current);
}

ccfunc int
ccvm_init(ccvm_t *vm)
{
  memset(vm,ccnil,sizeof(*vm));
  ccarrres(vm->globals,0xff);
  vm->entrybl=ccvm_block(ccnil,"main");
  vm->current=vm->entrybl;
  vm->curirix=0;
  return 1;
}

ccfunc int
ccsvm_exectree(cctree_t *tree)
{
  ccvm_t vm;
  ccvm_init(&vm);

  for(cctree_t *stat=tree; stat<ccarrend(tree); stat++)
  {

  	ccemit_tree(&vm,vm.current,stat);
  }

  ccvm_exec(&vm,vm.entrybl);

  ccout("Globals\n");
  { ccvm_value_t *it;
    ccarrfor(vm.globals,it) ccout(ccformat("%s=%i\n",it->name,it->leaf.sig));
  }
  return 1;
}

const char *ccvm_instr_S[]=
{ "STORE","BINOP","BLOCK","CONDI","ENTER","LEAVE"
};
const char *ccvm_value_S[]=
{ "LEAF","GLOBAL","BLOCK","INSTR",
};

ccfunc void
ccvmir_tos_(ccstr_t *buf, ccvm_instr_t *ir)
{
  if((ir->type==ccvm_instr_Kenter)||
     (ir->type==ccvm_instr_Kleave))
  {
    ccstrcatf(*buf,"%s: %s::%s", ccvm_instr_S[ir->type],
      ir->block[0]->super?ir->block[0]->super->debug_label:"",ir->block[0]->debug_label);
  } else
  if((ir->type==ccvm_instr_Kblock))
  {
    ccstrcatf(*buf,"%s: %s", ccvm_instr_S[ir->type], ir->block[0]->debug_label);
  } else
  { ccstrcat(*buf,ccvm_instr_S[ir->type]);
  }

}

ccfunc ccstr_t
ccvmir_tos(ccvm_instr_t *ir)
{
  ccstr_t buf=ccnil;
  ccvmir_tos_(&buf,ir);
  ccstrcatnl(buf);

  ccout(buf);

  ccstrdel(buf);

  return buf;
}
#endif