#ifndef _CCEXEC
#define _CCEXEC
// Todo: fix constants,
// Todo: legit jump instruction and conditional jump?
// Todo: faster string allocations
// Todo: Legit stack allocator along with revamped mingle system
// Todo: Legit type system

// Note: associates a value with an edict, as you can see, I have peculiar choice of words ...
ccfunc ccexec_value_t *
ccstack_mingle(ccexec_stack_t *stack, ccemit_value_t *value)
{
  ccassert(value!=0);


  ccexec_value_t *result=cctblputP(stack->values,value);
  ccassert(ccerrnon());

  return result;
}

// Note: yields the associated execution time value ...
ccfunc ccexec_value_t
ccstack_yield(ccexec_stack_t *stack, ccemit_value_t *value)
{
  ccassert(value!=0);

  ccexec_value_t *result=cctblgetP(stack->values,value);
  ccassert(ccerrnon());

  if((result->kind==ccev_kINVALID))
  	cctraceerr("value kind is invalid, did you register this value and not set its contents?");

  ccassert(result->kind!=ccev_kINVALID);

  return *result;
}

ccfunc ccexec_value_t
ccstack_yield_rvalue(
  ccexec_stack_t *stack, ccemit_value_t *couple)
{
  ccexec_value_t result={};

  switch(couple->kind)
  { case ccvalue_kEDICT:
    { switch(couple->edict->kind)
      { case ccedict_kFETCH:
        case ccedict_kARITH:
        case ccedict_kINVOKE:
          result=ccstack_yield(stack,couple);
        break;
      }
    } break;
    // Todo:
    case ccvalue_kCONST:
      // Todo: this is ridiculous
      result=ccexec_value_I(ccev_kRVALUE,couple->constant.type,cccast(void*,couple->constant.clsc.as_i64),"constant-value");
    break;
  }

  ccassert(result.kind==ccev_kRVALUE);

  return result;
}

ccfunc ccexec_value_t
ccstack_yield_lvalue(ccexec_stack_t *stack, ccemit_value_t *couple)
{
  ccexec_value_t result={};

  if(couple->kind==ccvalue_kEDICT)
  { switch(couple->edict->kind)
    { case ccedict_kLOCAL:
      case ccedict_kPARAM:
        result=ccstack_yield(stack,couple);
      break;
    }
  }

  ccassert(result.kind==ccev_kLVALUE);
  return result;
}

// Note: allocates an execution time addressable l-value on the stack and associates it with the given value...
ccfunc ccexec_value_t *
ccstack_local_alloc(
  ccexec_stack_t *stack, ccemit_value_t *value)
{
  ccnotnil((value));
  ccassert((value->kind==ccvalue_kEDICT),
    "cannot allocate local, expected a value of type EDICT and of subtype LOCAL or PARAM");

  ccedict_t *edict=value->edict;

  ccnotnil((edict));
  ccassert((edict->kind==ccedict_kLOCAL)||(edict->kind==ccedict_kPARAM),
    "cannot allocate local, expected an edict of type LOCAL or PARAM");

  // Todo:
  ccexec_value_t *result=ccstack_mingle(stack,value);
  result->value=ccmalloc(sizeof(ccclassic_t));
  result->kind=ccev_kLVALUE;
  result->type=edict->local.type;

  // Todo:
  result->label=ccnil;
  ccstrcatf(result->label,"'%s':localloc",edict->label);
  return result;
}

ccfunc cci32_t
ccexec_load(ccexec_stack_t *stack, cctree_t *type, void *address)
{
  return ccdref(cccast(cci32_t *,address));
}

ccfunc void
ccexec_enter(ccexec_stack_t *stack, ccemit_block_t *block)
{
  stack->current=block;
  stack->irindex=0;
}

//
ccfunc ccexec_value_t *
ccexec_edict_arith(ccexec_stack_t *stack, ccemit_value_t *val)
{
  ccexec_value_t *result={};

  cctoken_k       opr;
  ccemit_value_t *lhs,*rhs;

  opr=val->edict->arith.opr;
  lhs=val->edict->arith.lhs;
  rhs=val->edict->arith.rhs;

  ccnotnil(lhs);
  ccnotnil(rhs);

  ccexec_value_t lval,rval;
  lval=ccstack_yield_rvalue(stack,lhs);
  rval=ccstack_yield_rvalue(stack,rhs);


  if(opr==cctoken_kASSIGN)
  { ccassert(!"internal");
    // lhs->leaf=rhs->leaf;
  } else
  if(opr==cctoken_Kequals)
  { // return ccemit_const_i32(ccnil,lhs->leaf.sig==rhs->leaf.sig);
  } else
  if(opr==cctoken_Kgreater_than)
  { result=ccstack_mingle(stack,val);
    result->kind=ccev_kRVALUE;
    result->label="greater-than";
    result->asi32=lval.asi32>rval.asi32;
  } else
  if(opr==cctoken_Kgreater_than_eql)
  { result=ccstack_mingle(stack,val);
    result->kind=ccev_kRVALUE;
    result->label="greater-than-eql";
    result->asi32=lval.asi32>=rval.asi32;
  } else
  if(opr==cctoken_Kless_than)
  { // return ccemit_const_i32(ccnil,lhs->leaf.sig<rhs->leaf.sig);
  } else
  if(opr==cctoken_Kless_than_eql)
  {
  } else
  if(opr==cctoken_Ksub)
  { result=ccstack_mingle(stack,val);
    result->kind=ccev_kRVALUE;
    result->label="sub";
    result->asi32=lval.asi32-rval.asi32;
  } else
  if(opr==cctoken_Kadd)
  { result=ccstack_mingle(stack,val);
    result->kind=ccev_kRVALUE;
    result->label="add";
    result->asi32=lval.asi32+rval.asi32;
#if 0
    cctracelog("ADD: %i, %i; 0x%x=%i",
      lval.clsc.as_i32,
      rval.clsc.as_i32, result,result->clsc.as_i32);
#endif
  } else
  { ccassert(!"error");
  }

  ccassert(result!=ccnil);
  return result;
}

ccfunc int
ccexec_invoke(
  ccexec_t *exec, ccemit_value_t *val, ccexec_value_t *ret, ccexec_value_t *in);

ccfunc int
ccexec_edict(
  ccexec_t *exec, ccexec_stack_t *stack, ccemit_value_t *value)
{
  ccnotnil(stack);

  ccnotnil(value);
  ccassert(value->kind==ccvalue_kEDICT);

  ccedict_t *edict=value->edict;

  switch(edict->kind)
  {
    // Note: this is simply to ensure we've set the parameters ...
    case ccedict_kPARAM:
    { ccstack_yield(stack,value);
    } break;

    case ccedict_kLOCAL:
    { ccstack_local_alloc(stack,value);
    } break;

    case ccedict_kFETCH:
    {
      ccexec_value_t lval;
      lval=ccstack_yield_lvalue(stack,ccnotnil(edict->fetch.lval));

      ccexec_value_t *saved=ccstack_mingle(stack,value);
      saved->kind=ccev_kRVALUE;
      saved->type=lval.type;
      saved->asi64=ccexec_load(stack,lval.type,lval.value);
      saved->label=ccnil;
      ccstrcatf(saved->label,"fetch: %s",lval.label);
    } break;
    case ccedict_kSTORE:
    {
      ccexec_value_t lval,rval;
      lval=ccstack_yield_lvalue(stack,ccnotnil(edict->store.lval));
      rval=ccstack_yield_rvalue(stack,ccnotnil(edict->store.rval));

      // Todo:
      *cccast(cci32_t*,lval.value)=rval.asi32;

      ccexec_value_t *saved=ccstack_mingle(stack,value);
      saved->kind=ccev_kRVALUE;
      saved->type=lval.type;
      saved->value=rval.value;
      saved->label=rval.label;
    } break;
    case ccedict_kENTER:
    { ccnotnil(edict->enter.blc);
      ccexec_enter(stack,edict->enter.blc);
    } break;
    case ccedict_kARITH:
    {
      ccexec_edict_arith(stack,value);
    } break;
    case ccedict_kRETURN:
    {
      // Note: save the return value under the return instruction value ...
      ccexec_value_t *saved=ccstack_mingle(stack,value);
      *saved=ccstack_yield_rvalue(stack,edict->ret.rval);
    } return ccfalse;

    case ccedict_kINVOKE:
    {
      ccassert(edict->invoke.call!=0);

      ccexec_value_t  *rval=ccnil;
      ccemit_value_t **list=ccnil;
      ccarrfor(edict->invoke.rval,list)
        *ccarrone(rval)=ccstack_yield_rvalue(stack,*list);

      // Note: save the return value ...
      ccexec_value_t *ret=ccstack_mingle(stack,value);
      if(!ccexec_invoke(exec,edict->invoke.call,ret,rval))
      {
      	ccassert(!"no-return value, error");
      }

      ccarrdel(rval);
    } break;
    case ccedict_kTERNARY:
    {
      ccexec_value_t rval;
      rval=ccstack_yield_rvalue(stack,ccnotnil(edict->ternary.cnd));

      // Todo: is this flawed?
      if(rval.asi32)
        ccexec_enter(stack,ccnotnil(edict->ternary.lhs));
      else
        ccexec_enter(stack,ccnotnil(edict->ternary.rhs));

    } break;
    default: ccassert(!"error");
  }
  return cctrue;
}

ccfunc int
ccexec_invoke(
  ccexec_t *exec, ccemit_value_t *value, ccexec_value_t *ret, ccexec_value_t *arguments)
{
  ccnotnil(exec);
  ccnotnil(value);

  ccexec_stack_t stack={};

  ccemit_procd_t *func=value->func;
  cctree_t *type=func->tree->type;

  ccassert(ccarrlen(type->list)==ccarrlen(arguments));

  cctree_t **lval;
  ccarrfor(type->list,lval)
  {
    ccemit_value_t *local=ccfunc_local(func,*lval);
    ccexec_value_t *rval=ccstack_local_alloc(&stack,local);

    cci32_t int_value=arguments->asi32;
    ccdref(cccast(cci32_t*,rval->value))=int_value;
    arguments++;
  }

  ccexec_enter(&stack,func->decls);

  while(stack.irindex<ccarrlen(stack.current->edict))
  {
    ccemit_value_t **it=stack.current->edict+stack.irindex;
    stack.irindex++;

    if(!ccexec_edict(exec,&stack,*it))
    {
      // Note: is this flawed?
      ccedict_t *edict=(*it)->edict;

      if(edict->kind==ccedict_kRETURN)
        *ret=ccstack_yield(&stack,*it);
      return cctrue;
    }
  }


  ccarrdel(stack.values);

  return ccfalse;
}

ccfunc int
ccexec_init(ccexec_t *exec)
{ memset(exec,ccnil,sizeof(*exec));
  return cctrue;
}


ccfunc int
ccfib(int x)
{
	if(x<2) return x;

	return ccfib(x-2)+ccfib(x-1);
}



ccfunc int
ccexec_translation_unit(ccexec_t *exec, ccemit_t *emit)
{
  exec->emit=emit;

  ccexec_value_t *args=ccnil;
  ccexec_value_t *arg=ccarrone(args);
  arg->kind=ccev_kRVALUE;
  arg->asi32=22;

  ccu64_t cc_cs=ccclocktick();
  ccexec_value_t ret;
  ccexec_invoke(exec,emit->entry,&ret,args);
  ccu64_t cc_ce=ccclocktick();

  ccu64_t c_cs=ccclocktick();
  int c=ccfib(arg->asi32);
  ccu64_t c_ce=ccclocktick();


	printf("fib c:%i %f(s) - ccvm:%i %f(s)\n",
		c, ccclocksecs(c_ce-c_cs), ret.asi32, ccclocksecs(cc_ce-cc_cs));
  return 1;
}

#endif