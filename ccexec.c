#ifndef _CCSVM
#define _CCSVM

ccfunc ccstr_t ccvmir_tos(ccedict_t *ir);


ccfunc int
ccexec_yield(ccexec_t *exec, ccexec_value_t *val, ccemit_value_t *value)
{
  int i;
  ccexec_value_t *v=cctblgetP(exec->values,value,&i);
  if(i) memcpy(val,v,sizeof(*v));

  return i;
}

ccfunc ccexec_value_t *
ccexec_save(ccexec_t *exec, ccemit_value_t *value)
{
  int in_table;
  ccexec_value_t *store=cctblputP(exec->values,value,&in_table);

  ccassert(!in_table);
  return store;
}

ccfunc int
ccexec_rvalue(ccexec_t *exec, ccexec_value_t *rval, ccemit_value_t *val)
{
  switch(val->kind)
  { case ccvalue_kEDICT:
    { switch(val->edict->kind)
      { case ccedict_kFETCH:
          ccassert(ccexec_yield(exec,rval,val));
        break;
        case ccedict_kARITH:
          ccassert(ccexec_yield(exec,rval,val));
        break;
        default:
          ccassert(!"internal");
        return ccfalse;
      }
    } break;
    case ccvalue_kGLOBAL:
      ccassert(!"noimpl");
    break;
    case ccvalue_kCONST:
      rval->clsc=val->constant.clsc;
    break;
    default:
      ccassert(!"internal");
    return ccfalse;
  }
  return cctrue;
}

ccfunc void
ccexec_lvalue(ccexec_t *exec, ccexec_value_t *lval, ccemit_value_t *val)
{
  if(val->kind==ccvalue_kEDICT)
  { switch(val->edict->kind)
    { case ccedict_kLOCAL:
        ccassert(ccexec_yield(exec,lval,val));
      break;
      default: ccassert(!"internal");
    }
  } else ccassert(!"internal");
}





ccfunc void
ccexec_enter(ccexec_t *vm, ccblock_t *block)
{
  ccnotnil(block);
  vm->current=block;
  vm->curirix=0;
}

//
ccfunc ccexec_value_t *
ccexec_edict_arith(ccexec_t *exec, ccemit_value_t *val)
{
  ccexec_value_t *result={};

  cctoken_k       opr;
  ccemit_value_t *lhs,*rhs;

  opr=val->edict->arith.opr;
  lhs=val->edict->arith.lhs;
  rhs=val->edict->arith.rhs;

  ccnotnil(lhs);
  ccnotnil(rhs);

  if(opr==cctoken_kASSIGN)
  { ccassert(!"internal");
    // lhs->leaf=rhs->leaf;
  } else
  if(opr==cctoken_Kequals)
  { // return ccemit_const_i32(ccnil,lhs->leaf.sig==rhs->leaf.sig);
  } else
  if(opr==cctoken_Kgreater_than)
  { // return ccemit_const_i32(ccnil,lhs->leaf.sig>rhs->leaf.sig);
  } else
  if(opr==cctoken_Kgreater_than_eql)
  { // return ccemit_const_i32(ccnil,lhs->leaf.sig>=rhs->leaf.sig);
  } else
  if(opr==cctoken_Kless_than)
  { // return ccemit_const_i32(ccnil,lhs->leaf.sig<rhs->leaf.sig);
  } else
  if(opr==cctoken_Kless_than_eql)
  { // return ccemit_const_i32(ccnil,lhs->leaf.sig<=rhs->leaf.sig);
  } else
  if(opr==cctoken_Kadd)
  {
    result=ccexec_save(exec,val);

    ccexec_value_t lval,rval;
    ccassert(ccexec_rvalue(exec,&lval,lhs));
    ccassert(ccexec_rvalue(exec,&rval,rhs));

    result->clsc.as_i32=
        lval.clsc.as_i32 +
        rval.clsc.as_i32;

    cctracelog("ADD: %i, %i; 0x%x=%i",
      lval.clsc.as_i32,
      rval.clsc.as_i32, result,result->clsc.as_i32);
  } else
  { ccassert(!"error");
  }
  return result;
}

ccfunc void
ccexec_edict(ccexec_t *exec, ccblock_t *irset, ccemit_value_t *value)
{
  (void)irset;

  ccnotnil(value);
  ccassert(value->kind==ccvalue_kEDICT);

  ccedict_t *edict=value->edict;

  switch(edict->kind)
  { case ccedict_kLOCAL:
    { // Todo: stack alloc
      ccexec_value_t *saved=ccexec_save(exec,value);
      saved->addr=ccmalloc(sizeof(ccclassic_t));
      saved->type=edict->local.type;
      saved->debug_label=edict->local.debug_label;
      cctracelog("LOCAL: $%s %p: {%p}=%p", saved->debug_label,edict,saved,saved->addr);
    } break;
    case ccedict_kFETCH:
    {
      ccexec_value_t addrv;
      ccexec_yield(exec,&addrv,edict->fetch.rval);

      ccexec_value_t *saved=ccexec_save(exec,value);
      saved->type=addrv.type; // Todo:
      saved->addr=addrv.addr;
    } break;
     case ccedict_kENTER:
    { ccnotnil(edict->enter.blc);
      ccexec_enter(exec,edict->enter.blc);
    } break;
    case ccedict_kSTORE:
    {
      ccexec_value_t lval,rval;
      ccexec_lvalue(exec,&lval,ccnotnil(edict->store.lval));
      ccexec_rvalue(exec,&rval,ccnotnil(edict->store.rval));

      cctracelog("STORE: $%s %p, %i",lval.debug_label,lval.addr,rval.clsc.as_i32);

      // Todo:
      *cccast(cci32*,lval.addr)=rval.clsc.as_i32;

      ccexec_value_t *saved=ccexec_save(exec,value);
      saved->type=lval.type;
      saved->clsc=rval.clsc;
    } break;
    case ccedict_kARITH:
    {
      ccexec_edict_arith(exec,value);
    } break;
#if 0

    case ccedict_kBLOCK:
    { ccedict_t *it;
      ccarrfor(instr->block[0]->instr,it)
      {
        ccexec_edict(vm,irset,it);
      }
    } break;

    case ccedict_kCONDI:
    {
      if(instr->condi->instr->res->leaf.sig)
      {
        ccexec_enter(vm,instr->block[0]);
      } else
      {
        ccexec_enter(vm,instr->block[1]);
      }
    } break;
    case ccedict_kARITH:
    {
      instr->res=ccexec_edict_arith(irset,instr->oper,instr->lhs,instr->rhs);
    } break;
#endif
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
{ cctracelog("%s] l: %i, i: %i",block->label,ccarrlen(block->local),ccarrlen(block->edict));
  ccexec_enter(vm,block);
  do
  { if(vm->curirix<ccarrlen(vm->current->edict))
    {
      ccemit_value_t *ir=vm->current->edict+vm->curirix++;
      ccexec_edict(vm,vm->current,ir);

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

  int in_table;
  ccemit_value_t *entryV=cctblgetL(emit->globals,"main",&in_table);

  if(in_table)
  { ccfunction_t *entryF=entryV->function;
    ccvm_exec(exec,entryF->decls);
  } else cctraceerr("missing entry point");

  return 1;
}

#if 0

const char *ccvm_instr_S[]=
{ "STORE","LOCAL","BINOP","BLOCK","CONDI","ENTER","LEAVE","CALL","RETURN",
};
const char *ccvm_value_S[]=
{ "LEAF","GLOBAL","BLOCK","INSTR",
};

ccfunc void
ccvmir_tos_(ccstr_t *buf, ccedict_t *ir)
{
  if((ir->type==ccedict_kENTER)||
     (ir->type==ccedict_Kleave))
  {
    ccstrcatf(*buf,"%s: %s::%s", ccvm_instr_S[ir->type],
      ir->block[0]->super?ir->block[0]->super->debug_label:"",ir->block[0]->debug_label);
  } else
  if((ir->type==ccedict_kBLOCK))
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

#endif