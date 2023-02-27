// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCSEER_H
#define _CCSEER_H

// Note: sizeof and typeof we get in the tether table... typeof results in a type operand and
// sizeof in a constant operand ... should we do it that way?
typedef enum ccbuiltin_k ccbuiltin_k;
typedef enum ccbuiltin_k
{
  ccbuiltin_kINVALID=0,
  ccbuiltin_kCCBREAK,
  ccbuiltin_kCCERROR,
} ccbuiltin_k;

typedef enum cctype_k cctype_k;
typedef enum cctype_k
{ cctype_kINVALID=0,

  cctype_kRECORD,
  // Todo: rename this ...
  cctype_kSPECIFIER,
  cctype_kFUNCTION,
  cctype_kPOINTER,
  cctype_kARRAY,
} cctype_k;

typedef enum ccesse_k ccesse_k;
typedef enum ccesse_k
{ ccesse_kINVALID=0,
  ccesse_kCBINDING,
  ccesse_kFUNCTION,
  ccesse_kVARIABLE,
} ccesse_k;

typedef struct cctype_t cctype_t;
typedef struct ccesse_t ccesse_t;

// Note: how do we implement scoping?
typedef struct ccaura_t ccaura_t;
typedef struct ccaura_t
{ ccesse_t  ** entity_table;
} ccaura_t;

// Note: entity or essential, indivisible and relatively unique, such as variables ...
typedef struct ccesse_t ccesse_t;
typedef struct ccesse_t
{ const char    * name;
  ccesse_k        kind;
  ccbuiltin_k     sort;
  cctype_t      * type;
  ccaura_t      * aura;
  cctree_t      * tree;
} ccesse_t;

typedef struct ccelem_t ccelem_t;
typedef struct ccelem_t
{ const char * name;
  cctype_t   * type;
  cctree_t   * tree;
  cci32_t      slot;
} ccelem_t;

// Note: describes the underlying layout and or format of entities ...
typedef struct cctype_t cctype_t;
typedef struct cctype_t
{ const char * name;
  cctype_k     kind;
  cctoken_k    sort;
  cctree_t   * tree;
  cctype_t   * type;
  ccelem_t   * list; // Note: table, use tree as key, must be decl-name ...
  cci32_t      size;

  // Todo: implement...
  unsigned is_variadic: 1;
} cctype_t;

// Todo: legit scoping
// Note: #entity_table: Maps a "string" name to an entity
// Note: #symbol_table: Maps a "tree" pointer to its respective entities, symbolic trees ...
// Note: #tether_table: Maps a "tree" to a type ...
typedef struct ccseer_t ccseer_t;
typedef struct ccseer_t
{
  ccesse_t  ** entity_table;
  ccesse_t  ** symbol_table;
  cctype_t  ** tether_table;

  // Todo: how to actually do this properly and where to store it ...
  cctype_t *type_void;
  cctype_t *type_stdc_int;
  cctype_t *type_stdc_long;
  cctype_t *type_stdc_short;
  cctype_t *type_stdc_double;
  cctype_t *type_stdc_float;
  cctype_t *type_stdc_char;
  cctype_t *type_stdc_bool;
  cctype_t *type_stdc_signed;
  cctype_t *type_stdc_unsigned;
  cctype_t *type_msvc_int8;
  cctype_t *type_msvc_int16;
  cctype_t *type_msvc_int32;
  cctype_t *type_msvc_int64;
  cctype_t *type_void_ptr;
  cctype_t *type_stdc_char_ptr;
} ccseer_t;


ccfunc ccinle ccesse_t *ccesse(ccesse_k);

// Todo: proper allocator!
ccfunc ccinle ccesse_t *
ccesse(ccesse_k kind)
{
  ccesse_t *e=ccmalloc_T(ccesse_t);
  memset(e,ccnull,sizeof(*e));
  e->kind=kind;
  return e;
}

// Todo:
ccfunc cctype_t *
cctype_record(cctree_t *tree, ccelem_t *list, cci32_t size, char *name)
{
  ccassert(tree!=0);

  cctype_t *result=ccmalloc_T(cctype_t);

  result->kind=cctype_kRECORD;
  result->sort=cctoken_kINVALID;
  result->type=ccnull;
  result->name=name;
  result->tree=tree;
  result->size=size;
  result->list=list;
  return result;
}

// Todo:
ccfunc cctype_t *
cctype_pointer_modifier(cctype_t *type)
{
  cctype_t *result=ccmalloc_T(cctype_t);
  result->kind=cctype_kPOINTER;
  result->sort=cctoken_kINVALID;
  result->type=type;
  result->size=sizeof(char*);
  result->list=ccnull;
  return result;
}

// Todo:
ccfunc cctype_t *
cctype_function_modifier(cctype_t *type, ccelem_t *list, int is_variadic)
{
  cctype_t *result=ccmalloc_T(cctype_t);
  result->kind=cctype_kFUNCTION;
  result->sort=cctoken_kINVALID;
  result->type=type;
  result->size=0;
  result->list=list;
  result->is_variadic=is_variadic;
  return result;
}

// Todo:
ccfunc cctype_t *
cctype_array_modifier(cctype_t *type, cci32_t size)
{
  cctype_t *result=ccmalloc_T(cctype_t);
  result->kind=cctype_kARRAY;
  result->sort=cctoken_kINVALID;
  result->type=type;
  result->size=size;
  result->list=ccnull;
  return result;
}

// Todo:
ccfunc cctype_t *
cctype_specifier(cci32_t size, cctoken_k sort)
{
  cctype_t *result=ccmalloc_T(cctype_t);
  result->kind=cctype_kSPECIFIER;
  result->sort=sort;
  result->size=size;
  result->type=ccnull;
  result->list=ccnull;
  return result;
}

// Todo:
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
  if(t->kind==cctype_kFUNCTION)
  { b=cctype_to_string(t->type,b);
    *b++='(';
    *b++='.';
    *b++='.';
    *b++=')';
  } else
  if(t->kind==cctype_kRECORD)
  { *b++='s';
    *b++='t';
    *b++='r';
    *b++='u';
    *b++='c';
    *b++='t';
  } else
  if(t->kind==cctype_kSPECIFIER)
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