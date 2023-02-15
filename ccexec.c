#ifndef _CCEXEC
#define _CCEXEC
// Todo: faster string allocations
// Todo: Legit stack allocator along with revamped mingle system
// Todo: Legit type system

ccfunc ccu32_t
ccexec_sizeof(ccexec_frame_t *, cctype_t *);

// Note: Returns a coupled value, to retrieve the value again you can use the proper yield functions, do not long-term-cache,
// value could be invalidated ...
ccfunc ccexec_value_t *
ccstack_mingle(ccexec_frame_t *stack, ccemit_value_t *value)
{
ccenter("stack-mingle");
  ccexec_value_t *result=cctblsetP(stack->values,value);
ccleave("stack-mingle");
  return result;
}

// Note: yields the associated execution time value ...
ccfunc ccexec_value_t
ccstack_yield(ccexec_frame_t *stack, ccemit_value_t *value)
{
ccenter("stack-yield");
  ccexec_value_t *result=cctblgetP(stack->values,value);

  ccassert(ccerrnon());

  if((result->kind==ccev_kINVALID))
    cctraceerr("value kind is invalid, did you register this value and not set its contents?");

  ccassert(result->kind!=ccev_kINVALID);

ccleave("stack-yield");
  return *result;
}

ccfunc ccexec_value_t
ccstack_yield_rvalue(
  ccexec_frame_t *stack, ccemit_value_t *couple)
{
  ccexec_value_t result;
  memset(&result,ccnil,sizeof(result));

  switch(couple->kind)
  { case ccvalue_kEDICT:
    { switch(couple->edict->kind)
      { case ccedict_kFETCH:
        case ccedict_kARITH:
        case ccedict_kINVOKE:
          result=ccstack_yield(stack,couple);
        break;
  			default: ccassert(!"error");
      }
    } break;
    case ccvalue_kCONST:
      result=ccexec_rvalue(
        cccast(void*,couple->constant.clsc.asi64),"constant-value");
    break;
  	default: ccassert(!"error");
  }

	ccassert(result.kind==ccev_kRVALUE);
  return result;
}

ccfunc ccexec_value_t
ccstack_yield_lvalue(ccexec_frame_t *stack, ccemit_value_t *value)
{
  ccexec_value_t result;
  memset(&result,ccnil,sizeof(result));

  ccassert(value!=0);
  ccassert(value->kind==ccvalue_kEDICT);

  switch(value->edict->kind)
  { case ccedict_kLOCAL:
    case ccedict_kPARAM:
    case ccedict_kADDRESS:
      result=ccstack_yield(stack,value);
    break;
  	default: ccassert(!"error");
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

ccfunc ccinle void *
ccstack_push_size(
  ccexec_t *exec, int length)
{
  void *memory=ccnil;

  if(exec->stack_idx+length<=exec->stack_sze)
  {
    memory=cccast(char*,exec->stack)+exec->stack_idx;
    exec->stack_idx+=length;

  } else cctraceerr("stack_overflow");

  return memory;
}

ccfunc ccinle ccexec_value_t *
ccstack_push(
  ccexec_t *exec, int length)
{
  if(exec->stack_idx+length>exec->stack_sze)
  {
    cctraceerr("stack_overflow");
    ccassert(!"stack_overflow");
  }
  ccexec_value_t *memory=cccast(ccexec_value_t*,cccast(char*,exec->stack)+exec->stack_idx);
  exec->stack_idx+=sizeof(ccexec_value_t)*length;
  return memory;
}

// Note: Allocates memory on the stack appropriate for the value's type and returns
// a addressable l-value coupled to the given emit-value, to retrieve the l-value again
// simple use the values-hash-table ...
ccfunc ccinle ccexec_value_t *
ccstack_local_alloc(
  ccexec_t *exec, ccexec_frame_t *stack, ccemit_value_t *value)
{
ccenter("stack-local-alloc");

  ccnotnil((value));
  ccassert((value->kind==ccvalue_kEDICT),
    "cannot allocate local, expected a value of type EDICT and of subtype LOCAL or PARAM");

  ccedict_t *edict=value->edict;

  ccnotnil((edict));
  ccassert((edict->kind==ccedict_kLOCAL)||(edict->kind==ccedict_kPARAM),
    "cannot allocate local, expected an edict of type LOCAL or PARAM");

  ccu32_t size=ccexec_sizeof(stack,edict->local.type);
  ccassert(size>=8);

  char *memory=(char*)ccstack_push_size(exec,size);
  memset(memory,ccnil,size);

  ccexec_value_t *result=ccstack_mingle(stack,value);
  *result=ccexec_lvalue(memory,"local_alloc");

ccleave("stack-local-alloc");
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
{ switch(opr)
  { case cctoken_kTEQ:
      return ccexec_rvalue(cccast(void*,cccast(cci64_t,lval.asi64==rval.asi64)),"==");
    case cctoken_kFEQ:
      return ccexec_rvalue(cccast(void*,cccast(cci64_t,lval.asi64!=rval.asi64)),"!=");
    case cctoken_kGTN:
      return ccexec_rvalue(cccast(void*,cccast(cci64_t,lval.asi64>rval.asi64)),">");
    case cctoken_kGTE:
      return ccexec_rvalue(cccast(void*,cccast(cci64_t,lval.asi64>=rval.asi64)),">=");
    case cctoken_kLTN:
      return ccexec_rvalue(cccast(void*,cccast(cci64_t,lval.asi64<rval.asi64)),"<");
    case cctoken_kLTE:
      return ccexec_rvalue(cccast(void*,cccast(cci64_t,lval.asi64<=rval.asi64)),"<=");
    case cctoken_kMUL:
      return ccexec_rvalue(cccast(void*,cccast(cci64_t,lval.asi64*rval.asi64)),"*");
    case cctoken_kDIV:
      return ccexec_rvalue(cccast(void*,cccast(cci64_t,lval.asi64/rval.asi64)),"/");
    case cctoken_kSUB:
      return ccexec_rvalue(cccast(void*,cccast(cci64_t,lval.asi64-rval.asi64)),"-");
    case cctoken_kADD:
      return ccexec_rvalue(cccast(void*,cccast(cci64_t,lval.asi64+rval.asi64)),"+");
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
ccenter("exec-edict");

  // Todo
  ccexec_value_t zro=ccexec_rvalue(cccast(void*,0),"zro");

  ccedict_t *edict=value->edict;

  int result=cctrue;
  switch(edict->kind)
  {
    // Note: this is simply to ensure we've set the parameters ...
    case ccedict_kPARAM:
    {
ccenter("exec-edict-param");
      ccstack_yield(stack,value);
ccleave("exec-edict-param");
    } break;
    case ccedict_kLOCAL:
    {
ccenter("exec-edict-local");
      ccstack_local_alloc(exec,stack,value);
ccleave("exec-edict-local");
    } break;
    case ccedict_kADDRESS:
    {
ccenter("exec-edict-address");
      ccemit_value_t *lvalue,*rvalue;
      lvalue=edict->addr.lval;
      rvalue=edict->addr.rval;

      ccexec_value_t lval,rval;
      lval=ccstack_yield_lvalue(stack,lvalue);
      rval=ccstack_yield_rvalue(stack,rvalue);

      cci64_t *memory=cccast(cci64_t*,lval.value);
      memory+=rval.asi64;

      ccexec_value_t *saved=ccstack_mingle(stack,value);
      *saved=ccexec_lvalue(memory,"address");

ccleave("exec-edict-address");
    } break;
    case ccedict_kFETCH:
    {
ccenter("exec-edict-fetch");
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
ccleave("exec-edict-fetch");
    } break;
    case ccedict_kSTORE:
    {
ccenter("exec-edict-store");

      ccexec_value_t lval,rval;
      lval=ccstack_yield_lvalue(stack,ccnotnil(edict->store.lval));
      rval=ccstack_yield_rvalue(stack,ccnotnil(edict->store.rval));

      // Todo:
      *cccast(cci64_t*,lval.value)=rval.asi64;

      ccexec_value_t *saved=ccstack_mingle(stack,value);
      saved->kind=ccev_kRVALUE;
      saved->value=rval.value;
      saved->label=rval.label;

ccleave("exec-edict-store");
    } break;
    case ccedict_kARITH:
    {
ccenter("exec-edict-arith");
      ccexec_value_t lval,rval;
      lval=ccstack_yield_rvalue(stack,edict->arith.lhs);
      rval=ccstack_yield_rvalue(stack,edict->arith.rhs);

      ccexec_value_t *saved=ccstack_mingle(stack,value);
      *saved=ccexec_edict_arith(edict->arith.opr,lval,rval);
ccleave("exec-edict-arith");
    } break;
    case ccedict_kJUMP:
    { stack->current=edict->jump.blc;
      stack->irindex=edict->jump.tar;
    } break;
    case ccedict_kJUMPF:
    {
ccenter("exec-edict-jumpf");
      ccexec_value_t
      rval=ccstack_yield_rvalue(stack,edict->jump.cnd);
      if(!rval.asi32)
      { stack->current=edict->jump.blc;
        stack->irindex=edict->jump.tar;
      }
ccleave("exec-edict-jumpf");
    } break;
    case ccedict_kJUMPT:
    {
ccenter("exec-edict-jumpt");
      ccexec_value_t
      rval=ccstack_yield_rvalue(stack,ccnotnil(edict->jump.cnd));
      if(rval.asi32)
      { stack->current=edict->jump.blc;
        stack->irindex=edict->jump.tar;
      }
ccleave("exec-edict-jumpt");
    } break;
    // Note: save the return value under the return instruction value ...
    case ccedict_kRETURN:
    {
ccenter("exec-edict-return");
      ccexec_value_t *saved=ccstack_mingle(stack,value);
      *saved=ccstack_yield_rvalue(stack,edict->ret.rval);
      result=ccfalse;
ccleave("exec-edict-return");
    } break;
    case ccedict_kINVOKE:
    {
ccenter("exec-edict-invoke");
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
ccleave("exec-edict-invoke");
    } break;

    // Todo: to be removed ...
    case ccedict_kENTER:
    { ccexec_enter(stack,edict->enter.blc);
    } break;
    case ccedict_kTERNARY:
    { ccexec_value_t rval;
      rval=ccstack_yield_rvalue(stack,ccnotnil(edict->ternary.cnd));
      if(rval.asi32)
      { if(edict->ternary.lhs) ccexec_enter(stack,edict->ternary.lhs);
      }
      else
      { if(edict->ternary.rhs) ccexec_enter(stack,edict->ternary.rhs);
      }
    } break;
    case ccedict_kDBGBREAK:
    { cctracelog("break");
      ccbreak();
    } break;
  	case ccedict_kDBGERROR:
    { cctraceerr("error");
      ccbreak();
    } break;

    default: ccassert(!"error");
  }

ccleave("exec-edict");
  return result;
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
ccenter("invoke");
  int result=ccfalse;

  // Todo: let's not use our actual stack ...
  ccexec_frame_t stack;
  memset(&stack,ccnil,sizeof(stack));

  cctree_t *type=procd->tree->type;

  ccassert((int)ccarrlen(type->list)==l);

  cctree_t **ltree;
  ccarrfor(type->list,ltree)
  { ccemit_value_t *lparam=ccprocd_local(procd,*ltree);
    ccexec_value_t *lvalue=ccstack_local_alloc(exec,&stack,lparam);

    cci32_t int_value=i->asi32;
    ccdref(cccast(cci32_t*,lvalue->value))=int_value;
    i++;
  }

  ccexec_enter(&stack,procd->decls);

ccenter("procd-exec");
  while(cccast(ccu32_t,stack.irindex)<ccarrlen(stack.current->edict))
  {
    ccemit_value_t **it=stack.current->edict+stack.irindex;
    stack.irindex++;

    if(!ccexec_edict(exec,&stack,*it))
    {
      // Note: is this flawed?
      ccedict_t *edict=(*it)->edict;

      result=edict->kind==ccedict_kRETURN;

      if(result)
        *r=ccstack_yield(&stack,*it);

      break;
    }
  }
ccleave("procd-exec");

  ccarrdel(stack.values);

ccleave("invoke");
  return result;
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

  size_t stack_size=256 * 1024*1024;

  exec->stack=ccmalloc(stack_size);
  exec->stack_sze=stack_size;
  exec->stack_idx=0;

  return cctrue;
}

ccfunc void
ccexec_uninit(ccexec_t *exec)
{
  ccfree(exec->stack);
}

#endif