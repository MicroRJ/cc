#ifndef _CCSVM
#define _CCSVM

ccfunc ccstr_t ccvmir_tos(ccedict_t *ir);

ccfunc ccvalue_t *
ccsvm_execbinary(ccblock_t *irset, cctoken_t oper, ccvalue_t *lval, ccvalue_t *rval)
{
  (void) irset;

  if(!lval) cctraceerr("undefined lhs value"); // Todo: to string ...
  if(!rval) cctraceerr("undefined rhs value"); // Todo: to string ...

  ccassert(lval->kind==ccvalue_Kleaf||lval->kind==ccvalue_Kglobal);
  ccassert(rval->kind==ccvalue_Kleaf||rval->kind==ccvalue_Kglobal);

  if(oper.bit==cctoken_Kassign)
  { lval->leaf=rval->leaf;
    return lval;
  } else
  if(oper.bit==cctoken_Kequals)
  { return ccemit_const_i32(ccnil,lval->leaf.sig==rval->leaf.sig);
  } else
  if(oper.bit==cctoken_Kgreater_than)
  { return ccemit_const_i32(ccnil,lval->leaf.sig>rval->leaf.sig);
  } else
  if(oper.bit==cctoken_Kgreater_than_eql)
  { return ccemit_const_i32(ccnil,lval->leaf.sig>=rval->leaf.sig);
  } else
  if(oper.bit==cctoken_Kless_than)
  { return ccemit_const_i32(ccnil,lval->leaf.sig<rval->leaf.sig);
  } else
  if(oper.bit==cctoken_Kless_than_eql)
  { return ccemit_const_i32(ccnil,lval->leaf.sig<=rval->leaf.sig);
  } else
  if(oper.bit==cctoken_Kadd)
  { return ccemit_const_i32(ccnil,lval->leaf.sig+rval->leaf.sig);
  } else
  { ccassert(!"error");
    return ccnil;
  }
}

ccfunc void
ccvm_enter(ccexec_t *vm, ccblock_t *block)
{
  ccnotnil(block);

  vm->current=block;
  vm->curirix=0;
}

ccfunc void
ccvm_leave(ccexec_t *vm, ccblock_t *block)
{
  ccnotnil(block);

  vm->current=block->super;
}

ccfunc void
ccvm_exec_instr(ccexec_t *vm, ccblock_t *irset, ccedict_t *instr)
{
  switch(instr->type)
  {
    case ccedict_Klocal:
    {
    	// Todo:
    	ccvalue_t *local=instr->local;
    	ccnotnil(local);
    	ccassert(local->is_local);

    	local->address=ccmalloc(sizeof(ccclassic_t));

    	cctracelog("%s]LOCAL] [s:32] %s, %p", irset->debug_label,local->name,local->address);
    } break;
    case ccedict_Kstore:
    {
    	ccvalue_t *lhs,*rhs;
      lhs=instr->address;
      rhs=instr->value;

      ccassert(lhs->kind==ccvalue_Ktyped);
      ccnotnil(lhs);
      ccnotnil(rhs);

      ccclassic_t *address = ( ccclassic_t * )lhs->address;
      *address=rhs->classic;

    	cctracelog("%s::STORE($%s@%p,%p=%i)",
    		irset->debug_label,lhs->name,lhs->address,rhs,rhs->classic.as_i32);

      // if(rhs->kind==ccvalue_Kleaf)
      // { lhs->leaf=rhs->leaf;
      // } else
      // if(rhs->kind==ccvalue_Kinstr)
      // { lhs->leaf=rhs->instr->res->leaf;
      // }
      // instr->res=lhs;
    } break;

    case ccedict_Kblock:
    { ccedict_t *it;
      ccarrfor(instr->block[0]->instr,it)
      {
        ccvm_exec_instr(vm,irset,it);
      }
    } break;
    case ccedict_Kenter:
    { ccvm_enter(vm,instr->block[0]);
    } break;
    case ccedict_Kcondi:
    {
      if(instr->condi->instr->res->leaf.sig)
      {
        ccvm_enter(vm,instr->block[0]);
      } else
      {
        ccvm_enter(vm,instr->block[1]);
      }
    } break;
    case ccedict_Kbinop:
    {
      instr->res=ccsvm_execbinary(irset,instr->oper,instr->lhs,instr->rhs);
    } break;
    default: ccassert(!"error");
  }
}

ccfunc void
ccvm_exit(ccexec_t *vm)
{
  vm->current=ccnil;
}

ccfunc void
ccvm_exec(ccexec_t *vm, ccblock_t *block)
{ cctracelog("%s] l: %i, i: %i",block->debug_label,ccarrlen(block->local),ccarrlen(block->instr));

  ccvm_enter(vm,block);
  do
  {
    if(vm->curirix<ccarrlen(vm->current->instr))
    { ccedict_t *ir=vm->current->instr+vm->curirix++;
      ccvm_exec_instr(vm,vm->current,ir);
    } else ccvm_exit(vm);

  } while(vm->current);
}

ccfunc int
ccexec_init(ccexec_t *exec)
{ memset(exec,ccnil,sizeof(*exec));
  return cctrue;
}

ccfunc int
ccexec_translation_unit(ccexec_t *exec, ccemit_t *emit)
{
  exec->emit=emit;

  ccvalue_t *entryV=ccmapget(emit->globals,"main");
  if(!entryV) cctraceerr("missing entry point");

  ccfunction_t *entryF=entryV->function;

  ccvm_exec(exec,entryF->decls);

  return 1;
}

const char *ccvm_instr_S[]=
{ "STORE","LOCAL","BINOP","BLOCK","CONDI","ENTER","LEAVE","CALL","RETURN",
};
const char *ccvm_value_S[]=
{ "LEAF","GLOBAL","BLOCK","INSTR",
};

ccfunc void
ccvmir_tos_(ccstr_t *buf, ccedict_t *ir)
{
  if((ir->type==ccedict_Kenter)||
     (ir->type==ccedict_Kleave))
  {
    ccstrcatf(*buf,"%s: %s::%s", ccvm_instr_S[ir->type],
      ir->block[0]->super?ir->block[0]->super->debug_label:"",ir->block[0]->debug_label);
  } else
  if((ir->type==ccedict_Kblock))
  {
    ccstrcatf(*buf,"%s: %s", ccvm_instr_S[ir->type], ir->block[0]->debug_label);
  } else
  { ccstrcatS(*buf,ccvm_instr_S[ir->type]);
  }

}

ccfunc ccstr_t
ccvmir_tos(ccedict_t *ir)
{
  ccstr_t buf=ccnil;
  ccvmir_tos_(&buf,ir);
  ccstrcatnl(buf);

  ccout(buf);

  ccstrdel(buf);

  return buf;
}
#endif