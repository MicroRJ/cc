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
} ccbuitin_k;

typedef enum cctype_k cctype_k;
typedef enum cctype_k
{
  cctype_kINVALID=0,
  cctype_kINTEGER,
  cctype_kFUNCTION,
  cctype_kARRAY,
  cctype_kPOINTER,
} cctype_k;

typedef struct cctype_t cctype_t;
typedef struct cctype_t
{
  cctype_k     kind;
  const char * name;
  cctype_t   * type;
  cctype_t   * list;
  cci32_t      size;
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
} ccseer_t;


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
  if(t->kind==cctype_kINTEGER)
  { *b++='i';
    *b++='n';
    *b++='t';
  } else
    ccassert(!"error");

  *b=0;

  return b;
}


#endif