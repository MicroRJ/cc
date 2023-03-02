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

// Note: entity or essential, indivisible and relatively unique, such as variables ...
typedef struct ccesse_t ccesse_t;
typedef struct ccesse_t
{ ccesse_k        kind;
  const char    * name;
  ccesse_t      * root;
  cctype_t      * type;
  cctree_t      * tree;
  ccbuiltin_k     sort;
  ccesse_t     ** list; // Todo: is there a better way of doing this ...
} ccesse_t;

// Todo: legit scoping
// Note: #entity_table: Map of named entities, use "string"...
// Note: #symbol_table: Map of named or implicit entities, use symbolic tree...
// Note: #tether_table: Map of type info for each tree...
typedef struct ccseer_t ccseer_t;
typedef struct ccseer_t
{
  // Todo: I think that this is going to be key in getting rid of structs entirely.
  // We have the concept of an entity which is a named variable, but we could extend that definition
  // to allow for implicit entities, member variables, which are still unique.
  // We can use the symbol table along with selector trees to find them.
  // The idea is that you have this hierarchy of memory operands, in which
  // member variables are simply relative offsets, but they are also entities...
  // Since struct typed entities are orderly allocated on the stack, memory operations
  // such as de-referencing or copying a struct typed variable should still work fine and be
  // as simple as it was before.
  // The first step towards that would be having a more robust and cleaner front-end..
  //

  // Note: we could recycle all these tables for the entire compilation step...
  ccesse_t  ** entity_table;
  ccesse_t  ** symbol_table;
  cctype_t  ** tether_table;

  // Todo: how to actually do this properly and where to store it and what types
  // do we actually need...
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

ccfunc ccinle const char *
ccesse_kind_string(ccesse_t *esse)
{ switch(esse->kind)
  { case ccesse_kINVALID:  return "invalid";
    case ccesse_kTYPENAME: return "type-name";
    case ccesse_kCBINDING: return "c-binding";
    case ccesse_kFUNCTION: return "function";
    case ccesse_kVARIABLE: return "variable";
  }
  return "error";
}

ccfunc char *
ccesse_name_string(ccesse_t *esse, char **stringer)
{
  char *buffer=stringer?*stringer:ccnull;

  if(esse->root)
  {
    ccesse_name_string(esse->root,&buffer);
    ccstrcatL(buffer,".");
  }

  ccstrcatS(buffer,esse->name);

  if(stringer) *stringer=buffer;

  return buffer;
}


#endif