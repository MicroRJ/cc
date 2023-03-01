// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCEXEC_C
#define _CCEXEC_C

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
{ // Todo:
  ccfree(exec->stack);
}

// Note: allocates the register associated with this value ...
ccfunc ccinle ccexec_value_t *
ccload_alloc(ccexec_frame_t *frame, ccvalue_t *owner)
{
  ccexec_value_t *result=cctblsetP(frame->values,owner);
  result->kind=ccexec_value_kVALID;
  return result;
}

// Note: reads from the associated register ...
ccfunc ccexec_value_t
ccyield(ccexec_frame_t *stack, ccvalue_t *value)
{
  ccexec_value_t *result=cctblgetP(stack->values,value);
  ccassert(ccerrnon());

  if((result->kind==ccexec_value_kINVALID))
    cctraceerr("value kind is invalid, did you register this value and not set its contents?",0);

  ccassert(result->kind!=ccexec_value_kINVALID);
  return *result;
}

// Todo: merge this with its counter part!
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
      ccassert(couple->constant.type!=0);
      // Todo:
      result.kind=ccexec_value_kVALID;
      result.value=couple->constant.clsc.value;
    break;
    default:
      ccassert(!"error");
  }

  ccassert(result.kind!=ccexec_value_kINVALID);
  return result;
}

// Todo: merge this with its counter part!
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

// Todo: wtf is this
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
  ccassert((value!=0));
  ccassert((value->kind==ccvalue_kEDICT),
    "cannot allocate local, expected a value of type EDICT and of subtype LOCAL or PARAM");

  ccedict_t *edict=value->edict;

  ccassert((edict!=0));
  ccassert((edict->kind==ccedict_kLOCAL)||(edict->kind==ccedict_kPARAM),
    "cannot allocate local, expected an edict of type LOCAL or PARAM");

  cctype_t *type=edict->type;

  void *memory=ccstack_push_size(exec,type->size);
  memset(memory,ccnull,type->size); // Todo:

  ccexec_value_t *result=ccload_alloc(stack,value);
  result->value=memory;
  return result;
}

ccfunc ccinle ccexec_value_t
ccexec_edict_arith(cctoken_k opr, ccexec_value_t lval, ccexec_value_t rval)
{
  cci64_t i=0;

  switch(opr)
  {
    case cctoken_kBWXOR: i=lval.constI ^ rval.constI; break;

    case cctoken_kBWSHL: i=lval.constI << rval.constI; break;
    case cctoken_kBWSHR: i=lval.constI >> rval.constI; break;
    case cctoken_kTEQ: i=lval.constI == rval.constI; break;
    case cctoken_kFEQ: i=lval.constI != rval.constI; break;
    case cctoken_kGTN: i=lval.constI >  rval.constI; break;
    case cctoken_kGTE: i=lval.constI >= rval.constI; break;
    case cctoken_kLTN: i=lval.constI <  rval.constI; break;
    case cctoken_kLTE: i=lval.constI <= rval.constI; break;
    case cctoken_kSUB: i=lval.constI -  rval.constI; break;
    case cctoken_kADD: i=lval.constI +  rval.constI; break;
    case cctoken_kMUL: i=lval.constI *  rval.constI; break;
    case cctoken_kDIV: i=lval.constI /  rval.constI; break;
    case cctoken_kMOD: i=lval.constI %  rval.constI; break;
    default:
      ccassert(!"error");
  }

  ccexec_value_t v;
  v.kind  =ccexec_value_kVALID;
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
  ccedict_t *edict=value->edict;
  ccassert(edict->kind!=0);
  ccassert(edict->tree!=0);

  int result=cctrue;

  if(exec->break_next)
    ccbreak();

  exec->break_next=ccfalse;

  switch(edict->kind)
  {
    // Note: this is simply to ensure we've set the parameters ...
    case ccedict_kPARAM:
    {
      ccyield(stack,value);
    } break;
    case ccedict_kLOCAL:
    {
      ccstack_local_alloc(exec,stack,value);
    } break;
    case ccedict_kAADDR:
    { ccassert(edict->type!=0);
      ccassert(edict->lval!=0);
      ccassert(edict->rval!=0);

      cctype_t *type;
      ccexec_value_t lval,rval;

      type=edict->type;
      lval=ccyield_lvalue(stack,edict->lval);
      rval=ccyield_rvalue(stack,edict->rval);

      // Todo:
      char *memory=cccast(char*,lval.value);
      memory+=type->size*rval.constI;

      ccexec_value_t *saved=ccload_alloc(stack,value);
      saved->value=(void*)memory;
    } break;
    case ccedict_kLADDR:
    { ccassert(edict->type!=0);
      ccassert(edict->lval!=0);

      cctype_t *type;
      ccexec_value_t lval;

      type=edict->type;
      lval=ccyield_lvalue(stack,edict->lval);

      ccexec_value_t *saved=ccload_alloc(stack,value);
      saved->value=lval.value;

    } break;
    case ccedict_kFETCH:
    {
      ccassert(edict->type!=0);
      ccassert(edict->lval!=0);

      cctype_t *type;
      ccexec_value_t lval;

      type=edict->type;
      lval=ccyield_lvalue(stack,edict->lval);

      if(!lval.value)
        ccassert(!"write access violation, nullptr");

      // Todo:
      ccexec_value_t *saved=ccload_alloc(stack,value);

      // Todo:
      memcpy(&saved->value,lval.value,type->size);

      int BREAK;
      BREAK = 0;
    } break;
    case ccedict_kSTORE:
    {
      ccassert(edict->type!=0);
      ccassert(edict->lval!=0);
      ccassert(edict->rval!=0);

      cctype_t *type;
      ccexec_value_t lval,rval;

      type=edict->type;
      lval=ccyield_lvalue(stack,edict->lval);
      rval=ccyield_rvalue(stack,edict->rval);

      if(!lval.value)
        ccassert(!"write access violation, nullptr");

      // Todo:
      memcpy(lval.value,&rval.value,type->size);

      // Todo: produce operand only if necessary and take into account the type
    } break;
    case ccedict_kARITH:
    {
      ccexec_value_t lval,rval;
      cctoken_k sort;

      sort=edict->sort;
      lval=ccyield_rvalue(stack,edict->lval);
      rval=ccyield_rvalue(stack,edict->rval);

      ccexec_value_t val=ccexec_edict_arith(sort,lval,rval);

      ccexec_value_t *saved=ccload_alloc(stack,value);
      *saved=val;

    } break;
    case ccedict_kJUMP:
    { ccassert(edict->leap.block!= 0);
      ccassert(edict->leap.index!=-1);

      stack->current=edict->leap.block;
      stack->irindex=edict->leap.index;
    } break;
    case ccedict_kJUMPF:
    { ccassert(edict->rval!=0);
      ccassert(edict->leap.block!= 0);
      ccassert(edict->leap.index!=-1);

      ccexec_value_t rval=ccyield_rvalue(stack,edict->rval);
      if(!rval.constI)
      { stack->current=edict->leap.block;
        stack->irindex=edict->leap.index;
      }
    } break;
    case ccedict_kJUMPT:
    { ccassert(edict->rval!=0);
      ccassert(edict->leap.block!= 0);
      ccassert(edict->leap.index!=-1);

      ccexec_value_t rval=ccyield_rvalue(stack,edict->rval);

      if(rval.constI)
      { stack->current=edict->leap.block;
        stack->irindex=edict->leap.index;
      }
    } break;
    case ccedict_kRETURN:
    {
      stack->result=ccyield_rvalue(stack,edict->rval);
      result=ccfalse;
    } break;
    case ccedict_kINVOKE:
    {
      int rlen=ccarrlen(edict->blob);

      // Todo: proper push
      ccexec_value_t *rval=ccstack_push(exec,rlen);
      ccexec_value_t *rset=rval;

      ccvalue_t **list;
      ccarrfor(edict->blob,list)
        *rset++=ccyield_rvalue(stack,*list);

      // Note: save the return value ...
      ccexec_value_t *ret=ccload_alloc(stack,value);

      if(edict->lval->kind==ccvalue_kPROCD)
      {
        // Note: should not have to check this, just fail ...
        if(!ccexec_invoke(exec,edict->lval,ret,rlen,rval))
          cctraceerr("internal",0);

      } else
      if(edict->lval->kind==ccvalue_kPROCU)
      {
        // Todo: this is weird ...
        *ret=edict->lval->procu->proc(exec,value,rlen,rval);
      }

      ccstack_pull(exec,rlen);
    } break;
    case ccedict_kDBGBREAK:
    {
      cctracelog("called: break, breaking on next intruction...",0);

      exec->break_next=cctrue;

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
  return result;
}

ccfunc int
ccexec_invoke(
  ccexec_t *exec, ccvalue_t *value, ccexec_value_t *r, cci32_t l, ccexec_value_t *i)
{
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
    ccdref(cccast(cci64_t*,lvalue->value))=int_value;
    i++;
  }

  stack.current=procd->decls;
  stack.irindex=0;

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

  ccarrdel(stack.values);

  *r=stack.result;
  return result;
}

// Todo: REMOVE THIS
#ifndef ARG
# define ARG 22
#endif

// Todo: rename this
ccfunc ccexec_value_t
ccexec_translation_unit(ccexec_t *exec, ccemit_t *emit)
{
  exec->emit=emit;

  ccexec_value_t ret;
  memset(&ret,ccnull,sizeof(ret));

  if(emit->entry!=0)
  {
    ccexec_value_t *args=ccnil;
    ccexec_value_t *arg=ccarrone(args);
    arg->kind   =ccexec_value_kVALID;
    arg->constI =ARG;
    ccexec_invoke(exec,emit->entry,&ret,ccarrlen(args),args);
  } else
    cctraceerr("missing entry function",0);

  return ret;
}

#endif