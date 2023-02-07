/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
/*****************************************************************/

ccfunc cctree_t *
ccread_param_type_list(ccread_t *reader);

ccfunc cctree_t *
ccread_initializer(ccread_t *reader);

ccfunc cctree_t *
ccread_designator(ccread_t *parser)
{ if(ccsee(parser, cctoken_Klsquare))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *cex = ccread_constant_expression(parser);
    return cctree_new_designator(tok, cex);
  } else
  if(ccsee(parser, cctoken_Kmso))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *cex = ccread_identifier(parser);
    return cctree_new_designator(tok, cex);
  }
  return ccnil;
}

ccfunc cctree_t *
ccread_designator_list(ccread_t *parser)
{ cctree_t *result = ccread_designator(parser);
  if(result)
  { result->designator.next = ccread_designator_list(parser);
  }
  return result;
}

ccfunc cctree_t *
ccread_init_designation(ccread_t *parser)
{ cctree_t *list = ccread_designator_list(parser);
  if(list)
  { if(! cceat(parser, cctoken_kASSIGN))
    { ccsynerr(parser, 0, "expected '=' for designation");
    }
  }
  cctree_t *init = ccread_initializer(parser); // <-- designators are optional, check for an intializer ...
  if(list)
  { if(! init) // <-- if we have a designator ensure that we have an initializer ...
    { ccsynerr(parser, 0, "expected 'initializer' for designator");
    }
  }
  return cctree_new_designation(list, init);
}

ccfunc cctree_t *
ccread_initializer_list(ccread_t *parser)
{ cctree_t *desi = ccread_init_designation(parser);

  if(cceat(parser, cctoken_Kcomma))
  { if(! desi)
    { ccsynerr(parser, 0, "unexpected ',' in initializer list");
    }
    // Keep parsing, extra comma isn't a big deal ...
    desi->designation.next = ccread_initializer_list(parser);
  }
  return desi;
}

ccfunc cctree_t *
ccread_initializer(ccread_t *parser)
{
	if(cceat(parser, cctoken_Klcurly))
  {
    cctree_t *list = ccread_initializer_list(parser);

    if(! cceat(parser, cctoken_Krcurly))
    { ccsynerr(parser, 0, "expected '}' for initializer list");
    }
    return list;
  } else
  {
    return ccread_assignment_expr(parser);
  }
}

ccfunc cctype_t *
ccread_direct_decl_name_modifier(ccread_t *reader, cctype_t *type)
{ if(cceat(reader, cctoken_Klparen))
  {
    cctree_t *list = ccread_param_type_list(reader);

    if(!cceat(reader, cctoken_Krparen)) ccsynerr(reader, 0, "expected ')'");

    cctype_t *modifier = ccread_direct_decl_name_modifier(reader, type);

    if(modifier->kind==cctype_Kfunc) ccsynwar(reader,0,"function that returns function");
    if(modifier->kind==cctype_arr) ccsynwar(reader,0,"function that returns array");

    return cctype_new_fun(modifier,list);
  } else
  if(cceat(reader, cctoken_Klsquare))
  {
    cctree_t *expression = ccread_expression(reader);
    (void) expression;

    if(! cceat(reader, cctoken_Krsquare))
    { ccsynerr(reader, 0, "expected ']' for array modifier");
    }

    cctype_t *modifier = ccread_direct_decl_name_modifier(reader, type);
    return cctype_new_arr(modifier);
  }
  return type;
}
ccfunc cctree_t *
ccread_direct_decl_name(ccread_t *reader, cctype_t *type)
{ if(cceat(reader,cctoken_Klparen))
  { cctype_t *mod,*tmp;
    cctree_t *res;
    mod=cctype_clone(type);
    res=ccread_decl_name(reader,mod);
    if(!cceat(reader, cctoken_Krparen)) ccsynerr(reader, 0, "expected ')'");
    tmp=ccread_direct_decl_name_modifier(reader,type);
    *mod=*tmp;
    cctype_del(tmp);
    return res;
  } else
  if(ccsee(reader, cctoken_Kliteral_identifier))
  { cctree_t *nam=ccread_identifier(reader);
    cctype_t *mod=ccread_direct_decl_name_modifier(reader,type);
    return cctree_decl_name(mod,nam,ccnil,ccnil);
  } else // Note: abstract declarator then ...
  { cctype_t *mod=ccread_direct_decl_name_modifier(reader,type);
    return cctree_decl_name(mod,0,ccnil,ccnil);
  }
}

ccfunc cctype_t *
ccread_decl_name_modifier_maybe(ccread_t *parser, cctype_t *type)
{
  if(cceat(parser, cctoken_Kmul))
    return cctype_new_ptr(ccread_decl_name_modifier_maybe(parser, type));

  return type;
}

ccfunc cctree_t *
ccread_decl_name(ccread_t *reader, cctype_t *type)
{
  ccassert(type!=0);

  return ccread_direct_decl_name(reader,ccread_decl_name_modifier_maybe(reader,type));
}

ccfunc cctree_t *
ccread_init_decl_name(ccread_t *reader, cctype_t *type)
{ ccassert(type!=0);
  cctree_t *decl=ccread_decl_name(reader, type);
  if(decl)
  { if(cceat(reader,cctoken_kASSIGN))
    { decl->init=ccread_initializer(reader);
      if(!decl->init) ccsynerr(reader,0,"expected initializer after '='");
    }
  }
  return decl;
}

ccfunc cctree_t *
ccread_struct_decl_name(ccread_t *reader, cctype_t *type)
{ ccassert(type!=0);
  cctree_t *decl=ccread_decl_name(reader,type);
  if(decl)
  { if(cceat(reader,cctoken_Kcolon))
    { decl->size=ccread_constant_expression(reader);
      if(!decl->size) ccsynerr(reader,0,"expected constant expression after ':'");
    }
  }
  return decl;
}

ccfunc cctree_t *
ccread_init_decl_name_list(ccread_t *reader, cctype_t *type)
{ cctree_t *next,*list=ccnil;
  do
  { next=ccread_init_decl_name(reader,type);
    if(next) *ccarradd(list,1)=*next;
    cctree_del(next);
  } while(next!=0&&cceat(reader,cctoken_Kcomma));
  return list;
}

ccfunc cctree_t *
ccread_struct_decl_name_list(ccread_t *reader, cctype_t *type)
{ cctree_t *next,*list=ccnil;
  do
  { next=ccread_struct_decl_name(reader,type);
    if(next) *ccarradd(list,1)=*next;
    cctree_del(next);
  } while(next!=0&&cceat(reader,cctoken_Kcomma));
  return list;
}

ccfunc cctree_t *
ccread_init_decl(ccread_t *reader)
{ cctype_t *type; cctree_t *list;
  if(type=ccread_declaration_specifiers(reader))
  { ccread_attribute_seq(reader);
    if(list=ccread_init_decl_name_list(reader,type))
    { return cctree_decl(type,list);
    } else ccsynerr(reader,0,"expected declarator");
  }
  return ccnil;
}

ccfunc cctree_t *
ccread_struct_decl(ccread_t *parser)
{ cctype_t *type; cctree_t *list;
  if(type=ccread_specifier_qualifier_list(parser))
  { if(list=ccread_struct_decl_name_list(parser,type))
    { return cctree_decl(type,list);
    } else ccsynerr(readerm,0,"expected declarator");
  }
  return ccnil;
}

ccfunc cctree_t *
ccread_struct_decl_list(ccread_t *parser)
{ cctree_t *next,*list=ccnil;
  while(next=ccread_struct_decl(parser))
  { *ccarradd(list,1)=*next; cctree_del(next);
  }
  return list;
}

ccfunc cctype_t *
ccread_struct_or_union_specifier(ccread_t *reader)
{ if(cceat(reader, cctoken_Kstruct))
  { cctree_t *name,*list;
    cctype_t *type;
    name=ccread_identifier(reader);
    if(!cceat(reader, cctoken_Klcurly))
      ccsynerr(reader, 0, "expected '{' for struct specifier");
    list=ccread_struct_decl_list(reader);
    if(!cceat(reader, cctoken_Krcurly))
      ccsynerr(reader, 0, "expected '}' for struct specifier");
    type=cctype_new_struct_spec(list,name);
    return type;
  } else
  // if(cceat(reader, cctoken_Kenum))
  {
    ccassert(!"notimpl");
  }
  return 0;
}
//
// type-specifier:
//   void
//   char
//   short
//   int
//   __int8  ** msvc **
//   __int16  ** msvc **
//   __int32  ** msvc **
//   __int64  ** msvc **
//   long
//   float
//   double
//   signed
//   unsigned
//   _Bool
//   _Complex
//   atomic-type-specifier
//   struct-or-union-specifier
//   enum-specifier
//   typedef-name

ccfunc cctype_t *
ccread_type_specifier(ccread_t *parser)
{ // Todo: make this proper ....
  if(cctoken_t *tok = ccsee_typespec(parser))
  { switch(tok->bit)
    { case cctoken_Kenum:
      case cctoken_Kstruct:     return ccread_struct_or_union_specifier(parser);
      case cctoken_Kmsvc_int8:  return ccgobble(parser), ctype_int8;
      case cctoken_Kmsvc_int16: return ccgobble(parser), ctype_int16;
      case cctoken_Kmsvc_int32: return ccgobble(parser), ctype_int32;
      case cctoken_Kmsvc_int64: return ccgobble(parser), ctype_int64;
      case cctoken_Kbool:       return ccgobble(parser), ctype_int8;
      case cctoken_Kvoid:       return ccgobble(parser), ctype_void;
      case cctoken_Kchar:       return ccgobble(parser), ctype_int8;
      case cctoken_Kshort:      return ccgobble(parser), ctype_int16;
      case cctoken_Kint:        return ccgobble(parser), ctype_int32;
      case cctoken_Kfloat:      return ccgobble(parser), ctype_flo32;
      case cctoken_Kdouble:     return ccgobble(parser), ctype_flo64;
    }
  }
  return 0;
}
//
// storage-class-specifier:
//   auto
//   extern
//   register
//   static
//   _Thread_local
//   typedef
//   __declspec ( extended-decl-modifier-seq )
//
ccfunc ktt_i32
ccread_storage_class_specifier(ccread_t *parser)
{
  if(kttc__peek_storage_class(parser))
  {
    cctoken_t *tok = ccgobble(parser);
    (void) tok;

    return true;
  }
  return false;
}
//
// type-qualifier:
//   const
//   restrict
//   volatile
//   _Atomic
//
ccfunc ktt_i32
ccread_type_qualifier(ccread_t *parser)
{ if(kttc__peek_type_qualifier(parser))
  { cctoken_t *tok = ccgobble(parser);
    (void) tok;
    return true;
  }
  return false;
}
/**
 * function-specifier:
 *   inline
 *   _Noreturn
 **/
ccfunc ktt_i32
ccread_function_specifier(ccread_t *parser)
{ if(kttc__peek_func_specifier(parser))
  { cctoken_t *tok = ccgobble(parser);
    (void) tok;
    return true;
  }
  return false;
}
//
// alignment-specifier:
//   _Alignas ( type-name )
//   _Alignas ( constant-expression )
//
ccfunc ktt_i32
ccread_alignment_specifier(ccread_t *parser)
{ if(kttc__peek_alignment_specifier(parser))
  { cctoken_t *tok = ccgobble(parser);
    (void) tok;
    return true;
  }
  return false;
}
//
// specifier-qualifier-list:
//   type-specifier specifier-qualifier-list(opt)
//   type-qualifier specifier-qualifier-list(opt)
//   alignment-specifier specifier-qualifier-list(opt)
//
ccfunc cctype_t *
ccread_specifier_qualifier_list(ccread_t *parser)
{
  cctype_t *type = ccread_type_specifier(parser);
  ccread_type_qualifier(parser);
  ccread_alignment_specifier(parser);

  return type;
}
//
// attribute-seq:
//   attribute attribute-seq(opt)
// attribute: one of
//   __asm __based __cdecl __clrcall __fastcall __inline __stdcall __thiscall __vectorcall
//
ccfunc ktt_i32
ccread_attribute_seq(ccread_t *parser)
{
  (void)parser;
  return false;
}
//
// declaration-specifiers:
//   storage-class-specifier declaration-specifiersopt
//   type-specifier declaration-specifiersopt
//   type-qualifier declaration-specifiersopt
//   function-specifier declaration-specifiersopt
//   alignment-specifier declaration-specifiersopt
//
ccfunc cctype_t *
ccread_declaration_specifiers(ccread_t *parser)
{
  ccread_storage_class_specifier(parser);

  cctype_t *type = ccread_specifier_qualifier_list(parser);

  ccread_function_specifier(parser);

  return type;
}
///////////////////////////////////////////////////////
//
//  parameter-declaration:
//    declaration-specifiers declarator
//    declaration-specifiers abstract-declarator(opt)
//
//  parameter-list:
//    parameter-declaration
//    parameter-list , parameter-declaration
//
//  parameter-type-list:
//    parameter-list
//    parameter-list , ...
//
///////////////////////////////////////////////////////
ccfunc cctree_t *
ccread_param_decl(ccread_t *reader)
{
	cctype_t *spec=ccnil;
	cctree_t *decl=ccnil;

	if(ccsee(reader,cctoken_Kliteral_ellipsis))
		ccsynerr(reader,0,"unexpected '...', must be at end of function");

	spec=ccread_declaration_specifiers(reader);
  if(spec) decl=ccread_decl_name(reader,spec);

  return decl;
}

ccfunc cctree_t *
ccread_param_decl_list(ccread_t *reader)
{
	cctree_t *next,*list=ccnil;
	while(next=ccread_param_decl(reader))
	{
		*ccarradd(list,1)=*next;
		cctree_del(next);

		if(!cceat(reader,cctoken_Kcomma)) break;
	}
  return list;
}

ccfunc cctree_t *
ccread_param_type_list(ccread_t *reader)
{
  cctree_t *list = ccread_param_decl_list(reader);

  // Todo:
  // if(cceat(parser, cctoken_Kliteral_ellipsis))

  return list;
}
