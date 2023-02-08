#ifndef _CCEXEC
#define _CCEXEC

ccfunc ccstr_t ccvmir_tos(ccedict_t *ir);

ccfunc void
ccexec_yield(ccexec_t *exec, ccexec_value_t *val, ccemit_value_t *value)
{
  ccexec_value_t *v=cctblgetP(exec->values,value);

  if(ccerrnon())
  	if(val) memcpy(val,v,sizeof(*v));
  else
  	cctraceerr("invalid yield, no entry for value 0x%x %i", value, value);
}

ccfunc ccexec_value_t *
ccexec_save(ccexec_t *exec, ccemit_value_t *value)
{
  ccexec_value_t *v=cctblputP(exec->values,value);
  ccassert(ccerrnon());

  return v;
}

ccfunc int
ccexec_rvalue(ccexec_t *exec, ccexec_value_t *rval, ccemit_value_t *val)
{
  switch(val->kind)
  { case ccvalue_kEDICT:
    { switch(val->edict->kind)
      { case ccedict_kFETCH:
          ccexec_yield(exec,rval,val);
        break;
        case ccedict_kARITH:
          ccexec_yield(exec,rval,val);
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
		// Todo:
    	*rval=ccexec_value_I(ccexec_value_kCONST,
    		val->constant.type,
    		cccast(void*,val->constant.clsc.as_i64),
    		"constant-value");
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
    	case ccedict_kPARAM:
        ccexec_yield(exec,lval,val);
      break;
      default: ccassert(!"internal");
    }
  } else ccassert(!"internal");
}

ccfunc cci32_t
ccexec_load(ccexec_t *exec, cctype_t *type, void *address)
{
	return ccdref(cccast(cci32_t *,address));
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
    result->kind=ccexec_value_kCONST;
    result->debug_label="arith";

    ccexec_value_t lval,rval;
    ccassert(ccexec_rvalue(exec,&lval,lhs));
    ccassert(ccexec_rvalue(exec,&rval,rhs));
    ccassert(lval.kind==ccexec_value_kCONST);
    ccassert(rval.kind==ccexec_value_kCONST);

    result->asi32=lval.asi32+rval.asi32;
#if 0
    cctracelog("ADD: %i, %i; 0x%x=%i",
      lval.clsc.as_i32,
      rval.clsc.as_i32, result,result->clsc.as_i32);
#endif
  } else
  { ccassert(!"error");
  }
  return result;
}

ccfunc ccexec_value_t *
ccexec_local(ccexec_t *exec, ccemit_value_t *value)
{
	ccnotnil((value));
  ccassert((value->kind==ccvalue_kEDICT),
  	"cannot create local from given value, expected a value of type EDICT and of subtype LOCAL or PARAM");

  ccedict_t *edict=value->edict;

	ccnotnil((edict));
  ccassert((edict->kind==ccedict_kLOCAL)||(edict->kind==ccedict_kPARAM),
    "cannot create local from given value, expected an edict of type LOCAL or PARAM");

	// Todo: stack alloc
	ccexec_value_t *saved=ccexec_save(exec,value);
  saved->value       = ccmalloc(sizeof(ccclassic_t));
  saved->kind        = ccexec_value_kADDRS;
  saved->type        = edict->local.type;
  saved->debug_label = edict->local.debug_label;

  cctracelog("%s[0x%x: $%s 0x%x",
  	ccedict_s[edict->kind],value,saved->debug_label,saved->value);

  return saved;
}

ccfunc void
ccexec_edict(ccexec_t *exec, ccblock_t *irset, ccemit_value_t *value)
{
  (void)irset;

  ccnotnil(value);
  ccassert(value->kind==ccvalue_kEDICT);

  ccedict_t *edict=value->edict;

  switch(edict->kind)
  {
  	case ccedict_kLOCAL:
    { ccexec_local(exec,value);
    } break;
    // Note: this is simply to ensure we've set the parameters ...
    case ccedict_kPARAM:
    { ccexec_value_t lval;
    	ccexec_yield(exec,&lval,value);
    } break;
    case ccedict_kFETCH:
    {
      ccexec_value_t lval;
      ccexec_lvalue(exec,&lval,edict->fetch.lval);

      ccassert(lval.kind==ccexec_value_kADDRS);

      ccexec_value_t *saved=ccexec_save(exec,value);
      saved->kind=ccexec_value_kCONST;
      saved->type=lval.type;
      saved->asi64=ccexec_load(exec,lval.type,lval.value);
      saved->debug_label=lval.debug_label;
    } break;
    case ccedict_kSTORE:
    {
      ccexec_value_t lval,rval;
      ccexec_lvalue(exec,&lval,ccnotnil(edict->store.lval));
      ccexec_rvalue(exec,&rval,ccnotnil(edict->store.rval));

      ccassert(lval.kind==ccexec_value_kADDRS);
      ccassert(rval.kind==ccexec_value_kCONST);

      cctracelog("STORE[0x%x: $%s 0x%x=%i",
      	value,lval.debug_label,lval.value,rval.asi32);

      // Todo:
      *cccast(cci32_t*,lval.value)=rval.asi32;

      ccexec_value_t *saved=ccexec_save(exec,value);
      saved->kind=ccexec_value_kCONST;
      saved->type=lval.type;
      saved->value=rval.value;
      saved->debug_label=rval.debug_label;
    } break;
    case ccedict_kENTER:
    { ccnotnil(edict->enter.blc);
      ccexec_enter(exec,edict->enter.blc);
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
{ ccexec_enter(vm,block);
  do
  { if(vm->curirix<ccarrlen(vm->current->edict))
    {
      ccemit_value_t **ir=vm->current->edict+vm->curirix++;
      ccexec_edict(vm,vm->current,*ir);

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

  ccemit_value_t *entryV=*cctblgetL(emit->globals,"main");
  if(!ccerrnon()) cctraceerr("missing entry point");

  ccfunction_t *entryF=entryV->function;

  cctree_t **lval;
  ccarrfor(entryF->tree->type->list,lval)
  {
  	ccemit_value_t *local=ccfunc_local(entryF,*lval);
  	ccexec_value_t *rval=ccexec_local(exec,local);
  	ccdref(cccast(cci32_t*,rval->value))=1;
  }

  ccvm_exec(exec,entryF->decls);

  return 1;
}

#endif