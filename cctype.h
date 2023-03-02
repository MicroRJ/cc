#ifndef _CCTYPE_H
#define _CCTYPE_H

typedef   enum cctype_k cctype_k;
typedef struct cctype_t cctype_t;
typedef struct ccelem_t ccelem_t;

typedef enum cctype_k cctype_k;
typedef enum cctype_k
{ cctype_kINVALID=0,

  // Todo: rename this ...
  cctype_kSPECIFIER,

  cctype_kRECORD,
  cctype_kFUNCTION,
  cctype_kPOINTER,
  cctype_kARRAY,
} cctype_k;

typedef struct ccelem_t ccelem_t;
typedef struct ccelem_t
{ const char * name;
  cctype_t   * type;
  cctree_t   * tree; // Note: the decl-tree
  cci32_t      slot;
} ccelem_t;

// Note: entity descriptor, also bound to expressions implicitly and explicitly by means of casting ...
typedef struct cctype_t cctype_t;
typedef struct cctype_t
{ const char * name;
  cctype_k     kind;
  cctoken_k    sort;
  cctree_t   * tree;
  cctype_t   * type;
  // Note: remove the table, use plain array ...
  ccelem_t   * list;
  cci32_t      size;

  // Todo: implement...
  unsigned is_variadic: 1;
} cctype_t;

// Todo: move to source file...
ccfunc ccinle int
cctype_indirect(cctype_t *type)
{
  return (type->kind==cctype_kPOINTER)||(type->kind==cctype_kARRAY);
}

// Todo: move to source file...
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

// Todo: move to source file...
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

// Todo: move to source file...
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

// Todo: move to source file...
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

// Todo: move to source file...
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

// Todo: move to source file...
ccfunc char *
cctype_string(cctype_t *type, char **stringer)
{
  if(!type) return "null";

  char *buffer=stringer?*stringer:ccnull;

  if(type->kind==cctype_kPOINTER)
  { ccstrcatL(buffer,"*");
    cctype_string(type->type,&buffer);
  } else
  if(type->kind==cctype_kARRAY)
  { ccstrcatF(buffer,"[%i]",type->size/type->type->size);
    cctype_string(type->type,&buffer);
  } else
  if(type->kind==cctype_kFUNCTION)
  { cctype_string(type->type,&buffer);
    ccstrcatL(buffer,"(..)");
  } else
  if(type->kind==cctype_kRECORD)
  { ccstrcatL(buffer,"struct");
  } else
  if(type->kind==cctype_kSPECIFIER)
  { ccstrcatS(buffer,cctoken_to_string(type->sort));
  } else
    ccassert(!"error");

  if(stringer) *stringer=buffer;

  return buffer;
}

#endif