/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
/*****************************************************************/

ccfunc cctree_t *
ccread_parameter_type_list(ccreader_t *parser, ktt_i32 *is_variadic);

/////////////////////////////////
// designator:
//   [ constant-expression ]
//   . identifier
//
// designator-list:
//   designator
//   designator-list designator
//
// designation:
//   designator-list =
//
// init-designation:
//   designation(opt) initializer
//
// initializer-list:
//   init-designation(opt)
//   initializer-list, init-designation
//
// initializer:
//   assignment-expression
//   { initializer-list }
//   { initializer-list , }
//
/////////////////////////////////

ccfunc cctree_t *
ccread_designator(ccreader_t *parser)
{ if(ccsee(parser, cctokentype_lsquare))
  { cctok_t *tok = ccgobble(parser);
    cctree_t  *cex = ccread_constant_expression(parser);
    return cctree_new_designator(tok, cex);
  } else
  if(ccsee(parser, cctokentype_mso))
  { cctok_t *tok = ccgobble(parser);
    cctree_t  *cex = ccread_identifier(parser);
    return cctree_new_designator(tok, cex);
  }
  return ktt__nullptr;
}

ccfunc cctree_t *
ccread_designator_list(ccreader_t *parser)
{ cctree_t *result = ccread_designator(parser);
  if(result)
  { result->designator.next = ccread_designator_list(parser);
  }
  return result;
}

ccfunc cctree_t *
ccread_init_designation(ccreader_t *parser)
{ cctree_t *list = ccread_designator_list(parser);
  if(list)
  { if(! cceat(parser, cctokentype_assign))
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
ccread_initializer_list(ccreader_t *parser)
{ cctree_t *desi = ccread_init_designation(parser);

  if(cceat(parser, cctokentype_comma))
  { if(! desi)
    { ccsynerr(parser, 0, "unexpected ',' in initializer list");
    }
    // Keep parsing, extra comma isn't a big deal ...
    desi->designation.next = ccread_initializer_list(parser);
  }
  return desi;
}

ccfunc cctree_t *
ccread_initializer(ccreader_t *parser)
{ if(cceat(parser, cctokentype_lcurly))
  {
    cctree_t *list = ccread_initializer_list(parser);

    if(! cceat(parser, cctokentype_rcurly))
    { ccsynerr(parser, 0, "expected '}' for initializer list");
    }
    return list;
  } else
  {
    return ccread_assignment_expr(parser);
  }
}
/**
 * direct-declarator-modifier:
 *   direct-declarator-modifier [ type-qualifier-list(opt) assignment-expression(opt) ]
 *   direct-declarator-modifier [ static type-qualifier-list(opt) assignment-expression ]
 *   direct-declarator-modifier [ type-qualifier-list static assignment-expression ]
 *   direct-declarator-modifier [ type-qualifier-list(opt) * ]
 *   direct-declarator-modifier ( parameter-type-list )
 *   direct-declarator-modifier ( identifier-list(opt) )    ** obsolete **
 **/
ccfunc cctype_t *
ccread_direct_decl_name_modifier(ccreader_t *reader, cctype_t *type)
{
  if(cceat(reader, cctokentype_lparen))
  {
    ktt_i32 is_vari;
    cctree_t *list = ccread_parameter_type_list(reader, & is_vari);
    (void)list;

    if(!cceat(reader, cctokentype_rparen)) ccsynerr(reader, 0, "expected closing ')' for function modifier");

    cctype_t *modifier = ccread_direct_decl_name_modifier(reader, type);

    if(modifier->kind==cctype_fun) ccsynwar(reader,0,"function that returns function");
    if(modifier->kind==cctype_arr) ccsynwar(reader,0,"function that returns array");

    return cctype_new_fun(modifier);
  } else
  if(cceat(reader, cctokentype_lsquare))
  {
    cctree_t *expression = ccread_expression(reader);
    (void) expression;

    if(! cceat(reader, cctokentype_rsquare))
    { ccsynerr(reader, 0, "expected ']' for array modifier");
    }

    cctype_t *modifier = ccread_direct_decl_name_modifier(reader, type);
    return cctype_new_arr(modifier);
  }
  return type;
}
//
// direct-declarator:
//   identifier
//   ( declarator )
//   direct-declarator direct-declarator-modifier
//
ccfunc cctree_t *
ccread_direct_decl_name(ccreader_t *reader, cctype_t *type)
{ if(cceat(reader,cctokentype_lparen))
  { cctype_t *mod,*tmp;
    cctree_t *res;
    mod=cctype_clone(type);
    res=ccread_decl_name(reader,mod);
    if(!cceat(reader, cctokentype_rparen)) ccsynerr(reader, 0, "expected closing ')' for declarator");
    tmp=ccread_direct_decl_name_modifier(reader,type);
    *mod=*tmp;
    cctype_del(tmp);
    return res;
  } else
  if(ccsee(reader, cctokentype_literal_identifier))
  { cctree_t *nam=ccread_identifier(reader);
    cctype_t *mod=ccread_direct_decl_name_modifier(reader,type);
    return cctree_new_decl_name(mod,nam);
  } else // Note: abstract declarator then ...
  { cctype_t *mod=ccread_direct_decl_name_modifier(reader,type);
    return cctree_new_decl_name(mod,0);
  }
}
/**
 * declarator-pointer:
 *  type-qualifier-list(opt)
 *  type-qualifier-list(opt) declarator-pointer
 **/
ccfunc cctype_t *
ccread_decl_name_modifier_maybe(ccreader_t *parser, cctype_t *type)
{
  if(cceat(parser, cctokentype_mul))
    return cctype_new_ptr(ccread_decl_name_modifier_maybe(parser, type));

  return type;
}
//
// declarator:
//   pointer(opt) direct-declarator
//
ccfunc cctree_t *
ccread_decl_name(ccreader_t *reader, cctype_t *type)
{
  ccassert(type!=0);

  return ccread_direct_decl_name(reader,ccread_decl_name_modifier_maybe(reader,type));
}
//
//  init-declarator:
//    declarator
//    declarator = initializer
//
ccfunc cctree_t *
ccread_init_decl_name(ccreader_t *reader, cctype_t *type)
{
  ccassert(type!=0);

  cctree_t *decl=ccread_decl_name(reader, type);
  cctree_t *init=0;

  if(cceat(reader,cctokentype_assign))
  { init=ccread_initializer(reader);
  }

  return cctree_new_init_decl_name(decl,init);
}
//
// init-declarator-list:
//   init-declarator
//   init-declarator-list , init-declarator
//
ccfunc cctree_t *
ccread_init_decl_name_list(ccreader_t *reader, cctype_t *type)
{ cctree_t *next,*list=ccarr_empty;
  do
  { next=ccread_init_decl_name(reader,type);
    if(next) *ccarr_add(list,1)=*next;
    cctree_del(next);
  } while(cceat(reader,cctokentype_comma));
  return list;
}
//
// declaration:
//   declaration-specifiers attribute-seq (opt) init-declarator-list (opt) ;
//   static_assert-declaration
//
ccfunc cctree_t *
ccread_init_decl(ccreader_t *reader)
{ cctype_t *type;
  type=ccread_declaration_specifiers(reader);

  if(!type) return 0;

  ccread_attribute_seq(reader);

  cctree_t *list;
  list=ccread_init_decl_name_list(reader,type);

  return cctree_new_init_decl(type,list);
}
//
// struct-declarator:
//   declarator
//   declarator(opt) : constant-expression
//
ccfunc cctree_t *
ccread_struct_decl_name(ccreader_t *reader, cctype_t *type)
{ cctree_t *decl,*cexp=ccarr_empty;
  decl=ccread_decl_name(reader,type);
  if(cceat(reader,cctokentype_colon))
  { cexp=ccread_constant_expression(reader);
  }
  return cctree_new_struct_decl_name(decl,cexp);
}
//
// struct-declarator-list:
//   struct-declarator
//   struct-declarator-list , struct-declarator
//
ccfunc cctree_t *
ccread_struct_decl_name_list(ccreader_t *parser, cctype_t *type)
{ cctree_t *next,*list=ccarr_empty;
  do
  { next=ccread_struct_decl_name(parser,type);
    if(next) *ccarr_add(list,1)=*next;
    cctree_del(next);
  } while(next!=0&&cceat(parser,cctokentype_comma));
  return list;
}
//
// struct-declaration:
//   specifier-qualifier-list struct-declarator-list(opt);
//   static_assert-declaration
//
ccfunc cctree_t *
ccread_struct_decl(ccreader_t *parser)
{ cctype_t *type;
  cctree_t *list;
  if(type=ccread_specifier_qualifier_list(parser))
  { list=ccread_struct_decl_name_list(parser,type);
    if(!parser->bed->term_expl) ccsynerr(parser, 0, "expected ';' at end of struct-declaration");
    return cctree_new_struct_decl(type,list);
  }
  return ktt__nullptr;
}
//
// struct-declaration-list:
//   struct-declaration
//   struct-declaration-list struct-declaration
//
ccfunc cctree_t *
ccread_struct_decl_list(ccreader_t *parser)
{ cctree_t *next,*list=ccarr_empty;
  while(next=ccread_struct_decl(parser))
  { // Todo:
   *ccarr_add(list,1)=*next;
    cctree_del(next);
  }
  return list;
}

// struct-or-union:
//    struct
//    union
//  struct-or-union-specifier:
//    struct-or-union identifier(opt) { struct-declaration-list }
//    struct-or-union identifier
ccfunc cctype_t *
ccread_struct_or_union_specifier(ccreader_t *reader)
{ if(cceat(reader, cctokentype_struct))
  { cctree_t *name,*list;
    cctype_t *type;
    name=ccread_identifier(reader);
    if(!cceat(reader, cctokentype_lcurly))
      ccsynerr(reader, 0, "expected '{' for struct specifier");
    list=ccread_struct_decl_list(reader);
    if(!cceat(reader, cctokentype_rcurly))
      ccsynerr(reader, 0, "expected '}' for struct specifier");
    type=cctype_new_struct_spec(name,list);
    return type;
  } else
  // if(cceat(reader, cctokentype_enum))
  {
    ccassert(!"notimpl");

    if(cctree_t *name = ccread_identifier(reader))
    { return cctree_new_enum_specifier(name);
    }
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
ccread_type_specifier(ccreader_t *parser)
{ // Todo: make this proper ....
  if(cctok_t *tok = ccsee_typespec(parser))
  { switch(tok->bit)
    { case cctokentype_enum:
      case cctokentype_struct:     return ccread_struct_or_union_specifier(parser);
      case cctokentype_msvc_int8:  return ccgobble(parser), ctype_int8;
      case cctokentype_msvc_int16: return ccgobble(parser), ctype_int16;
      case cctokentype_msvc_int32: return ccgobble(parser), ctype_int32;
      case cctokentype_msvc_int64: return ccgobble(parser), ctype_int64;
      case cctokentype_bool:       return ccgobble(parser), ctype_int8;
      case cctokentype_void:       return ccgobble(parser), ctype_void;
      case cctokentype_char:       return ccgobble(parser), ctype_int8;
      case cctokentype_short:      return ccgobble(parser), ctype_int16;
      case cctokentype_int:        return ccgobble(parser), ctype_int32;
      case cctokentype_float:      return ccgobble(parser), ctype_flo32;
      case cctokentype_double:     return ccgobble(parser), ctype_flo64;
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
ccread_storage_class_specifier(ccreader_t *parser)
{
  if(kttc__peek_storage_class(parser))
  {
    cctok_t *tok = ccgobble(parser);
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
ccread_type_qualifier(ccreader_t *parser)
{ if(kttc__peek_type_qualifier(parser))
  { cctok_t *tok = ccgobble(parser);
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
ccread_function_specifier(ccreader_t *parser)
{ if(kttc__peek_func_specifier(parser))
  { cctok_t *tok = ccgobble(parser);
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
ccread_alignment_specifier(ccreader_t *parser)
{ if(kttc__peek_alignment_specifier(parser))
  { cctok_t *tok = ccgobble(parser);
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
ccread_specifier_qualifier_list(ccreader_t *parser)
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
ccread_attribute_seq(ccreader_t *parser)
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
ccread_declaration_specifiers(ccreader_t *parser)
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
ccread_parameter_declaration(ccreader_t *parser)
{
  if(cctype_t *decl_spec = ccread_declaration_specifiers(parser))
  { cctree_t *decl = ccread_decl_name(parser, decl_spec); // <-- abtract
    // Just make every decl have next pointer.
    return kttc__make_parameter_declaration(decl);
  }
  return ktt__nullptr;
}

ccfunc cctree_t *
ccread_parameter_list(ccreader_t *parser)
{
  // Try a little harder here to check for misplaced commas ...
  while(cceat(parser, cctokentype_comma))
  { ccsynerr(parser, 0, "misplaced or too many ',' in parameter list");
  }

  // Todo: check here for misplaced ellipsis
  // Todo: replace with array ...
  if(cctree_t *list = ccread_parameter_declaration(parser))
  { if(cceat(parser, cctokentype_comma))
    { list->parameter_declaration.next = ccread_parameter_list(parser);
    }
    return list;
  }
  return ktt__nullptr;
}

ccfunc cctree_t *
ccread_parameter_type_list(ccreader_t *parser, ktt_i32 *is_variadic)
{ if(is_variadic) * is_variadic = false;
  cctree_t *list = ccread_parameter_list(parser);
  if(cceat(parser, cctokentype_literal_ellipsis))
  { if(is_variadic) * is_variadic = true;
  }
  return list;
}
