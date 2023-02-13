#ifndef _CCEXEC
#define _CCEXEC

// Todo: faster string allocations
// Todo: Legit stack allocator along with revamped mingle system
// Todo: Legit type system

ccfunc ccu32_t
ccexec_sizeof(ccexec_frame_t *, cctype_t *);

// Note: associates a value with an edict, as you can see, I have peculiar choice of words ...
ccfunc ccexec_value_t *
ccstack_mingle(ccexec_frame_t *stack, ccemit_value_t *value)
{
  ccassert(value!=0);

  ccexec_value_t *result=cctblsetP(stack->values,value);
  // ccassert(ccerrnon());

  return result;
}

// Note: yields the associated execution time value ...
ccfunc ccexec_value_t
ccstack_yield(ccexec_frame_t *stack, ccemit_value_t *value)
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
  ccexec_frame_t *stack, ccemit_value_t *couple)
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
    case ccvalue_kCONST:
      result=ccexec_rvalue(
        cccast(void*,couple->constant.clsc.asi64),"constant-value");
    break;
  }

  ccassert(result.kind==ccev_kRVALUE);

  return result;
}

ccfunc ccexec_value_t
ccstack_yield_lvalue(ccexec_frame_t *stack, ccemit_value_t *couple)
{
  ccexec_value_t result={};

  if(couple->kind==ccvalue_kEDICT)
  { switch(couple->edict->kind)
    { case ccedict_kLOCAL:
      case ccedict_kPARAM:
      case ccedict_kADDRESS:
        result=ccstack_yield(stack,couple);
      break;
    }
  }

  ccassert(result.kind==ccev_kLVALUE);
  return result;
}

ccfunc void
ccstack_pull(
  ccexec_t *exec, int length)
{
  exec->stack_idx-=sizeof(ccexec_value_t)*length;
}

ccfunc ccinle ccexec_value_t *
ccstack_push(
  ccexec_t *exec, int length)
{
  ccexec_value_t *memory=cccast(ccexec_value_t*,cccast(char*,exec->stack)+exec->stack_idx);
  exec->stack_idx+=sizeof(ccexec_value_t)*length;
  return memory;
}

// Note: allocates an execution time addressable l-value on the stack and associates it with the given value...
ccfunc ccinle ccexec_value_t *
ccstack_local_alloc(
  ccexec_t *exec, ccexec_frame_t *stack, ccemit_value_t *value)
{
  ccnotnil((value));
  ccassert((value->kind==ccvalue_kEDICT),
    "cannot allocate local, expected a value of type EDICT and of subtype LOCAL or PARAM");

  ccedict_t *edict=value->edict;

  ccnotnil((edict));
  ccassert((edict->kind==ccedict_kLOCAL)||(edict->kind==ccedict_kPARAM),
    "cannot allocate local, expected an edict of type LOCAL or PARAM");

  ccu32_t size=ccexec_sizeof(stack,edict->local.type);
  ccassert(size>=8);

  char *memory=cccast(char*,exec->stack)+exec->stack_idx;
  memset(memory,ccnil,size);
  exec->stack_idx+=size;

  // Todo: stack alloc dude!
  ccexec_value_t *result=ccstack_mingle(stack,value);
  *result=ccexec_lvalue(memory,"local_alloc");

  // cctracelog("'%s': %p",value->label,result->value);
  return result;
}

ccfunc ccinle void
ccexec_enter(ccexec_frame_t *stack, ccemit_block_t *block)
{
  stack->current=block;
  stack->irindex=0;
}

//
ccfunc ccinle ccexec_value_t
ccexec_edict_arith(cctoken_k opr, ccexec_value_t lval, ccexec_value_t rval)
{
  switch(opr)
  { case cctoken_kGTN:
      return ccexec_rvalue(cccast(void*,lval.asi64>rval.asi64),">");
    case cctoken_kGTE:
      return ccexec_rvalue(cccast(void*,lval.asi64>=rval.asi64),">=");
    case cctoken_kLTN:
      return ccexec_rvalue(cccast(void*,lval.asi64<rval.asi64),"<");
    case cctoken_kLTE:
      return ccexec_rvalue(cccast(void*,lval.asi64<=rval.asi64),"<=");
    case cctoken_kMUL:
      return ccexec_rvalue(cccast(void*,lval.asi64*rval.asi64),"*");
    case cctoken_kDIV:
      return ccexec_rvalue(cccast(void*,lval.asi64/rval.asi64),"/");
    case cctoken_kSUB:
      return ccexec_rvalue(cccast(void*,lval.asi64-rval.asi64),"-");
    case cctoken_kADD:
      return ccexec_rvalue(cccast(void*,lval.asi64+rval.asi64),"+");
    default: ccassert(!"error");
  }
  return ccexec_rvalue(cccast(void*,0),"internal-error");
}

ccfunc int
ccexec_invoke(
  ccexec_t *e, ccemit_procd_t *p, ccexec_value_t *r, int l, ccexec_value_t *i);

ccfunc int
ccexec_edict(
  ccexec_t *exec, ccexec_frame_t *stack, ccemit_value_t *value)
{
  // Todo:
  ccglobal ccexec_value_t zro=ccexec_rvalue(cccast(void*,0),"zro");

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
    { ccstack_local_alloc(exec,stack,value);
    } break;
    case ccedict_kADDRESS:
    { ccemit_value_t *lvalue,*rvalue;
      lvalue=edict->addr.lval;
      rvalue=edict->addr.rval;

      ccassert(lvalue!=ccnil);
      ccassert(rvalue!=ccnil);

      ccexec_value_t lval,rval;
      lval=ccstack_yield_lvalue(stack,lvalue);
      rval=ccstack_yield_rvalue(stack,rvalue);

      cci64_t *memory=cccast(cci64_t*,lval.value);
      memory+=rval.asi64;

      ccexec_value_t *saved=ccstack_mingle(stack,value);
      *saved=ccexec_lvalue(memory,"address");
    } break;
    case ccedict_kFETCH:
    {
      ccemit_value_t *lvalue,*rvalue;
      lvalue=edict->fetch.lval;
      rvalue=edict->fetch.rval;

      ccexec_value_t lval,rval;
      lval=ccstack_yield_lvalue(stack,lvalue);

      if(rvalue)
        rval=ccstack_yield_rvalue(stack,rvalue);
      else
        rval=zro;

      cci64_t *memory=cccast(cci64_t*,lval.value);
      memory+=rval.asi64;

      ccexec_value_t *saved=ccstack_mingle(stack,value);
      *saved=ccexec_rvalue(cccast(void*,ccdref(memory)),"fetch");
    } break;
    case ccedict_kSTORE:
    {
      ccexec_value_t lval,rval;
      lval=ccstack_yield_lvalue(stack,ccnotnil(edict->store.lval));
      rval=ccstack_yield_rvalue(stack,ccnotnil(edict->store.rval));

      // Todo:
      *cccast(cci64_t*,lval.value)=rval.asi64;

      ccexec_value_t *saved=ccstack_mingle(stack,value);
      saved->kind=ccev_kRVALUE;
      saved->value=rval.value;
      saved->label=rval.label;
    } break;
    case ccedict_kJUMP:
    {
#ifdef _HARD_DEBUG
      ccnotnil(edict->jump.blc);
      ccnotnil(edict->jump.tar);
      ccnotnil(edict->jump.tar);
#endif
      stack->current=edict->jump.blc;
      stack->irindex=edict->jump.tar;
    } break;
    case ccedict_kJUMPF:
    {
#ifdef _HARD_DEBUG
      ccnotnil(edict->jump.blc);
      ccnotnil(edict->jump.tar);
      ccnotnil(edict->jump.cnd);
#endif
      ccexec_value_t
      rval=ccstack_yield_rvalue(stack,ccnotnil(edict->jump.cnd));
      if(!rval.asi32)
      { stack->current=edict->jump.blc;
        stack->irindex=edict->jump.tar;
      }
    } break;
    case ccedict_kJUMPT:
    {
#ifdef _HARD_DEBUG
      ccnotnil(edict->jump.blc);
      ccnotnil(edict->jump.tar);
      ccnotnil(edict->jump.cnd);
#endif
      ccexec_value_t
      rval=ccstack_yield_rvalue(stack,ccnotnil(edict->jump.cnd));
      if(rval.asi32)
      { stack->current=edict->jump.blc;
        stack->irindex=edict->jump.tar;
      }
    } break;
    case ccedict_kARITH:
    {
#ifdef _HARD_DEBUG
      ccnotnil(edict->arith.opr);
      ccnotnil(edict->arith.lhs);
      ccnotnil(edict->arith.rhs);
#endif
      ccexec_value_t lval,rval;
      lval=ccstack_yield_rvalue(stack,edict->arith.lhs);
      rval=ccstack_yield_rvalue(stack,edict->arith.rhs);

      ccexec_value_t *saved=ccstack_mingle(stack,value);
      *saved=ccexec_edict_arith(edict->arith.opr,lval,rval);
    } break;
    case ccedict_kRETURN:
    {
      // Note: save the return value under the return instruction value ...
      ccexec_value_t *saved=ccstack_mingle(stack,value);
      *saved=ccstack_yield_rvalue(stack,edict->ret.rval);
    } return ccfalse;

    case ccedict_kINVOKE:
    {
#ifdef _HARD_DEBUG
      ccnotnil(edict->invoke.call);
#endif

#if 0
      ccexec_value_t  *rval=ccnil;
      ccemit_value_t **list=ccnil;
      ccarrfor(edict->invoke.rval,list)
        *ccarrone(rval)=ccstack_yield_rvalue(stack,*list);
#endif
      int rlen=ccarrlen(edict->invoke.rval);
      ccexec_value_t *rval=ccstack_push(exec,rlen);
      ccexec_value_t *setr=rval;
      ccemit_value_t **list=ccnil;
      ccarrfor(edict->invoke.rval,list)
        *setr++=ccstack_yield_rvalue(stack,*list);

      // Note: save the return value ...
      ccexec_value_t *ret=ccstack_mingle(stack,value);
      if(!ccexec_invoke(exec,edict->invoke.call,ret,rlen,rval))
      {
        ccassert(!"no-return value, error");
      }

      ccstack_pull(exec,rlen);
    } break;

    // Todo: to be removed ...
    case ccedict_kENTER:
    {
#ifdef _HARD_DEBUG
      ccnotnil(edict->enter.blc);
#endif
      ccexec_enter(stack,edict->enter.blc);
    } break;
    case ccedict_kTERNARY:
    { ccassert(!"deprecated");

      ccexec_value_t rval;
      rval=ccstack_yield_rvalue(stack,ccnotnil(edict->ternary.cnd));

      // Todo: is this flawed?
      if(rval.asi32)
      { if(edict->ternary.lhs) ccexec_enter(stack,edict->ternary.lhs);
      }
      else
      { if(edict->ternary.rhs) ccexec_enter(stack,edict->ternary.rhs);
      }
    } break;
    default: ccassert(!"error");
  }
  return cctrue;
}

ccfunc ccu32_t
ccexec_sizeof(ccexec_frame_t *_s, cctype_t *_t)
{
  if(_t->kind==cctype_kARRAY)
  {
    ccexec_value_t rval;
    rval=ccstack_yield_rvalue(_s,_t->length);

    ccu32_t size=ccexec_sizeof(_s,_t->type);
    size*=rval.asu32;

    return size;
  } else
  if(_t->kind==cctype_kINTEGER)
  { return sizeof(cci64_t);
  }
  ccassert(!"error");
  return 0;
}

ccfunc int
ccexec_invoke(
  ccexec_t *exec, ccemit_procd_t *procd, ccexec_value_t *r, int l, ccexec_value_t *i)
{
  // Todo: let's not use our actual stack ...
  ccexec_frame_t stack={};

  cctree_t *type=procd->tree->type;

  ccassert((int)ccarrlen(type->list)==l);

ccenter("setargs");
  cctree_t **lval;
  ccarrfor(type->list,lval)
  {
    ccemit_value_t *local=ccprocd_local(procd,*lval);
    ccexec_value_t *rval=ccstack_local_alloc(exec,&stack,local);

    cci32_t int_value=i->asi32;
    ccdref(cccast(cci32_t*,rval->value))=int_value;
    i++;
  }
ccleave("setargs");

  ccexec_enter(&stack,procd->decls);

  while(stack.irindex<ccarrleni(stack.current->edict))
  {
    ccemit_value_t **it=stack.current->edict+stack.irindex;
    stack.irindex++;

    if(!ccexec_edict(exec,&stack,*it))
    {
      // Note: is this flawed?
      ccedict_t *edict=(*it)->edict;

      if(edict->kind==ccedict_kRETURN)
        *r=ccstack_yield(&stack,*it);
      return cctrue;
    }
  }

  ccarrdel(stack.values);

  return ccfalse;
}

ccfunc ccexec_value_t
ccexec_translation_unit(ccexec_t *exec, ccemit_t *emit)
{
  exec->emit=emit;

  ccexec_value_t *args=ccnil;
  *ccarrone(args)=ccexec_rvalue(cccast(void*,ARG),"arg-0");

  ccexec_value_t ret;
  ccexec_invoke(exec,emit->entry,&ret,ccarrlen(args),args);

  return ret;
}

ccfunc int
ccexec_init(ccexec_t *exec)
{ memset(exec,ccnil,sizeof(*exec));

  exec->stack=ccmalloc(0x40000000);
  exec->stack_sze=0x40000000;
  exec->stack_idx=0;
  return cctrue;
}

#endif