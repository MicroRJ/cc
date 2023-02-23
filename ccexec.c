
// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCEXEC
#define _CCEXEC

ccfunc cci64_t
ccsizeof(cctype_t *);

// Note: allocates the register associated with this value ...
ccfunc ccinle ccexec_value_t *
ccload_alloc(ccexec_frame_t *frame, ccvalue_t *value)
{
ccdbenter("ccload-alloc");
  ccexec_value_t *result=cctblsetP(frame->values,value);
ccdbleave("ccload-alloc");
  return result;
}

ccfunc ccinle ccexec_value_t *
ccloadI(ccexec_frame_t *frame, ccvalue_t *owner, cci64_t value, const char *name)
{
ccdbenter("ccload");
  ccexec_value_t *result=ccload_alloc(frame,owner);
  result->kind=ccexec_value_kVALUE;
  result->name=name;
  result->constI=value;
ccdbleave("ccload");
  return result;
}

ccfunc ccinle ccexec_value_t *
ccloadA(ccexec_frame_t *frame, ccvalue_t *owner, ccaddress_t value, const char *name)
{
ccdbenter("ccload");
  ccexec_value_t *result=ccload_alloc(frame,owner);
  result->kind=ccexec_value_kADDRESS;
  result->name=name;
  result->address=value;
ccdbleave("ccload");
  return result;
}

// Note: reads from the associated register ...
ccfunc ccexec_value_t
ccyield(ccexec_frame_t *stack, ccvalue_t *value)
{
ccdbenter("stack-yield");
  ccexec_value_t *result=cctblgetP(stack->values,value);
  ccassert(ccerrnon());

  if((result->kind==ccexec_value_kINVALID))
    cctraceerr("value kind is invalid, did you register this value and not set its contents?",0);

  ccassert(result->kind!=ccexec_value_kINVALID);

ccdbleave("stack-yield");
  return *result;
}

ccfunc ccexec_value_t
ccyield_rvalue(
  ccexec_frame_t *stack, ccvalue_t *couple)
{
  ccexec_value_t result;
  memset(&result,ccnull,sizeof(result));

  switch(couple->kind)
  { case ccvalue_kEDICT:
    { switch(couple->edict->kind)
      { case ccedict_kFETCH:
        case ccedict_kARITH:
        case ccedict_kINVOKE:
        case ccedict_kLADDR:
        // case ccedict_kAADDR: FETCH
          result=ccyield(stack,couple);
        break;
        default: ccassert(!"error");
      }
    } break;
    case ccvalue_kCONST:
      result.kind=ccexec_value_kVALUE;
      result.name="constant";
      result.value=couple->constant.clsc.value;
    break;
    default:
      ccassert(!"error");
  }

  ccassert(result.kind!=ccexec_value_kINVALID);
  return result;
}

ccfunc ccexec_value_t
ccyield_lvalue(ccexec_frame_t *stack, ccvalue_t *value)
{
  ccexec_value_t result;
  memset(&result,ccnull,sizeof(result));

  ccassert(value!=0);
  ccassert(value->kind==ccvalue_kEDICT);

  switch(value->edict->kind)
  { case ccedict_kLOCAL:
    case ccedict_kPARAM:
    case ccedict_kAADDR:
    case ccedict_kFETCH:
      result=ccyield(stack,value);
    break;
    default: ccassert(!"error");
  }

  ccassert(result.kind!=ccexec_value_kINVALID);
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

  } else cctraceerr("stack_overflow",0);

  return memory;
}

ccfunc ccinle ccexec_value_t *
ccstack_push(
  ccexec_t *exec, int length)
{
  return (ccexec_value_t *)ccstack_push_size(exec,length*sizeof(ccexec_value_t));
}

ccfunc ccinle ccexec_value_t *
ccstack_local_alloc(
  ccexec_t *exec, ccexec_frame_t *stack, ccvalue_t *value)
{
ccdbenter("stack-local-alloc");
  ccassert((value!=0));
  ccassert((value->kind==ccvalue_kEDICT),
    "cannot allocate local, expected a value of type EDICT and of subtype LOCAL or PARAM");

  ccedict_t *edict=value->edict;

  ccassert((edict!=0));
  ccassert((edict->kind==ccedict_kLOCAL)||(edict->kind==ccedict_kPARAM),
    "cannot allocate local, expected an edict of type LOCAL or PARAM");

  cci64_t size=ccsizeof(edict->local.type);
  ccassert(size>=8);

  char *memory=(char*)ccstack_push_size(exec,(cci32_t)size); // Todo: cast
  memset(memory,ccnil,size);

  ccexec_value_t *result=ccloadA(stack,value,memory,"local_alloc");

ccdbleave("stack-local-alloc");
  return result;
}

ccfunc ccinle ccexec_value_t
ccexec_edict_arith(cctoken_k opr, ccexec_value_t lval, ccexec_value_t rval)
{
  // Todo:
  cci64_t i;
  const char *n;

  switch(opr)
  { case cctoken_kTEQ: i=lval.constI == rval.constI, n="=="; break;
    case cctoken_kFEQ: i=lval.constI != rval.constI, n="!="; break;
    case cctoken_kGTN: i=lval.constI >  rval.constI, n=">";  break;
    case cctoken_kGTE: i=lval.constI >= rval.constI, n=">="; break;
    case cctoken_kLTN: i=lval.constI <  rval.constI, n="<";  break;
    case cctoken_kLTE: i=lval.constI <= rval.constI, n="<="; break;
    case cctoken_kMUL: i=lval.constI *  rval.constI, n="*";  break;
    case cctoken_kDIV: i=lval.constI /  rval.constI, n="/";  break;
    case cctoken_kSUB: i=lval.constI -  rval.constI, n="-";  break;
    case cctoken_kADD: i=lval.constI +  rval.constI, n="+";  break;
    default:
      i=0;
      n=0;
      ccassert(!"error");
  }

  ccexec_value_t v;
  v.kind  =ccexec_value_kVALUE;
  v.name  =n;
  v.constI=i;
  return v;
}

ccfunc int
ccexec_invoke(
  ccexec_t *, ccvalue_t *, ccexec_value_t *, cci32_t, ccexec_value_t *);

ccfunc int
ccexec_edict(
  ccexec_t *exec, ccexec_frame_t *stack, ccvalue_t *value)
{
ccdbenter("exec-edict");

  ccedict_t *edict=value->edict;

  int result=cctrue;
  switch(edict->kind)
  {
    // Note: this is simply to ensure we've set the parameters ...
    case ccedict_kPARAM:
    {
ccdbenter("exec-edict-param");
      ccyield(stack,value);
ccdbleave("exec-edict-param");
    } break;
    case ccedict_kLOCAL:
    {
ccdbenter("exec-edict-local");
      ccstack_local_alloc(exec,stack,value);
ccdbleave("exec-edict-local");
    } break;
    case ccedict_kAADDR:
    {
ccdbenter("exec-edict-Aaddr");
      ccexec_value_t lval,rval;
      lval=ccyield_lvalue(stack,edict->addr.lval);
      rval=ccyield_rvalue(stack,edict->addr.rval);

      // Todo:
      cci64_t *memory=cccast(cci64_t*,lval.address);
      memory+=rval.constI;
      ccloadA(stack,value,memory,"address");

ccdbleave("exec-edict-Aaddr");
    } break;
    case ccedict_kLADDR:
    {
ccdbenter("exec-edict-Laddr");
      ccexec_value_t lval;
      lval=ccyield_lvalue(stack,edict->addr.lval);

      // Todo:
      ccloadA(stack,value,lval.address,"address");
ccdbleave("exec-edict-Laddr");
    } break;
    case ccedict_kFETCH:
    {
ccdbenter("exec-edict-fetch");
      ccexec_value_t lval;
      lval=ccyield_lvalue(stack,edict->fetch.lval);

      if(!lval.address)
        ccassert(!"write access violation, nullptr");

      // Todo:
      cci64_t *memory=cccast(cci64_t*,lval.address);
      ccloadI(stack,value,*memory,"fetch");
ccdbleave("exec-edict-fetch");
    } break;
    // Todo: produce operand only if necessary and take into account the type
    case ccedict_kSTORE:
    {
ccdbenter("exec-edict-store");
      ccexec_value_t lval,rval;
      lval=ccyield_lvalue(stack,edict->store.lval);
      rval=ccyield_rvalue(stack,edict->store.rval);

      if(!lval.address)
        ccassert(!"write access violation, nullptr");

      // Todo:
      memcpy(lval.address,&rval.value,sizeof(rval.value));
      ccloadI(stack,value,rval.constI,"store");
ccdbleave("exec-edict-store");
    } break;
    case ccedict_kARITH:
    {
ccdbenter("exec-edict-arith");
      ccexec_value_t lval,rval;
      lval=ccyield_rvalue(stack,edict->arith.lhs);
      rval=ccyield_rvalue(stack,edict->arith.rhs);

      ccexec_value_t val=ccexec_edict_arith(edict->arith.opr,lval,rval);
      ccloadI(stack,value,val.constI,val.name);

ccdbleave("exec-edict-arith");
    } break;
    case ccedict_kJUMP:
    { stack->current=edict->jump.blc;
      stack->irindex=edict->jump.tar;
    } break;
    case ccedict_kJUMPF:
    {
ccdbenter("exec-edict-jumpf");
      ccexec_value_t
      rval=ccyield_rvalue(stack,edict->jump.cnd);
      if(!rval.constI)
      { stack->current=edict->jump.blc;
        stack->irindex=edict->jump.tar;
      }
ccdbleave("exec-edict-jumpf");
    } break;
    case ccedict_kJUMPT:
    {
ccdbenter("exec-edict-jumpt");
      ccexec_value_t
      rval=ccyield_rvalue(stack,edict->jump.cnd);
      if(rval.constI)
      { stack->current=edict->jump.blc;
        stack->irindex=edict->jump.tar;
      }
ccdbleave("exec-edict-jumpt");
    } break;
    case ccedict_kRETURN:
    {
ccdbenter("exec-edict-return");
      stack->result=ccyield_rvalue(stack,edict->ret.rval);
      result=ccfalse;
ccdbleave("exec-edict-return");
    } break;
    case ccedict_kINVOKE:
    {
ccdbenter("exec-edict-invoke");

      int rlen=ccarrlen(edict->invoke.rval);
      ccexec_value_t *rval=ccstack_push(exec,rlen);
      ccexec_value_t *setr=rval;
      ccvalue_t **list=ccnil;
      ccarrfor(edict->invoke.rval,list)
        *setr++=ccyield_rvalue(stack,*list);

      // Note: save the return value ...
      ccexec_value_t *ret=ccload_alloc(stack,value);

      if(edict->invoke.call->kind==ccvalue_kPROCD)
      {
        if(!ccexec_invoke(exec,edict->invoke.call,ret,rlen,rval))
          ccassert(!"no-return value, error");
      } else
      if(edict->invoke.call->kind==ccvalue_kPROCU)
      {
        *ret=edict->invoke.call->procu->proc(exec,value,rlen,rval);
      }

      ccstack_pull(exec,rlen);

ccdbleave("exec-edict-invoke");
    } break;
    case ccedict_kDBGBREAK:
    {
      cctracelog("called: break",0);
#ifndef _DEVELOPER
      ccbreak();
#endif
    } break;
    case ccedict_kDBGERROR:
    {
      cctraceerr("called: error",0);
#ifndef _DEVELOPER
      ccbreak();
#endif
    } break;

    default: ccassert(!"error");
  }

ccdbleave("exec-edict");
  return result;
}

ccfunc cci64_t
ccsizeof(cctype_t *type)
{
  if(type->kind==cctype_kPOINTER)
  {
    return sizeof(char*);
  } else
  if(type->kind==cctype_kARRAY)
  {
    cci64_t size=ccsizeof(type->type);
    size*=type->size;
    return size;
  } else
  if(type->kind==cctype_kTYPENAME)
  { return sizeof(cci64_t);
  }
  ccassert(!"error");
  return 0;
}

ccfunc int
ccexec_invoke(
  ccexec_t *exec, ccvalue_t *value, ccexec_value_t *r, cci32_t l, ccexec_value_t *i)
{
ccdbenter("invoke");
  int result=ccfalse;

  ccprocd_t *procd=value->procd;

  // Todo: let's not use our actual stack ...
  ccexec_frame_t stack;
  memset(&stack,ccnil,sizeof(stack));
  stack.procedure=procd;

  cctree_t *type=procd->esse->tree->type;
  ccassert((int)ccarrlen(type->list)==l);

  cctree_t **ltree;
  ccarrfor(type->list,ltree)
  { ccvalue_t *lparam=ccprocd_local(procd,*ltree);
    ccexec_value_t *lvalue=ccstack_local_alloc(exec,&stack,lparam);

    // Todo: dedicated 'store' ...

    cci64_t int_value=i->constI;
    ccdref(cccast(cci64_t*,lvalue->address))=int_value;
    i++;
  }

  stack.current=procd->decls;
  stack.irindex=0;

ccdbenter("procd-exec");
  while(cccast(ccu32_t,stack.irindex)<ccarrlen(stack.current->edict))
  {
    ccvalue_t **it=stack.current->edict+stack.irindex;
    stack.irindex++;

    // Note: break if we hit some sort of halt intruction ...
    if(!ccexec_edict(exec,&stack,*it))
    {
      result=cctrue;
      break;
    }
  }
ccdbleave("procd-exec");

  ccarrdel(stack.values);

  *r=stack.result;

ccdbleave("invoke");
  return result;
}

// Todo: REMOVE THIS
#ifndef ARG
# define ARG 22
#endif

ccfunc ccexec_value_t
ccexec_translation_unit(ccexec_t *exec, ccemit_t *emit)
{
  exec->emit=emit;

  ccexec_value_t *args=ccnil;
  ccexec_value_t *arg=ccarrone(args);
  arg->name   ="arg-0";
  arg->kind   =ccexec_value_kADDRESS;
  arg->constI =ARG;

  ccexec_value_t ret;
  ccexec_invoke(exec,emit->entry,&ret,ccarrlen(args),args);

  return ret;
}

ccfunc int
ccexec_init(ccexec_t *exec)
{ memset(exec,ccnil,sizeof(*exec));

  // Todo:
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