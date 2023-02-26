// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCEMIT_VALUE
#define _CCEMIT_VALUE

// Todo: remove all definitions from this header file!

typedef struct ccprocd_t ccprocd_t;

typedef enum ccvalue_k ccvalue_k;
typedef enum ccvalue_k
{ ccvalue_kINVALID=0,
  ccvalue_kCONST,
  ccvalue_kGLOBAL,

  ccvalue_kPROCD,
  ccvalue_kPROCU,

  ccvalue_kEDICT,
} ccvalue_k;

typedef struct ccconstant_t ccconstant_t;
typedef struct ccconstant_t
{ cctype_t    * type;
  ccclassic_t   clsc;
} ccconstant_t;

typedef ccexec_value_t (ccbinding_t)(ccexec_t *,ccvalue_t *,cci32_t, ccexec_value_t *);

typedef struct ccprocu_t ccprocu_t;
typedef struct ccprocu_t
{ cctype_t    * type;
  ccbinding_t * proc;
} ccprocu_t;

typedef struct ccvalue_t ccvalue_t;
typedef struct ccvalue_t
{ ccvalue_k       kind;

  ccunion
  { ccedict_t     * edict;
    ccprocd_t     * procd;
    ccprocu_t     * procu;
    ccconstant_t    constant;
  };
} ccvalue_t;

typedef struct ccblock_t ccblock_t;
typedef struct ccblock_t
{ ccvalue_t  * *edict;
} ccblock_t;

typedef struct ccprocd_t ccprocd_t;
typedef struct ccprocd_t
{ ccesse_t   *esse;
  ccvalue_t **local;
  ccblock_t **block;
  ccblock_t  *decls;
  ccblock_t  *enter;
  ccblock_t  *leave;
} ccprocd_t;

typedef struct ccemit_t ccemit_t;
typedef struct ccemit_t
{ ccseer_t  *  seer;

  // Note: only functions for now!
  // Todo: should key into these by entity ...
  ccvalue_t ** globals;

  ccblock_t *  current;
  ccvalue_t *  entry;
} ccemit_t;


#define ccblock_store(block,...)  ccblock_add_edict(block,ccedict_store(__VA_ARGS__))
#define ccblock_fetch(block,...)  ccblock_add_edict(block,ccedict_fetch(__VA_ARGS__))
#define ccblock_laddr(block,...)  ccblock_add_edict(block,ccedict_laddr(__VA_ARGS__))
#define ccblock_aaddr(block,...)  ccblock_add_edict(block,ccedict_aaddr(__VA_ARGS__))
#define ccblock_arith(block,...)  ccblock_add_edict(block,ccedict_arith(__VA_ARGS__))
#define ccblock_return(block,...) ccblock_add_edict(block,ccedict_return(__VA_ARGS__))
#define ccblock_invoke(block,...) ccblock_add_edict(block,ccedict_call(__VA_ARGS__))
#define ccblock_jump(block,...)   ccblock_add_edict(block,ccedict_jump(__VA_ARGS__))
#define ccblock_fjump(block,...)  ccblock_add_edict(block,ccedict_fjump(__VA_ARGS__))
#define ccblock_tjump(block,...)  ccblock_add_edict(block,ccedict_tjump(__VA_ARGS__))
#define ccblock_dbgbreak(block)   ccblock_add_edict(block,ccedict_dbgbreak())
#define ccblock_dbgerror(block)   ccblock_add_edict(block,ccedict_dbgerror())

// Todo: remove!
ccfunc ccinle ccvalue_t *
ccvalue()
{
  ccvalue_t *t=ccmalloc_T(ccvalue_t);
  memset(t,ccnull,sizeof(*t));
  return t;
}

// Todo: remove!
ccfunc ccinle ccblock_t *
ccblock()
{
  ccblock_t *t=ccmalloc_T(ccblock_t);
  memset(t,ccnull,sizeof(*t));
  return t;
}

// Todo: remove!
ccfunc ccinle ccprocu_t *
ccprocu()
{
  ccprocu_t *t=ccmalloc_T(ccprocu_t);
  memset(t,ccnil,sizeof(*t));
  return t;
}

// Todo: remove!
ccfunc ccvalue_t *
ccprocd_local(ccprocd_t *func, cctree_t *tree)
{
  ccvalue_t **v=cctblgetP(func->local,tree);
  if(ccerrnon()) return *v;
  return ccnil;
}

// Todo: hash by entity and this should take the value instead!
ccfunc ccvalue_t *
ccemit_include_global(ccemit_t *emit, const char *label)
{
  ccassert(label!=0);

  ccvalue_t **v=cctblputS(emit->globals,label);

  ccassert(ccerrnon() ||
    cctraceerr("'%s': failed to include global, %s",label,ccerrstr()));

  ccvalue_t *value=ccvalue(label);
  *v=value;
  return value;
}

// Todo: hash by entity instead
ccfunc ccvalue_t *
ccemit_global(ccemit_t *emit, const char *label)
{
  ccvalue_t **v=cctblgetS(emit->globals,label);
  ccvalue_t *value=*v;

  ccassert(ccerrnon() ||
    cctraceerr("'%s': global not found, %s",label,ccerrstr()));

  return value;
}


// Todo: remove this, it is silly...
ccfunc ccinle void
ccvalue_retarget(ccvalue_t *value, ccleap_t p)
{
  ccassert(value!=0);
  ccassert(value->kind==ccvalue_kEDICT);

  ccassert(value->edict!=0);
  ccassert((value->edict->kind==ccedict_kJUMP)||
           (value->edict->kind==ccedict_kJUMPT)||
           (value->edict->kind==ccedict_kJUMPF));


  value->edict->leap=p;
}

ccfunc ccleap_t
ccblock_label_ex(ccblock_t *block, int index, ccstr_t label)
{ ccleap_t t;
  t.label=label;
  t.block=block;
  t.index=index;
  return t;
}

ccfunc ccleap_t
ccblock_label(ccblock_t *block, ccstr_t label)
{ return ccblock_label_ex(block,ccarrlen(block->edict),label);
}

ccfunc ccvalue_t *
ccblock_add(ccblock_t *block)
{ ccvalue_t  *v=ccmalloc_T(ccvalue_t);
  ccvalue_t **t=ccarradd(block->edict,1);
  memset(v,ccnil,sizeof(*v));
  *t=v;
  return v;
}

ccfunc ccvalue_t *
ccblock_add_edict(ccblock_t *block, ccedict_t *edict)
{ ccvalue_t *value=ccblock_add(block);
  value->kind  = ccvalue_kEDICT;
  value->edict = edict;
  return value;
}

#endif