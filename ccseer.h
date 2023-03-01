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


typedef enum ccesse_k ccesse_k;
typedef enum ccesse_k
{ ccesse_kINVALID=0,
  ccesse_kTYPENAME,
  ccesse_kCBINDING,
  ccesse_kFUNCTION,
  ccesse_kVARIABLE,
} ccesse_k;

typedef struct ccesse_t ccesse_t;

// Note: entity or essential, indivisible and relatively unique, such as variables ...
typedef struct ccesse_t ccesse_t;
typedef struct ccesse_t
{ ccesse_k        kind;
  const char    * name;
  cctree_t      * tree;
  ccbuiltin_k     sort;
  cctype_t      * type;
} ccesse_t;

// Todo: legit scoping
// Note: #entity_table: Maps a "string" name to an entity
// Note: #symbol_table: Maps a "tree" pointer to its respective entities, symbolic trees ...
// Note: #tether_table: Maps a "tree" to a type ...
typedef struct ccseer_t ccseer_t;
typedef struct ccseer_t
{
  ccesse_t  ** entity_table;
  ccesse_t  ** symbol_table;

  // Note: probably get rid of this table...
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


#endif