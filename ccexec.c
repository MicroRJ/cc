// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCEXEC_C
#define _CCEXEC_C

struct ccstruct_t
{
  int a;
  int b;
  int c;
} _t;

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

// Note: allocates a register for this value ...
ccfunc ccinle ccexec_value_t *
ccexec_register(ccexec_frame_t *frame, ccvalue_t *owner, cctype_t *type)
{
  ccexec_value_t *result=cctblsetP(frame->values,owner);
  result->kind=ccexec_value_kVALID;
  result->type=type;
  return result;
}

// Note: reads from the associated register ...
ccfunc ccexec_value_t
ccexec_yield(ccexec_frame_t *stack, ccvalue_t *value, int is_lval)
{
  ccassert(stack!=0);
  ccassert(value!=0);

  ccexec_value_t result;
  memset(&result,ccnull,sizeof(result));

  switch(value->kind)
  { case ccvalue_kEDICT:
    { result=*cctblgetP(stack->values,value);

      ccassert(ccerrnon() ||
        cctraceerr("invalid value, did you register this value? ;%s",
          cctree_string(value->edict->tree,ccnull)));

      ccassert(result.kind!=ccexec_value_kINVALID ||
        cctraceerr("invalid value, did you register this value and not set its contents?",0));

      switch(value->edict->kind)
      {
        case ccedict_kPARAM:
        case ccedict_kLOCAL: ccassert(is_lval);
        break;

        case ccedict_kINVOKE:
        case ccedict_kARITH:
        case ccedict_kLADDR: ccassert(!is_lval);
        break;

        case ccedict_kAADDR:
        case ccedict_kFETCH:
        break;

        default:
          ccassert(!"error");
      }
    } break;
    case ccvalue_kCONST:
      ccassert(value->constant.type!=0);
      // Todo:
      result.kind=ccexec_value_kVALID;
      result.value=value->constant.clsc.value;

      ccassert(!is_lval);
    break;

    default:
      ccassert(!"error");
  }

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

ccfunc ccinle ccexec_value_t
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

  void *memory=ccstack_push_size(exec,edict->type->size);
  memset(memory,ccnull,edict->type->size); // Todo:

  ccexec_value_t *result=ccexec_register(stack,value,edict->type);
  result->value=memory;
  return *result;
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

  // Todo:
  ccexec_value_t v;
  v.kind  =ccexec_value_kVALID;
  v.constI=i;
  return v;
}

ccfunc int
ccexec_invoke(
  ccexec_t *, ccvalue_t *, ccexec_value_t *, ccu32_t, ccexec_value_t *);

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
      ccexec_yield(stack,value,cctrue);
    } break;
    case ccedict_kLOCAL:
    {
      ccexec_value_t lval;
      lval=ccstack_local_alloc(exec,stack,value);

      // Todo:
      ccdebuglog("%p LOCAL: [%s] %p{%s}; %s",
        value,cctype_string(edict->type,ccnull),
        lval.value,cctype_string(lval.type,ccnull),
        cctree_string(edict->tree,ccnull));
    } break;
    case ccedict_kAADDR:
    { ccassert(edict->type!=0);
      ccassert(edict->lval!=0);
      ccassert(edict->rval!=0);

      cctype_t *type=edict->type;
      ccexec_value_t lval=ccexec_yield(stack,edict->lval,cctrue);
      ccexec_value_t rval=ccexec_yield(stack,edict->rval,ccfalse);

      // Todo:
      ccexec_value_t *saved=ccexec_register(stack,value,type);
      saved->value=lval.value+type->size*rval.constI;

      // ccdebuglog("%p ADDDR: [%s] %p{%s}, %i; %s",
      //   value,cctype_string(type,ccnull),
      //   lval.value,cctype_string(lval.type,ccnull),
      //   cccast(cci32_t,rval.value),
      //   cctree_string(edict->tree,ccnull));
    } break;
    case ccedict_kLADDR:
    { ccassert(edict->type!=0);
      ccassert(edict->lval!=0);

      cctype_t *type=edict->type;
      ccexec_value_t lval=ccexec_yield(stack,edict->lval,cctrue);

      ccexec_value_t *saved=ccexec_register(stack,value,type);
      saved->value=lval.value;

    } break;
    case ccedict_kFETCH:
    {
      ccassert(edict->type!=0);
      ccassert(edict->lval!=0);

      cctype_t *type=edict->type;
      ccexec_value_t lval=ccexec_yield(stack,edict->lval,cctrue);

      if(!lval.value)
        ccassert(!"write access violation, nullptr");

      // Todo:
      ccexec_value_t *saved=ccexec_register(stack,value,type);

      // Todo: how would we make this more robust, values of values? In reality, I wouldn't want
      // to have the notion of a struct at the vm level, but that would require a slightly more
      // advanced front-end...
      if(type->kind==cctype_kRECORD)
      { saved->value=ccstack_push_size(exec,type->size);
        ccassert(saved->value!=0);

        memcpy(saved->value,lval.value,type->size);
      } else
      {
        memcpy(&saved->value,lval.value,type->size);
      }

      // Todo:
      // ccdebuglog("%p FETCH: [%s] %p{%s}; %s",
      //   value,cctype_string(type,ccnull),
      //   lval.value,cctype_string(lval.type,ccnull),
      //   cctree_string(edict->tree,ccnull));
    } break;
    case ccedict_kSTORE:
    {
      ccassert(edict->type!=0);
      ccassert(edict->lval!=0);
      ccassert(edict->rval!=0);

      ccassert(
        (edict->tree->kind==cctree_kDECL)||
        (edict->tree->kind==cctree_kBINARY));

      cctype_t *type=edict->type;
      ccexec_value_t lval=ccexec_yield(stack,edict->lval,cctrue);
      ccexec_value_t rval=ccexec_yield(stack,edict->rval,ccfalse);

      if(!lval.value)
        ccassert(!"write access violation, nullptr");

      // Todo:
      if(type->kind==cctype_kRECORD)
        memcpy(lval.value,rval.value,type->size);
      else
        memcpy(lval.value,&rval.value,type->size);

      // Todo: produce operand

      // Todo:
      // ccdebuglog("%p STORE: [%s] %p{%s}, %p{%s}; %s",
      //     value,cctype_string(type,ccnull),
      //     lval.value,cctype_string(lval.type,ccnull),
      //     rval.value,cctype_string(rval.type,ccnull),
      //     cctree_string(edict->tree,ccnull));
    } break;
    case ccedict_kARITH:
    {
      cctoken_k sort=edict->sort;
      ccexec_value_t lval=ccexec_yield(stack,edict->lval,ccfalse);
      ccexec_value_t rval=ccexec_yield(stack,edict->rval,ccfalse);

      ccexec_value_t val=ccexec_edict_arith(sort,lval,rval);

      ccexec_value_t *saved=ccexec_register(stack,value,ccnull);
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

      ccexec_value_t rval=ccexec_yield(stack,edict->rval,ccfalse);
      if(!rval.constI)
      { stack->current=edict->leap.block;
        stack->irindex=edict->leap.index;
      }
    } break;
    case ccedict_kJUMPT:
    { ccassert(edict->rval!=0);
      ccassert(edict->leap.block!= 0);
      ccassert(edict->leap.index!=-1);

      ccexec_value_t rval=ccexec_yield(stack,edict->rval,ccfalse);

      if(rval.constI)
      { stack->current=edict->leap.block;
        stack->irindex=edict->leap.index;
      }
    } break;
    case ccedict_kRETURN:
    {
      stack->result=ccexec_yield(stack,edict->rval,ccfalse);
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
        *rset++=ccexec_yield(stack,*list,ccfalse);

      // Note: save the return value ...
      ccexec_value_t *ret=ccexec_register(stack,value,ccnull);

      if(edict->lval->kind==ccvalue_kPROCD)
      {
        // Note: should not have to check this, just fail ...
        if(!ccexec_invoke(exec,edict->lval,ret,rlen,rval))
          cctraceerr("no return value specified",0);

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
  ccexec_t *exec, ccvalue_t *value, ccexec_value_t *r, ccu32_t l, ccexec_value_t *i)
{
  int result=ccfalse;

  ccprocd_t *procd=value->procd;

  // Todo: let's not use our actual stack ...
  ccexec_frame_t stack;
  memset(&stack,ccnull,sizeof(stack));

  stack.procedure=procd;

  // Todo: remove this please
  ccesse_t **list=procd->esse->list;

  ccassert(ccarrlen(procd->param)==l);

  ccvalue_t **p;
  ccarrfor(procd->param,p)
  { ccexec_value_t x=ccstack_local_alloc(exec,&stack,*p);
    memcpy(x.value,&i->value,8); // Todo:
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