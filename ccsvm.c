#ifndef _CCSVM
#define _CCSVM

#define is ==

typedef enum ccvm_instr_K ccvm_instr_K;
typedef enum ccvm_value_K ccvm_value_K;
typedef struct ccvm_value_t ccvm_value_t;
typedef struct ccvm_instr_t ccvm_instr_t;
typedef struct ccvm_block_t ccvm_block_t;
typedef struct ccvm_routine_t ccvm_routine_t;

typedef enum ccvm_instr_K
{ ccvm_instr_Kfetch = 0,
  ccvm_instr_Kstore,
  ccvm_instr_Kbinop,
  ccvm_instr_Kblock,
  ccvm_instr_Kcondi,
  ccvm_instr_Kwhile,
} ccvm_instr_K;
typedef enum ccvm_value_K
{ ccvm_value_Kleaf = 0,
  ccvm_value_Kglobal,
  ccvm_value_Kblock,
  ccvm_value_Kinstr,
} ccvm_value_K;
typedef struct ccvm_value_t
{ ccvm_value_K    kind;
  cctype_t       *type;
  ccstr_t         name;
  cctoken_t       leaf;
  ccvm_block_t   *block;
  ccvm_instr_t   *instr;
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
typedef struct ccvm_block_t
{ ccvm_block_t *super;
  ccvm_instr_t *instr;
  ccvm_value_t *local;
  ccstr_t       label;
} ccvm_block_t;
typedef struct ccvm_routine_t
{ ccvm_block_t *enter;
  ccvm_block_t *leave;
  ccvm_block_t *current;
} ccvm_routine_t;

ccfunc void
ccsvm_execblock(ccvm_block_t *block);

ccglobal ccvm_value_t *globals=ccarrnil;

ccfunc ccvm_value_t *
ccvm_findglobal(const char *name)
{
  ccvm_value_t *it;
  ccarrfor(globals,it)
  {
    if(strcmp(name,it->name)==0)
    {
      return it;
    }
  }
  return ccnil;
}

ccfunc ccvm_value_t *
ccvm_addglobal(cctype_t *type, ccstr_t name)
{ ccvm_value_t *val=ccarradd(globals,1);
  val->kind=ccvm_value_Kglobal;
  val->type=type;
  val->name=name;
  return val;
}

ccfunc ccvm_value_t *
ccvm_value(ccvm_value_K kind)
{ ccvm_value_t *value=(ccvm_value_t *)ccmalloc(sizeof(*value));
  memset(value,0,sizeof(*value));
  value->kind=kind;
  return value;
}

ccfunc ccvm_block_t *
ccvm_block(const char *label)
{
  ccvm_block_t *block=(ccvm_block_t *)ccmalloc(sizeof(*block));
  memset(block,0,sizeof(*block));

  ccstrput(block->label,label);
  // Todo:
  ccarrres(block->instr,0xff);
  ccarrres(block->local,0xff);
  return block;
}

ccfunc ccvm_value_t *
ccvm_leafvalue(cctoken_t leaf)
{ ccvm_value_t *value=ccvm_value(ccvm_value_Kleaf);
  value->leaf=leaf;
  return value;
}

ccfunc ccvm_value_t *
ccvm_intvalue(signed long long int val)
{ ccvm_value_t *value=ccvm_value(ccvm_value_Kleaf);
  value->leaf.bit=cctoken_Kliteral_integer;
  value->leaf.sig=val;
  return value;
}

ccfunc ccvm_value_t *
ccvm_blockvalue(ccvm_block_t *block)
{ ccvm_value_t *value=ccvm_value(ccvm_value_Kblock);
  value->block=block;
  return value;
}

ccfunc ccvm_value_t *
ccvm_instrvalue(ccvm_instr_t *instr)
{ ccvm_value_t *value=ccvm_value(ccvm_value_Kinstr);
  value->instr=instr;
  return value;
}


ccfunc ccvm_instr_t *
ccvmblock_addir(ccvm_block_t *block)
{
  ccvm_instr_t *res=ccarradd(block->instr,1);
  memset(res,0,sizeof(*res));
  return res;
}

ccfunc ccvm_value_t *
ccemit_binary(ccvm_block_t *block, cctoken_t oper, ccvm_value_t *lhs, ccvm_value_t *rhs)
{ ccvm_instr_t *i=ccvmblock_addir(block);
  i->type=ccvm_instr_Kbinop;
  i->oper=oper;
  i->lhs =lhs;
  i->rhs =rhs;
  return ccvm_instrvalue(i);
}

ccfunc ccvm_value_t *
ccemit_store(ccvm_block_t *block, ccvm_value_t *lhs, ccvm_value_t *rhs)
{ ccvm_instr_t *i=ccvmblock_addir(block);
  i->type=ccvm_instr_Kstore;
  i->lhs=lhs;
  i->rhs=rhs;
  return ccvm_instrvalue(i);
}

ccfunc ccvm_value_t *
ccemit_while(ccvm_block_t *irset, ccvm_value_t *condi, ccvm_block_t *cond_block, ccvm_block_t *loop_block)
{ ccassert(irset!=0);ccassert(condi!=0);ccassert(cond_block!=0);

  ccvm_instr_t *i=ccvmblock_addir(irset);
  i->type=ccvm_instr_Kwhile;
  i->condi=condi;
  i->block[0]=cond_block;
  i->block[1]=loop_block;
  return ccvm_instrvalue(i);
}

ccfunc ccvm_value_t *
ccemit_condi(ccvm_block_t *irset, ccvm_value_t *condi, ccvm_block_t **block)
{ ccvm_instr_t *i=ccvmblock_addir(irset);
  i->type=ccvm_instr_Kcondi;
  i->condi=condi;
  i->block[0]=block[0];
  i->block[1]=block[1];
  return ccvm_instrvalue(i);
}

ccfunc ccvm_value_t *
ccemit_block(ccvm_block_t *irset, ccvm_block_t *block)
{ ccvm_instr_t *i=ccvmblock_addir(irset);
  i->type=ccvm_instr_Kblock;
  i->block[0]=block;
  return ccvm_instrvalue(i);
}

ccfunc ccvm_value_t *
ccsvm_resolvevalue(ccvm_block_t *irset, cctree_t *tree)
{ ccvm_value_t *value=ccnil;
  if(tree->kind==cctree_Kbinary)
  {
    ccvm_value_t *lhs=ccsvm_resolvevalue(irset, tree->binary.lhs);
    ccvm_value_t *rhs=ccsvm_resolvevalue(irset, tree->binary.rhs);

    return ccemit_binary(irset,tree->binary.opr,lhs,rhs);
  } else
  if(tree->kind==cctree_Kidentifier)
  { value=ccvm_findglobal(tree->constant.token.str);
  } else
  if(tree->kind==cctree_Kint)
  { value=ccvm_leafvalue(tree->constant.token);
  }
  return value;
}

ccfunc ccvm_value_t *
ccsvm_execbinary(cctoken_t oper, ccvm_value_t *lval, ccvm_value_t *rval)
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
  { return ccvm_intvalue(lval->leaf.sig==rval->leaf.sig);
  } else
  if(oper.bit==cctoken_Kgreater_than)
  { return ccvm_intvalue(lval->leaf.sig>rval->leaf.sig);
  } else
  if(oper.bit==cctoken_Kgreater_than_eql)
  { return ccvm_intvalue(lval->leaf.sig>=rval->leaf.sig);
  } else
  if(oper.bit==cctoken_Kless_than)
  { return ccvm_intvalue(lval->leaf.sig<rval->leaf.sig);
  } else
  if(oper.bit==cctoken_Kless_than_eql)
  { return ccvm_intvalue(lval->leaf.sig<=rval->leaf.sig);
  } else
  if(oper.bit==cctoken_Kadd)
  { return ccvm_intvalue(lval->leaf.sig+rval->leaf.sig);
  } else
  { ccassert(!"error");
    return ccnil;
  }
}

ccfunc ccvm_value_t *
ccsv_buildtree(ccvm_block_t *block, cctree_t *tree)
{
  if(tree->kind==cctree_Kmixed_statement)
  { ccvm_block_t *child=ccvm_block("mixed");
    cctree_t *it;
    ccarrfor(tree->mix_statement.stat,it)
    {
      ccsv_buildtree(child,it);
    }
    return ccemit_block(block,child);
  } else
  if(tree->kind==cctree_Kint)
  {
    return ccvm_leafvalue(tree->constant.token);
  } else
  if(tree->kind==cctree_Kinit_decl)
  { ccassert(tree->init_decl.list);

    cctree_t *it;
    ccarrfor(tree->init_decl.list,it)
    { ccvm_value_t *address,*value=ccnil;
      address=ccvm_addglobal(
        it->init_decl_name.decl->decl_name.type,
        it->init_decl_name.decl->decl_name.name);

      cctree_t *init=it->init_decl_name.init;
      if(init)
      { if(init->kind==cctree_Kint)
        { value=ccvm_leafvalue(init->constant.token);
        } else ccassert(!"error");
        ccemit_store(block,address,value);
      }
    }
    // Note: don't return anything here ...
  } else
  if(tree->kind==cctree_Kbinary)
  {
    cctree_t *lhs,*rhs;
    lhs=tree->binary.lhs;ccassert(lhs!=0);
    rhs=tree->binary.rhs;ccassert(rhs!=0);

    ccvm_value_t *lval,*rval;
    lval=ccsvm_resolvevalue(block,lhs);
    rval=ccsvm_resolvevalue(block,rhs);

    ccassert(lval!=0);
    ccassert(rval!=0);

    cctoken_t tok=tree->binary.opr;

    if(tok.bit==cctoken_Kassign)
    { return ccemit_store(block,lval,rval);
    } else
    { return ccemit_binary(block,tok,lval,rval);
    }

  } else
  if(tree->kind==cctree_Kwhile_statement)
  { cctree_t *cond=tree->while_statement.cond;
    cctree_t *stat=tree->while_statement.stat;

    ccvm_block_t *cond_block=ccvm_block("while::conditional");
    ccvm_value_t *cond_value=ccsv_buildtree(cond_block,cond);
    ccvm_block_t *loop_block=ccnil;
    if(stat)
    { loop_block=ccvm_block("while:loop");
      ccsv_buildtree(loop_block,stat);
    }
    return ccemit_while(block,cond_value,cond_block,loop_block);
  } else
  if(tree->kind==cctree_Kconditional_statement)
  {
    cctree_t  *cond=tree->conditional_statement.cond;
    cctree_t **stat=tree->conditional_statement.stat;

    ccvm_value_t *cond_value=ccsv_buildtree(block,cond);
    ccvm_block_t *condi_block[2]={ccnil,ccnil};

    if(stat[0])
    { condi_block[0]=ccvm_block("if::true");
      ccsv_buildtree(condi_block[0],stat[0]);
    }
    if(stat[1])
    { condi_block[1]=ccvm_block("if::false");
      ccsv_buildtree(condi_block[1],stat[1]);
    }
    return ccemit_condi(block,cond_value,condi_block);
  } else
  { ccassert(!"error");
  }

  return ccnil;
}
ccfunc void
ccsvm_execinstr(ccvm_instr_t *instr)
{
  // ccassert(instr->res is ccnil);

  switch(instr->type)
  { case ccvm_instr_Kfetch:
    {
    } break;
    case ccvm_instr_Kblock:
    { ccvm_instr_t *it;
      ccarrfor(instr->block[0]->instr,it) ccsvm_execinstr(it);
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
    case ccvm_instr_Kcondi:
    {
      if(instr->condi->instr->res->leaf.sig)
      {
        ccvm_block_t *block=instr->block[0];
        if(block) ccsvm_execblock(block);
      } else
      {
        ccvm_block_t *block=instr->block[1];
        if(block) ccsvm_execblock(block);
      }
    } break;
    case ccvm_instr_Kwhile:
    {
      for(;;)
      { ccsvm_execblock(instr->block[0]);
        if(instr->condi->instr->res->leaf.sig)
        {
          ccsvm_execblock(instr->block[1]);
        } else break;
      }
    } break;
    case ccvm_instr_Kbinop:
    {
      instr->res=ccsvm_execbinary(instr->oper,instr->lhs,instr->rhs);
    } break;
    default: ccassert(!"error");
  }
}

ccfunc void
ccsvm_execblock(ccvm_block_t *block)
{ cctracelog("%s",block->label);
  ccvm_instr_t *it;
  ccarrfor(block->instr,it) ccsvm_execinstr(it);
}

ccfunc int
ccsvm_execstats(cctree_t *tree)
{
  ccarrres(globals,0xff);

  ccvm_block_t *block=ccvm_block("main");

  cctree_t *stat;
  ccarrfor(tree,stat) ccsv_buildtree(block,stat);

  ccsvm_execblock(block);

  ccout("Globals\n");
  { ccvm_value_t *it;
    ccarrfor(globals,it) ccout(ccformat("%s=%i\n",it->type->name,it->leaf.sig));
  }

  return 1;
}

#endif