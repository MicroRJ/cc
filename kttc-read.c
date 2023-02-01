/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
/*****************************************************************/

ccfunc void
ccreader_init(ccreader_t *parser)
{ memset(parser, 0, sizeof(*parser));
  cclex_init(& parser->lex);
  parser->bed = 0;
  parser->min = 0;
  parser->max = 0;
}

ccfunc void
ccreader_uninit(ccreader_t *parser)
{ cclex_uninit(& parser->lex); // <-- free all the string memory.
  ccarr_del(parser->buf); // <-- free all the buffered tokens.
}

ccfunc void
ccread_all_tokens(ccreader_t *parser);

// Todo: reset the token array ...
ccfunc void
ccreader_move(ccreader_t *parser, size_t len, const char *min)
{ cclex_move(& parser->lex, len, min);
  ccread_all_tokens(parser);
  parser->bed = 0;
  parser->min = parser->buf;
  parser->max = parser->buf + ccarr_len(parser->buf);
}

ccfunc void
ccreader_file(ccreader_t *reader, const char *name)
{
  unsigned long int size;
	void *file=ccopenfile(name);
  void *data=ccpullfile(file,0,&(size=0));
  ccclosefile(file);

  ccreader_move(reader,size,(char*)data);

  ccfree(data);
}

ccfunc void
ccread_all_tokens(ccreader_t *parser)
{ while(cclex_next_token(& parser->lex))
  {
    cctok_t *token=ccarr_add(parser->buf,1);
    cclex_token(& parser->lex,token);
  }
}

ccfunc cctok_t *
kttc__peek_ahead(ccreader_t *parser, ktt_i32 offset)
{ if((parser->min + offset < parser->max))
  { return parser->min + offset;
  }
  // TODO(RJ): this should point to a valid location in a file?
  static cctok_t end_tok = { cctokentype_end }; // <-- hopefully no-one modifies this.
  return & end_tok;
}

ccfunc cctok_t *
ccpeep(ccreader_t *parser)
{ return kttc__peek_ahead(parser, 0);
}

ccfunc ktt_i32
ccsee(ccreader_t *parser, cctokentype_kind kind)
{ return ccpeep(parser)->bit == kind;
}

ccfunc ktt_i32
ccsee_end(ccreader_t *parser)
{ return ccsee(parser, cctokentype_end);
}

ccfunc ktt_i32
kttc__peek_oper_increment(ccreader_t *parser)
{
  cctok_t *tok0 = kttc__peek_ahead(parser, 0);
  cctok_t *tok1 = kttc__peek_ahead(parser, 1);
  return (tok0->sig == cctokentype_add) && (tok1->sig == cctokentype_add);
}

ccfunc ktt_i32
kttc__peek_oper_decrement(ccreader_t *parser)
{
  cctok_t *tok0 = kttc__peek_ahead(parser, 0);
  cctok_t *tok1 = kttc__peek_ahead(parser, 1);
  return (tok0->sig == cctokentype_sub) &&  (tok1->sig == cctokentype_sub);
}

ccfunc cctok_t *
kttc__consume_oper_increment(ccreader_t *parser, cctokentype_kind new_sig)
{
  if(kttc__peek_oper_increment(parser))
  { cctok_t *tok = ccgobble(parser);
    tok->sig = new_sig;
    ccgobble(parser);
    return tok;
  }

  return ktt__nullptr;
}

ccfunc cctok_t *
kttc__consume_oper_decrement(ccreader_t *parser, cctokentype_kind new_sig)
{
  if(kttc__peek_oper_decrement(parser))
  { cctok_t *tok = ccgobble(parser);
    tok->sig = new_sig;
    ccgobble(parser);
    return tok;
  }

  return ktt__nullptr;
}

ccfunc cctok_t *
kttc__peek_alignment_specifier(ccreader_t *parser)
{
  cctok_t *token = ccpeep(parser);

  if(token->bit > kttc__algn_spec_0 &&
     token->bit < kttc__algn_spec_1)
  {
    return token;
  }
  return ktt__nullptr;
}

ccfunc cctok_t *
kttc__peek_type_qualifier(ccreader_t *parser)
{
  cctok_t *token = ccpeep(parser);

  if(token->bit > cctype_qual_0 &&
     token->bit < cctype_qual_1)
  {
    return token;
  }
  return ktt__nullptr;
}

ccfunc cctok_t *
ccsee_typespec(ccreader_t *parser)
{
  cctok_t *token = ccpeep(parser);

  if(token->bit > cctype_spec_0 &&
     token->bit < cctype_spec_1)
  {
    return token;
  }
  return ktt__nullptr;
}

ccfunc cctok_t *
kttc__peek_storage_class(ccreader_t *parser)
{
  cctok_t *token = ccpeep(parser);

  if(token->bit > kttc__scls_spec_0 &&
     token->bit < kttc__scls_spec_1)
  {
    return token;
  }
  return ktt__nullptr;
}

ccfunc cctok_t *
kttc__peek_func_specifier(ccreader_t *parser)
{
  cctok_t *token = ccpeep(parser);

  if(token->bit > kttc__func_spec_0 &&
     token->bit < kttc__func_spec_1)
  {
    return token;
  }
  return ktt__nullptr;
}

ccfunc cctok_t *
ccgobble(ccreader_t *reader)
{ if((reader->min<reader->max)) return reader->bed = reader->min ++;
  return ccpeep(reader); // <-- use peek here to return special end token.
}

// NOTE(RJ): gotta be careful with how you inline this in a function call, order of execution my not be what you'd
// expect, especially if one of the arguments is recursive.
ccfunc cctok_t *
cceat(ccreader_t *parser, cctokentype_kind kind)
{ if(ccsee(parser,kind)) return ccgobble(parser);
  return 0;
}

ccfunc cctype_t *
cctype_new(cctypekind_t kind)
{ cctype_t *result = (cctype_t *) ccmalloc(sizeof(cctype_t));
  memset(result, 0, sizeof(*result));
  result->kind = kind;
  return result;
}

ccfunc cctree_t *
cctree_new(cctreetype_t kind)
{ cctree_t *result = (cctree_t *) ccmalloc(sizeof(cctree_t));
  memset(result, 0, sizeof(*result));
  result->kind = kind;
  return result;
}

ccfunc void
cctype_del(cctype_t *type)
{ ccfree(type);
}

ccfunc void
cctree_del(cctree_t *tree)
{ ccfree(tree);
}

ccfunc cctype_t *
cctype_clone(cctype_t *type)
{ cctype_t *result = cctype_new(type->kind);
  memcpy(result, type, sizeof(*type));
  return result;
}

ccfunc cctype_t *
cctree_t_clone(cctype_t *type)
{ cctype_t *result = cctype_new(type->kind);
  memcpy(result, type, sizeof(*type));
  return result;
}

ccfunc cctype_t *
cctype_new_ptr(cctype_t *modifier_of)
{ cctype_t *type = cctype_new(cctype_ptr);
  type->modifier_of = modifier_of;
  return type;
}

ccfunc cctype_t *
cctype_new_arr(cctype_t *modifier_of)
{ cctype_t *type = cctype_new(cctype_arr);
  type->modifier_of = modifier_of;
  return type;
}

ccfunc cctype_t *
cctype_new_fun(cctype_t *modifier_of)
{ cctype_t *type = cctype_new(cctype_fun);
  type->modifier_of = modifier_of;
  return type;
}

ccfunc cctree_t *
cctree_new_decl_name(cctype_t *type, cctree_t *name)
{ cctree_t *tree = cctree_new(cctree_decl_name);
  tree->decl_name.type=type;
  tree->decl_name.name=name;
  return tree;
}

ccfunc cctree_t *
cctree_new_init_decl_name(cctree_t *decl, cctree_t *init)
{ cctree_t *tree = cctree_new(cctree_init_decl_name);
  tree->init_decl_name.decl=decl;
  tree->init_decl_name.init=init;
  return tree;
}

ccfunc cctree_t *
cctree_new_init_decl(cctype_t *type, cctree_t *list)
{ cctree_t *tree = cctree_new(cctree_init_decl);
  tree->init_decl.type=type;
  tree->init_decl.list=list;
  return tree;
}

ccfunc cctree_t *
cctree_new_designator(cctok_t *token, cctree_t *expr)
{ cctree_t *tree = cctree_new(cctree_t_designator);
  tree->designator.token = * token;
  tree->designator.expr  = expr;
  return tree;
}

ccfunc cctree_t *
cctree_new_constant(cctype_t *type, cctok_t *token)
{ cctree_t *result = cctree_new(cctree_t_integer);
  result->constant.type  = type;
  result->constant.token = *token;
  return result;
}

ccfunc cctree_t *
cctree_new_top(cctok_t *token, cctree_t *lhs, cctree_t *mhs, cctree_t *rhs)
{ cctree_t *result = cctree_new(cctree_t_bop);
  result->top.opr = * token;
  result->top.lhs = lhs;
  result->top.mhs = mhs;
  result->top.rhs = rhs;
  return result;
}

ccfunc cctree_t *
cctree_new_bop(cctok_t *token, cctree_t *lhs, cctree_t *rhs)
{ cctree_t *result = cctree_new(cctree_t_bop);
  result->bop.opr = * token;
  result->bop.lhs = lhs;
  result->bop.rhs = rhs;
  return result;
}

ccfunc cctree_t *
cctree_new_uop(cctok_t *token, cctree_t *mhs)
{ cctree_t *result = cctree_new(cctree_t_uop);
  result->uop.opr = * token;
  result->uop.mhs = mhs;
  return result;
}


ccfunc cctree_t *
cctree_new_identifier(cctok_t *token)
{ // Make sure we return null here, not just for safety but because other functions
  // depend on it for convenience.
  if(token)
  {
    cctree_t *tree = cctree_new(cctree_t_iname);
    tree->constant.token = * token;
    return tree;
  }
  return ktt__nullptr;
}

ccfunc cctree_t *
cctree_new_struct_decl_name(cctree_t *decl, cctree_t *expr)
{ ccassert(decl!=0);
	cctree_t *tree = cctree_new(cctree_struct_decl_name);
  tree->struct_decl_name.decl = decl;
  tree->struct_decl_name.expr = expr;
  return tree;
}
ccfunc cctree_t *
cctree_new_struct_decl(cctype_t *type, cctree_t *list)
{ ccassert(type!=0);
	ccassert(list!=0);
	cctree_t *tree = cctree_new(cctree_struct_decl);
  tree->struct_decl.type = type;
  tree->struct_decl.list = list;
  return tree;
}
ccfunc cctype_t *
cctype_new_struct_spec(cctree_t *name, cctree_t *list)
{ ccassert(list!=0);
	cctype_t *type=cctype_new(cctype_struct_spec);
  type->name = name;
  type->list = list;
  return type;
}

ccfunc cctype_t *
cctree_new_enum_specifier(cctree_t *name)
{ (void)name;
  // cctree_t *tree = cctree_new(cctree_t_enum_specifier);
  // tree->enum_specifier.name = name;
  // return tree;
  return cctype_new(cctype_enum_specifier);
}
ccfunc cctree_t *
cctree_new_designation(cctree_t *list, cctree_t *init)
{ // Make sure we return null here, not just for safety but because other functions
  // depend on it for convenience.
  if(list)
  {
    cctree_t *tree = cctree_new(cctree_t_designation);
    tree->designation.list = list;
    tree->designation.init = init;
    return tree;
  }
  return ktt__nullptr;
}

ccfunc cctree_t *
kttc__make_parameter_declaration(cctree_t *decl)
{
  if(decl)
  {
    cctree_t *tree = cctree_new(cctree_t_parameter_declaration);
    tree->parameter_declaration.decl = decl;
    return tree;
  }
  return ktt__nullptr;
}

/********************************************************
 * This file structure:                                 *
 *  Productions are listed in descending order, also    *
 *  split into three major categories, translation-unit,*
 *  declarations and expressions.                       *
 ********************************************************/
//
//  Productions will be split into three major categories, translation unit, declarations, expressions.
//  Productions will be listed in descending order, so terminals will be at the very end.

// https://learn.microsoft.com/en-us/cpp/c-language/source-files-and-source-programs?view=msvc-170
// https://learn.microsoft.com/en-us/cpp/c-language/c-function-definitions?view=msvc-170
//
//
// translation-unit:
//   external-declaration
//   translation-unit external-declaration
//
// external-declaration: /* Allowed only at external (file) scope */
//   function-definition
//   declaration
//
// function-definition:
//   declaration-specifiers(opt) attribute-seq(opt) declarator declaration-list(opt) compound-statement
//
// /* attribute-seq is Microsoft-specific */
//
// Prototype parameters are:
//
// declaration-specifiers:
//     storage-class-specifier declaration-specifiers(opt)
//     type-specifier declaration-specifiers(opt)
//     type-qualifier declaration-specifiers(opt)
//
// declaration-list:
//     declaration
//     declaration-list declaration
//
// declarator:
//     pointer(opt) direct-declarator
//
// direct-declarator: /* A function declarator */
//     direct-declarator ( parameter-type-list ) /* New-style declarator */
//     direct-declarator ( identifier-list(opt) ) /* Obsolete-style declarator */
//
// The parameter list in a definition uses this syntax:
//
// parameter-type-list: /* The parameter list */
//     parameter-list
//     parameter-list , ...
//
// parameter-list:
//     parameter-declaration
//     parameter-list , parameter-declaration
//
// parameter-declaration:
//     declaration-specifiers declarator
//     declaration-specifiers abstract-declarator(opt)
//
// The parameter list in an old-style function definition uses this syntax:
//
// identifier-list: /* Used in obsolete-style function definitions and declarations */
//     identifier
//     identifier-list , identifier
//
// The syntax for the function body is:
//
// compound-statement:
//     { declaration-list(opt) statement-list(opt) }
ccfunc cctree_t *
ccread_translation_unit(ccreader_t *parser)
{

  (void) parser;
}

ccfunc cctree_t *
ccread_external_declaration(ccreader_t *parser)
{
  (void) parser;
}























