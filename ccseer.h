// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCSEER_H
#define _CCSEER_H

// Note: Implicit entities as a means for getting rid of structs entirely:
// We have the concept of an entity or esse, amongst the many things we'd picture an entity
// to be, is a named or explicit variable, and we can further extend that definition to allow
// for implicit entities, entities that are not necessarily named but rather implicitly included
// when you include an explicit entity, so you end up with this hierarchy of entities...
//
// struct _t { int b, c; } a;
// 'a' is an explicit entity, and 'b' and 'c' are implicit and do not collide with any other names outside
// the scope of the struct itself...
//
// Using this system we simplify the emitter stage since struct-typed variables act as a pseudo namespaces...
//
// Note: Downside of implicit entities:
// My biggest oversight was thinking that it would work just fine in the case of pointers too,
// since entities, explicit or implicit, are allocated on the stack in order, memory operations
// such as de-referencing or copying should still work fine and be as simple as they were before.
// Even though this is true, it does not make for a good 'value' system and it isn't as simple.
//
// struct _t { int b, c; };
// _t  a;
// _t *b;
// _t *c;
//
// b=&a;
// a=*b;
// b=c;
//
// '&a': is pretty much an impossible operation, first, what does that even mean, are we getting the address
// of each entity? clearly no, that isn't how things would work, we have to instead get the address of where
// the value 'a' resides, but what IS 'a'? 'a' is undefined, there's no such thing as 'a', there's only
// 'a::b' and 'a::c'. Now, you could ask for the address of the first member of 'a', which is possible
// but that doesn't fix the next problem.
//
// '*b': is pretty much also, an impossible operation since 'b' is undefined, there's only 'b::b' and 'b::c'.
//
// In summary, the problem is that you need some sort of tangible value to reference and operate on, but there's
// none.
//
//
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
  // Note: we could recycle all these tables for the entire compilation step...
  ccesse_t  ** entity_table;
  ccesse_t  ** symbol_table;

  // Note: I think we just have have to make seer emit entities for everything, including
  // constants...
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