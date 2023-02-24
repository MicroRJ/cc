// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCSEER_H
#define _CCSEER_H


typedef enum ccbuitin_k ccbuitin_k;
typedef enum ccbuitin_k
{
  ccbuiltin_kINVALID=0,
  ccbuiltin_kCCASSERT,
  ccbuiltin_kCCBREAK,
  ccbuiltin_kCCERROR,
  ccbuiltin_kCCPRINTF,
} ccbuitin_k;

typedef enum cctype_k cctype_k;
typedef enum cctype_k
{
  cctype_kINVALID=0,

  cctype_kTYPENAME,

  cctype_kFUNCTION,
  cctype_kPOINTER,
  cctype_kARRAY,
} cctype_k;

typedef struct cctype_t cctype_t;
typedef struct cctype_t
{ cctype_k     kind;
  cctoken_k    sort;
  cctype_t   * type;
  cci32_t      size;
  cctype_t   * list;

  int is_variadic;
} cctype_t;

typedef enum ccesse_k ccesse_k;
typedef enum ccesse_k
{ ccesse_kINVALID=0,
  ccesse_kTYPENAME,
  ccesse_kFUNCTION,
  ccesse_kVARIABLE,
} ccesse_k;

ccglobal const char *ccesse_s[]=
{ "INVALID",
  "TYPENAME",
  "FUNCTION",
  "VARIABLE",
};

// Note: what is an actual entity?
typedef struct ccesse_t ccesse_t;
typedef struct ccesse_t
{ ccesse_k       kind;
  ccbuitin_k     builtin;
  cctree_t     * tree;
  cctype_t     * type;
  const char   * name;
} ccesse_t;

// Todo: legit scoping
typedef struct ccseer_t ccseer_t;
typedef struct ccseer_t
{
  ccesse_t  ** entity_tale; // Note: by string
  ccesse_t  ** symbol_tale; // Note: by tree

  cctype_t  ** tether_tale; // Note: by tree
} ccseer_t;

// Todo:
ccfunc cctype_t *
ccseer_create_pointer_type(cctype_t *type)
{
  cctype_t *result=ccmalloc_T(cctype_t);
  result->kind=cctype_kPOINTER;
  result->sort=cctoken_kINVALID;
  result->type=type;
  result->size=sizeof(char*)*8;
  result->list=ccnull;
  return result;
}

// Todo:
ccfunc cctype_t *
ccseer_create_function_type(cctype_t *type)
{
  cctype_t *result=ccmalloc_T(cctype_t);
  result->kind=cctype_kFUNCTION;
  result->sort=cctoken_kINVALID;
  result->type=type;
  result->size=0;
  result->list=ccnull;
  return result;
}

ccfunc char *
cctype_to_string(cctype_t *t, char *b)
{
  if(t->kind==cctype_kPOINTER)
  { b=cctype_to_string(t->type,b);
    *b++='*';
  } else
  if(t->kind==cctype_kARRAY)
  { b=cctype_to_string(t->type,b);
    *b++='[';
    *b++=']';
  } else
  if(t->kind==cctype_kTYPENAME)
  {
    // Todo:
    const char *name=cctoken_to_string(t->sort);
    size_t l=strlen(name);
    memcpy(b,name,l);
    b+=l;

  } else
    ccassert(!"error");

  *b=0;

  return b;
}

#endif