/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
/*****************************************************************/
//
// Note: No static language specific analysis done here, this file is meant to be modular in this manner...

ccfunc void
ccread_init(ccread_t *_r)
{
	memset(_r,ccnil,sizeof(*_r));
  cclex_init(& _r->lex);
}

ccfunc void
ccread_uninit(ccread_t *_r)
{
	cclex_uninit(&_r->lex);
  ccarrdel(_r->buf);
}

ccfunc void
ccread_all_tokens(ccread_t *_r);

// Todo: reset the token array ...
ccfunc void
ccreader_move(ccread_t *_r, size_t _l, const char *_d)
{
  cclex_move(&_r->lex,_l,_d);
  ccread_all_tokens(_r);

  _r->bed=0;
  _r->min=_r->buf;
  _r->max=_r->buf+ccarrlen(_r->buf);
}

ccfunc void
ccread_include(ccread_t *_r, const char *name)
{
  unsigned long int size=0;
  void *file=ccopenfile(name);
  void *data=ccpullfile(file,0,&size);
  ccclosefile(file);

  ccreader_move(_r,size,(char*)data);

  // Todo:
  // ccfree(data);
}

ccfunc void
ccread_all_tokens(ccread_t *_r)
{ while(cclex_next_token(& _r->lex))
  {
    cctoken_t *token=ccarradd(_r->buf,1);
    cclex_token(& _r->lex,token);
  }
}

ccfunc cctoken_t *
ccpeek(ccread_t *_r, cci32_t _o)
{
	if((_r->min+_o<_r->max))
  {
  	return _r->min+_o;
  }

  // Note: is this flawed?
  static cctoken_t end_tok = { cctoken_kEND };
  return & end_tok;
}

ccfunc ccinle cctoken_t *
ccpeep(ccread_t *_r)
{
	return ccpeek(_r,0);
}

ccfunc ccinle int
ccsee(ccread_t *_r, cctoken_k kind)
{
	return ccpeep(_r)->bit==kind;
}

ccfunc ccinle int
ccsee_end(ccread_t *_r)
{
	return ccsee(_r,cctoken_kEND);
}

// Note: I keep coming up with these names ...
ccfunc ccinle cctoken_t *
ccgobble(ccread_t *_r)
{
	// Todo: instead of saving the last token, just save its flags, that's was we're really looking for ...
	if(_r->min<_r->max)
		return _r->bed=_r->min++;

  return ccpeep(_r); // <-- use peek here to return special end token.
}

ccfunc ccinle cctoken_t *
cceat(ccread_t *_r, cctoken_k _k)
{
	if(ccsee(_r,_k))
		return ccgobble(_r);
  return 0;
}

// Todo: most of the time there's a switch statement do then handle each specifier,
// is this really necessary or is it just more overhead ...

// Todo: remove, redundant ...
ccfunc cctoken_t *
kttc__peek_alignment_specifier(ccread_t *parser)
{
  cctoken_t *token = ccpeep(parser);

  if(token->bit > kttc__algn_spec_0 &&
     token->bit < kttc__algn_spec_1)
  {
    return token;
  }
  return ccnil;
}

// Todo: remove, redundant ...
ccfunc cctoken_t *
kttc__peek_type_qualifier(ccread_t *parser)
{
  cctoken_t *token = ccpeep(parser);

  if(token->bit > cctype_qual_0 &&
     token->bit < cctype_qual_1)
  {
    return token;
  }
  return ccnil;
}

// Todo: remove, redundant ...
ccfunc cctoken_t *
ccsee_typespec(ccread_t *parser)
{
  cctoken_t *token = ccpeep(parser);

  if(token->bit > cctype_spec_0 &&
     token->bit < cctype_spec_1)
  {
    return token;
  }
  return ccnil;
}

// Todo: remove, redundant ...
ccfunc cctoken_t *
kttc__peek_storage_class(ccread_t *parser)
{
  cctoken_t *token = ccpeep(parser);

  if(token->bit > kttc__scls_spec_0 &&
     token->bit < kttc__scls_spec_1)
  {
    return token;
  }
  return ccnil;
}

// Todo: remove, redundant ...
ccfunc cctoken_t *
kttc__peek_func_specifier(ccread_t *parser)
{
  cctoken_t *token = ccpeep(parser);

  if(token->bit > kttc__func_spec_0 &&
     token->bit < kttc__func_spec_1)
  {
    return token;
  }
  return ccnil;
}

ccfunc cctree_t *
ccread_arglist_expr(ccread_t *reader, cctree_t *root, cci32_t mark);
ccfunc cctree_t *
ccread_expression(ccread_t *parser, cctree_t *root, cci32_t mark);
ccfunc cctree_t *
ccread_cast_expr(ccread_t *parser, cctree_t *root, cci32_t mark);

ccfunc ccinle cctree_t *
ccread_identifier(ccread_t *reader, cctree_t *root, cci32_t mark)
{ cctoken_t *token=cceat(reader,cctoken_kLITIDENT);
	cctree_t *tree=ccnil;
	if(token) tree=cctree_identifier(root,mark,token->str);
	return tree;
}

// Section: EXPRESSIONS
// https://learn.microsoft.com/en-us/cpp/c-language/summary-of-expressions
ccfunc cctree_t *
ccread_primary(ccread_t *reader, cctree_t *root, cci32_t mark)
{ cctoken_t *token=ccpeep(reader);
	if(!token) return ccnil;
	switch(token->bit)
	{ case cctoken_kLITIDENT:
			return ccread_identifier(reader,root,mark);
	  case cctoken_kLITINTEGER:
  		return cctree_constant(root,mark,ctype_int64,ccgobble(reader));
	  case cctoken_kLITFLOAT:
  		return cctree_constant(root,mark,ctype_flo64,ccgobble(reader));
	  case cctoken_kLITSTRING:
	  	ccassert(!"error");
  		// return cctree_constant(root,mark,cctreee_array_modifier(ctype_int8),ccgobble(reader));
	  case cctoken_kLPAREN:
	  { cctree_t *group=ccread_expression(reader,root,mark);
	    if(!cceat(reader,cctoken_kRPAREN)) cctraceerr("expected ')'");
	    return cctree_group(root,mark,group);
	  }
	}
  return ccnil;
}

/**
 * postfix-expression:
 *   primary-expression
 *   postfix-expression [ expression ]
 *   postfix-expression ( argument-expression-listopt )
 *   postfix-expression . identifier
 *   postfix-expression -> identifier
 *   postfix-expression ++
 *   postfix-expression --
 *   ( type-name ) { initializer-list }
 *   ( type-name ) { initializer-list , }
 **/
//
// Todo:
ccfunc cctree_t *
ccread_postfix(ccread_t *reader, cctree_t *root, cci32_t mark)
{
	// Todo:
	// ccsee(reader,cctoken_kARROW)

  cctree_t *lhs = ccread_primary(reader,root,mark);

  if(cceat(reader, cctoken_kLPAREN))
  {
  	cctree_t *args = ccread_arglist_expr(reader,root,mark);

    if(!cceat(reader,cctoken_kRPAREN))
    	ccsynerr(reader, 0, "expected ')'");

    return cctree_call(root,mark,lhs,args);
  } else
  if(ccsee(reader, cctoken_kLSQUARE))
  {
  	cctree_t *args = ccread_expression(reader,root,mark);

    if(!cceat(reader,cctoken_kRSQUARE))
    	ccsynerr(reader, 0, "expected ']'");

    return cctree_index(root,mark,lhs,args);
  } else
  if(ccsee(reader,cctoken_kDOT))
  { lhs = cctree_unary(root,mark,ccgobble(reader),lhs);
  } else
  if(cctoken_t *inc = cceat(reader, cctoken_Kpos_increment))
  { lhs = cctree_unary(root,mark,inc,lhs);
  } else
  if(cctoken_t *dec = cceat(reader, cctoken_Kpos_decrement))
  { lhs = cctree_unary(root,mark,inc,lhs);
  }
  return lhs;
}

/**
 * unary-expression:
 *   postfix-expression
 *   ++ unary-expression
 *   -- unary-expression
 *   unary-operator cast-expression
 *   sizeof unary-expression
 *   sizeof ( type-name )  _Alignof ( type-name )
 *
 * unary-operator: one of
 *   & * + - ~ !
 **/
ccfunc cctree_t *
ccread_unary_expr(ccread_t *reader, cctree_t *root, cci32_t mark)
{ cctree_t *result = ccnil;

  if(ccsee(reader,cctoken_kADD))
  { cctoken_t *tok=ccgobble(reader);
    if(cceat(reader,cctoken_kADD))
    { // TODO(RJ): this is dumb, remove...
      cctoken_t clo=*tok;
      clo.sig=cctoken_Kpre_increment;
      result=cctree_unary(root,mark, & clo, ccread_cast_expr(reader,root,mark));
    } else
    { result=cctree_unary(root,mark, tok, ccread_cast_expr(reader,root,mark));
    }
  } else
  if(ccsee(reader,cctoken_kSUB))
  { cctoken_t *tok=ccgobble(reader);
    if(cceat(reader,cctoken_kSUB))
    { // TODO(RJ): this is dumb, remove...
      cctoken_t clo=*tok;
      clo.sig=cctoken_Kpre_decrement;
      result=cctree_unary(root,mark, & clo, ccread_cast_expr(reader,root,mark));
    } else
    { result=cctree_unary(root,mark, tok, ccread_cast_expr(reader,root,mark));
    }
  } else
  if(ccsee(reader, cctoken_kMUL))
  {
    // TODO(RJ): this is dumb, remove...
    cctoken_t *tok = ccgobble(reader);
    cctoken_t clo = *tok;
    clo.sig = cctoken_Kptr_dereference;

    result = cctree_unary(root,mark, & clo, ccread_cast_expr(reader,root,mark));
  } else
  if(ccsee(reader, cctoken_Kbitwise_invert) ||
     ccsee(reader, cctoken_Knegate))
  { result = cctree_unary(root,mark, ccgobble(reader), ccread_cast_expr(reader,root,mark));
  } else
  { result = ccread_postfix(reader,root,mark);
  }
  return result;
}
/**
 * cast-expression:
 *   unary-expression
 *   ( type-name ) cast-expression
 **/
ccfunc cctree_t *
ccread_cast_expr(ccread_t *parser, cctree_t *root, cci32_t mark)
{ cctree_t *result = 0;
  if(cctoken_t *tok = cceat(parser, cctoken_kLPAREN))
  { // TODO(RJ):
    result = 0;

    if(! cceat(parser, cctoken_kRPAREN))
    { ccsynerr(parser, 0, "expected ')'");
    }
  } else
  { result = ccread_unary_expr(parser,root,mark);
  }
  return result;
}
/**
 * multiplicative-expression:
 *   cast-expression
 *   multiplicative-expression * cast-expression
 *   multiplicative-expression / cast-expression
 *   multiplicative-expression % cast-expression
 **/
ccfunc cctree_t *
ccread_multiplicative_expr(ccread_t *parser, cctree_t *root, cci32_t mark)
{ cctree_t *lhs = ccread_unary_expr(parser,root,mark);
  while(  ccsee(parser, cctoken_kMUL) ||
          ccsee(parser, cctoken_kDIV) ||
          ccsee(parser, cctoken_Kmod) )
  {
    cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_unary_expr(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
  return lhs;
}
/**
 * additive-expression:
 *   multiplicative-expression
 *   additive-expression + multiplicative-expression
 *   additive-expression - multiplicative-expression
 **/
ccfunc cctree_t *
ccread_additive_expr(ccread_t *parser, cctree_t *root, cci32_t mark)
{ cctree_t *lhs = ccread_multiplicative_expr(parser,root,mark);
  while (ccsee(parser, cctoken_kADD) ||
         ccsee(parser, cctoken_kSUB))
  {
    cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_multiplicative_expr(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
  return lhs;
}
/**
 * shift-expression:
 *   additive-expression
 *   shift-expression << additive-expression
 *   shift-expression >> additive-expression
 **/
ccfunc cctree_t *
ccread_shift_expr(ccread_t *parser, cctree_t *root, cci32_t mark)
{ cctree_t *lhs = ccread_additive_expr(parser,root,mark);
  while(ccsee(parser, cctoken_Kbitwise_shl) ||
        ccsee(parser, cctoken_Kbitwise_shr))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_additive_expr(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
  return lhs;
}
/**
 * relational-expression:
 *   shift-expression
 *   relational-expression < shift-expression
 *   relational-expression > shift-expression
 *   relational-expression <= shift-expression
 *   relational-expression >= shift-expression
 **/
// for the greater than, you can instead use the less than with the operands flipped...
ccfunc cctree_t *
ccread_relational_expr(ccread_t *parser, cctree_t *root, cci32_t mark)
{ cctree_t *lhs = ccread_shift_expr(parser,root,mark);
  while ( ccsee(parser, cctoken_kLTN)      ||
          ccsee(parser, cctoken_kLTE)  ||
          ccsee(parser, cctoken_kGTN)   ||
          ccsee(parser, cctoken_kGTE) )
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_shift_expr(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
  return lhs;
}
/**
 * equality-expression:
 *  relational-expression
 *  equality-expression == relational-expression
 *  equality-expression != relational-expression
 **/
ccfunc cctree_t *
ccread_equality_expr(ccread_t *parser, cctree_t *root, cci32_t mark)
{ cctree_t *lhs = ccread_relational_expr(parser,root,mark);
  if(ccsee(parser, cctoken_Kequals)     ||
     ccsee(parser, cctoken_Knot_equals))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_equality_expr(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
  return lhs;
}

/**
 * AND-expression:
 *   equality-expression
 *   AND-expression & equality-expression
 **/
ccfunc cctree_t *
ccread_bitwise_and_expr(ccread_t *parser, cctree_t *root, cci32_t mark)
{ cctree_t *lhs = ccread_equality_expr(parser,root,mark);
  while(ccsee(parser, cctoken_Kbitwise_and))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_equality_expr(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
  return lhs;
}
/**
 * exclusive-OR-expression:
 *   AND-expression
 *   exclusive-OR-expression ^ AND-expression
 **/
ccfunc cctree_t *
ccread_bitwise_xor_expr(ccread_t *parser, cctree_t *root, cci32_t mark)
{ cctree_t *lhs = ccread_bitwise_and_expr(parser,root,mark);
  while(ccsee(parser, cctoken_Kbitwise_and))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_bitwise_and_expr(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
  return lhs;
}
/**
 * inclusive-OR-expression:
 *   exclusive-OR-expression
 *   inclusive-OR-expression | exclusive-OR-expression
 **/
ccfunc cctree_t *
ccread_bitwise_or_expr(ccread_t *parser, cctree_t *root, cci32_t mark)
{ cctree_t *lhs = ccread_bitwise_xor_expr(parser,root,mark);
  while(ccsee(parser, cctoken_Kbitwise_or))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_bitwise_xor_expr(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
  return lhs;
}
/**
 * logical-AND-expression:
 *   inclusive-OR-expression
 *   logical-AND-expression && inclusive-OR-expression
 **/
ccfunc cctree_t *
ccread_logical_and_expr(ccread_t *parser, cctree_t *root, cci32_t mark)
{ cctree_t *lhs = ccread_bitwise_or_expr(parser,root,mark);
  while(ccsee(parser, cctoken_Klogical_and))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_bitwise_or_expr(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
  return lhs;
}
/**
 * logical-OR-expression:
 *   logical-AND-expression
 *   logical-OR-expression || logical-AND-expression
 **/
ccfunc cctree_t *
ccread_logical_or_expr(ccread_t *parser, cctree_t *root, cci32_t mark)
{ cctree_t *lhs = ccread_logical_and_expr(parser,root,mark);
  while(ccsee(parser, cctoken_Klogical_or))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_logical_and_expr(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
  return lhs;
}
/**
 * conditional-expression:
 *   logical-OR-expression
 *   logical-OR-expression ? expression : conditional-expression
 **/
ccfunc cctree_t *
ccread_conditional_expr(ccread_t *parser, cctree_t *root, cci32_t mark)
{
	cctree_t *rad=ccread_logical_or_expr(parser,root,mark);

  if(cceat(parser, cctoken_Kconditional))
  {
    cctree_t *lhs = 0, *rhs = 0;
    lhs = ccread_logical_or_expr(parser,root,mark);

    if(cceat(parser, cctoken_Kcolon)) // <-- some compilers allow omitting this.
    { rhs = ccread_conditional_expr(parser,root,mark);
    } else
    { ccsynerr(parser, 0, "expected ':' invalid conditional expression"); // <-- but we issue a syntax error anyways.
    }

    return cctree_ternary(root,mark,rad,lhs,rhs);
  }

  return rad;
}
/**
 * assignment-expression:
 *   conditional-expression
 *   unary-expression assignment-operator assignment-expression
 *
 * assignment-operator: one of
 *   = *= /= %= += -= <<= >>= &= ^= |=
 **/
ccfunc cctree_t *
ccread_assignment_expr(ccread_t *parser, cctree_t *root, cci32_t mark)
{ cctree_t *lhs = ccread_conditional_expr(parser,root,mark);
  if(cctoken_t *tok = cceat(parser, cctoken_kASSIGN))
  { cctree_t *rhs = ccread_assignment_expr(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
  return lhs;
}
/**
 * constant-expression:
 *   conditional-expression
 **/
ccfunc cctree_t *
ccread_constant_expression(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  return ccread_conditional_expr(reader,root,mark);
}
/**
 * expression:
 *   assignment-expression
 *   expression , assignment-expression
 **/
ccfunc cctree_t *
ccread_expression(ccread_t *reader, cctree_t *root, cci32_t mark)
{ cctree_t *result=ccread_assignment_expr(reader,root,mark);
  if(ccsee(reader, cctoken_Kcomma))
  { result=cctree_binary(root,mark,ccgobble(reader),result,ccread_expression(reader,root,mark));
  }
  return result;
}
/**
 * argument-expression-list:
 *   assignment-expression
 *   argument-expression-list , assignment-expression
 **/
ccfunc cctree_t *
ccread_arglist_expr(ccread_t *reader, cctree_t *root, cci32_t mark)
{ cctree_t *list=ccnil,*next;
  do
  { next=ccread_assignment_expr(reader,root,mark);
    if(next) *ccarradd(list,1)=*next;
    cctree_del(next);
  } while(next!=0&&cceat(reader,cctoken_Kcomma));
  return list;
}

// Section: DECLARATIONS

ccfunc cctree_t **
ccread_param_type_list(ccread_t *reader, cctree_t *root, cci32_t mark);

ccfunc cctree_t *
ccread_initializer(ccread_t *reader, cctree_t *root, cci32_t mark);

ccfunc cctree_t *
ccread_decl_name(ccread_t *reader, cctree_t *root, cci32_t mark, cctree_t *type);

ccfunc cctree_t *
ccread_declaration_specifiers(ccread_t *reader, cctree_t *root, cci32_t mark);

ccfunc cci32_t
ccread_attribute_seq(ccread_t *reader, cctree_t *root, cci32_t mark);

ccfunc cctree_t *
ccread_specifier_qualifier_list(ccread_t *reader, cctree_t *root, cci32_t mark);

#if 0
ccfunc cctree_t *
ccread_designator(ccread_t *reader, cctree_t *root, cci32_t mark)
{
	if(ccsee(reader, cctoken_kLSQUARE))
  { cctoken_t *tok = ccgobble(reader);
    cctree_t  *cex = ccread_constant_expression(reader,root,mark);
    return cctree_new_designator(tok, cex);
  } else
  if(ccsee(reader, cctoken_kDOT))
  { cctoken_t *tok = ccgobble(reader);
    cctree_t  *cex = ccread_identifier(reader,root,mark);
    return cctree_new_designator(tok, cex);
  }
  return ccnil;
}

ccfunc cctree_t *
ccread_designator_list(ccread_t *reader, cctree_t *root, cci32_t mark)
{
	cctree_t *result = ccread_designator(reader,root,mark);
  if(result)
  { result->designator.next = ccread_designator_list(reader,root,mark);
  }
  return result;
  return ccnil;
}

ccfunc cctree_t *
ccread_init_designation(ccread_t *reader, cctree_t *root, cci32_t mark)
{
	cctree_t *list = ccread_designator_list(reader,root,mark);
  if(list)
  { if(! cceat(reader, cctoken_kASSIGN))
    { ccsynerr(reader, 0, "expected '=' for designation");
    }
  }
  cctree_t *init = ccread_initializer(reader,root,mark); // <-- designators are optional, check for an intializer ...
  if(list)
  { if(! init) // <-- if we have a designator ensure that we have an initializer ...
    { ccsynerr(reader, 0, "expected 'initializer' for designator");
    }
  }
  return cctree_new_designation(list, init);
	return ccnil;
}

ccfunc cctree_t *
ccread_initializer_list(ccread_t *reader, cctree_t *root, cci32_t mark)
{ cctree_t *desi=ccread_init_designation(reader,root,mark);

  if(cceat(reader,cctoken_Kcomma))
  { if(! desi)
    { ccsynerr(reader,0,"unexpected ',' in initializer list");
    }
    desi->designation.next=ccread_initializer_list(reader,root,mark);
  }
  return desi;
}
#endif

ccfunc cctree_t *
ccread_initializer(ccread_t *reader, cctree_t *root, cci32_t mark)
{
#if 0
	if(cceat(reader, cctoken_Klcurly))
  {
    cctree_t *list = ccread_initializer_list(reader,root,mark);

    if(! cceat(reader, cctoken_Krcurly))
    { ccsynerr(reader, 0, "expected '}' for initializer list");
    }
    return list;
  } else
  {
    return ccread_assignment_expr(reader,root,mark);
  }
#endif
  return ccread_assignment_expr(reader,root,mark);
}

ccfunc cctree_t *
ccread_direct_decl_name_modifier(ccread_t *reader, cctree_t *root, cci32_t mark, cctree_t *type)
{ if(cceat(reader, cctoken_kLPAREN))
  { cctree_t **list=ccread_param_type_list(reader,root,mark);
    if(!cceat(reader, cctoken_kRPAREN)) ccsynerr(reader, 0, "expected ')'");
    cctree_t *modifier=ccread_direct_decl_name_modifier(reader,root,mark,type);
    if(modifier->kind==cctree_kFUNC) ccsynwar(reader,0,"function that returns function");
    if(modifier->kind==cctree_kARRAY) ccsynwar(reader,0,"function that returns array");
    return cctreee_function_modifier(modifier,list);
  } else
  if(cceat(reader, cctoken_kLSQUARE))
  {
    cctree_t *rval=ccread_expression(reader,root,mark);

    if(!cceat(reader,cctoken_kRSQUARE))
    	ccsynerr(reader,0,"expected ']'");

    cctree_t *modi=ccread_direct_decl_name_modifier(reader,root,mark,type);
    return cctreee_array_modifier(modi,rval);
  }
  return type;
}

ccfunc cctree_t *
ccread_direct_decl_name(ccread_t *reader, cctree_t *root, cci32_t mark, cctree_t *type)
{ if(cceat(reader,cctoken_kLPAREN))
  { cctree_t *mod,*tmp;
    cctree_t *res;
    mod=cctree_clone(type);
    res=ccread_decl_name(reader,root,mark,mod);
    if(!cceat(reader, cctoken_kRPAREN)) ccsynerr(reader, 0, "expected ')'");
    tmp=ccread_direct_decl_name_modifier(reader,root,mark,type);
    *mod=*tmp;
    cctree_del(tmp);
    return res;
  } else
  if(ccsee(reader, cctoken_kLITIDENT))
  { cctree_t *name=ccread_identifier(reader,root,mark);
		cctree_t *mod=ccread_direct_decl_name_modifier(reader,root,mark,type);
    return cctree_decl_name(root,mark, mod,name,ccnil,ccnil);
  } else // Note: abstract declarator then ...
  { cctree_t *mod=ccread_direct_decl_name_modifier(reader,root,mark,type);
    return cctree_decl_name(root,mark, mod,0,ccnil,ccnil);
  }
}

ccfunc ccinle cctree_t *
ccread_decl_name_modifier_maybe(ccread_t *reader, cctree_t *root, cci32_t mark, cctree_t *type)
{ if(cceat(reader,cctoken_kMUL))
    return cctreee_pointer_modifier(ccread_decl_name_modifier_maybe(reader,root,mark,type));
  return type;
}

ccfunc ccinle cctree_t *
ccread_decl_name(ccread_t *reader, cctree_t *root, cci32_t mark, cctree_t *type)
{ ccnotnil(type);
  cctree_t *tree;
  tree=ccread_direct_decl_name(reader,root,mark,
  	ccread_decl_name_modifier_maybe(reader,root,mark,type));
	return tree;
}

ccfunc cctree_t *
ccread_init_decl_name(ccread_t *reader, cctree_t *root, cci32_t mark, cctree_t *type)
{ ccassert(type!=0);

  cctree_t *decl=ccread_decl_name(reader,root,mark,type);

  if(decl)
  { if(cceat(reader,cctoken_kASSIGN))
    { decl->init=ccread_initializer(reader,root,mark);
      if(!decl->init) ccsynerr(reader,0,"expected initializer after '='");
    }
  }
  return decl;
}

ccfunc cctree_t *
ccread_struct_decl_name(ccread_t *reader, cctree_t *root, cci32_t mark, cctree_t *type)
{ ccassert(type!=0);
  cctree_t *decl=ccread_decl_name(reader,root,mark,type);
  if(decl)
  { if(cceat(reader,cctoken_Kcolon))
    { decl->size=ccread_constant_expression(reader,root,mark);
      if(!decl->size) ccsynerr(reader,0,"expected constant expression after ':'");
    }
  }
  return decl;
}

ccfunc cctree_t **
ccread_init_decl_name_list(ccread_t *reader, cctree_t *root, cci32_t mark, cctree_t *type)
{ ccnotnil(root);
	cctree_t *next,**list=ccnil;
  do
  { next=ccread_init_decl_name(reader,root,mark,type);
    if(!next) break;
    *ccarradd(list,1)=next;
  } while(cceat(reader,cctoken_Kcomma));
  return list;
}

ccfunc cctree_t **
ccread_struct_decl_name_list(ccread_t *reader, cctree_t *root, cci32_t mark, cctree_t *type)
{ cctree_t *next,**list=ccnil;
  do
  { next=ccread_struct_decl_name(reader,root,mark,type);
    if(next) *ccarradd(list,1)=next;
  } while(next!=0&&cceat(reader,cctoken_Kcomma));
  return list;
}

ccfunc cctree_t *
ccread_init_decl(ccread_t *reader, cctree_t *root, cci32_t mark)
{ cctree_t *type; cctree_t **list;
  if(type=ccread_declaration_specifiers(reader,root,mark))
  { ccread_attribute_seq(reader,root,mark);
    if(list=ccread_init_decl_name_list(reader,root,mark,type))
    { return cctree_decl(root,mark,type,list);
    } else ccsynerr(reader,0,"expected declarator");
  }
  return ccnil;
}

ccfunc cctree_t *
ccread_struct_decl(ccread_t *reader, cctree_t *root, cci32_t mark)
{ cctree_t *type; cctree_t **list;
  if(type=ccread_specifier_qualifier_list(reader,root,mark))
  { if(list=ccread_struct_decl_name_list(reader,root,mark,type))
    { return cctree_decl(root,mark,type,list);
    } else ccsynerr(readerm,0,"expected declarator");
  }
  return ccnil;
}

ccfunc cctree_t *
ccread_struct_or_union_specifier(ccread_t *reader, cctree_t *root, cci32_t mark)
{ if(cceat(reader, cctoken_kSTRUCT))
  { cctree_t *name;
    cctree_t *tree;
    name=ccread_identifier(reader,root,mark);
    if(!cceat(reader, cctoken_Klcurly))
      ccsynerr(reader,0,"expected '{' for struct specifier");
    // Todo: semicolon handling, proper root ...
    cctree_t *next,**list=ccnil;
  	while(next=ccread_struct_decl(reader,root,mark))
  	{ *ccarradd(list,1)=next;
  		if(!reader->bed->term_expl) ccsynerr(reader,0,"expected ';'");
  	}
    if(!cceat(reader, cctoken_Krcurly))
      ccsynerr(reader,0,"expected '}' for struct specifier");
    tree=cctreee_struct_specifier(list,name);
    return tree;
  } else
  if(cceat(reader, cctoken_Kenum))
  {
    ccassert(!"noimpl");
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

ccfunc cctree_t *
ccread_type_specifier(ccread_t *reader, cctree_t *root, cci32_t mark)
{ // Todo: make this proper ....
  if(cctoken_t *tok = ccsee_typespec(reader))
  { switch(tok->bit)
    { case cctoken_Kenum:
      case cctoken_kSTRUCT:     return ccread_struct_or_union_specifier(reader,root,mark);
      case cctoken_Kmsvc_int8:  return ccgobble(reader), ctype_int8;
      case cctoken_Kmsvc_int16: return ccgobble(reader), ctype_int16;
      case cctoken_Kmsvc_int32: return ccgobble(reader), ctype_int32;
      case cctoken_Kmsvc_int64: return ccgobble(reader), ctype_int64;
      case cctoken_Kbool:       return ccgobble(reader), ctype_int8;
      case cctoken_Kvoid:       return ccgobble(reader), ctype_void;
      case cctoken_Kchar:       return ccgobble(reader), ctype_int8;
      case cctoken_Kshort:      return ccgobble(reader), ctype_int16;
      case cctoken_Kint:        return ccgobble(reader), ctype_int32;
      case cctoken_Kfloat:      return ccgobble(reader), ctype_flo32;
      case cctoken_Kdouble:     return ccgobble(reader), ctype_flo64;
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
ccfunc cci32_t
ccread_storage_class_specifier(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  if(kttc__peek_storage_class(reader))
  {
    cctoken_t *tok = ccgobble(reader);
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
ccfunc cci32_t
ccread_type_qualifier(ccread_t *reader, cctree_t *root, cci32_t mark)
{ if(kttc__peek_type_qualifier(reader))
  { cctoken_t *tok = ccgobble(reader);
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
ccfunc cci32_t
ccread_function_specifier(ccread_t *reader, cctree_t *root, cci32_t mark)
{ if(kttc__peek_func_specifier(reader))
  { cctoken_t *tok = ccgobble(reader);
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
ccfunc cci32_t
ccread_alignment_specifier(ccread_t *reader, cctree_t *root, cci32_t mark)
{ if(kttc__peek_alignment_specifier(reader))
  { cctoken_t *tok = ccgobble(reader);
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
ccfunc cctree_t *
ccread_specifier_qualifier_list(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *type = ccread_type_specifier(reader,root,mark);
  ccread_type_qualifier(reader,root,mark);
  ccread_alignment_specifier(reader,root,mark);

  return type;
}
//
// attribute-seq:
//   attribute attribute-seq(opt)
// attribute: one of
//   __asm __based __cdecl __clrcall __fastcall __inline __stdcall __thiscall __vectorcall
//
ccfunc cci32_t
ccread_attribute_seq(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  (void)reader;
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
ccfunc cctree_t *
ccread_declaration_specifiers(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  ccread_storage_class_specifier(reader,root,mark);

  cctree_t *type = ccread_specifier_qualifier_list(reader,root,mark);

  ccread_function_specifier(reader,root,mark);

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
ccread_param_decl(ccread_t *reader, cctree_t *root, cci32_t mark)
{
	cctree_t *spec=ccnil;
	cctree_t *decl=ccnil;

	if(ccsee(reader,cctoken_Kliteral_ellipsis))
		ccsynerr(reader,0,"unexpected '...', must be at end of function");

	spec=ccread_declaration_specifiers(reader,root,mark);
  if(spec) decl=ccread_decl_name(reader,root,mark,spec);

  return decl;
}

ccfunc cctree_t **
ccread_param_decl_list(ccread_t *reader, cctree_t *root, cci32_t mark)
{ cctree_t *next,**list=ccnil;
	while(next=ccread_param_decl(reader,root,mark))
	{ *ccarradd(list,1)=next;
		if(!cceat(reader,cctoken_Kcomma)) break;
	}
  return list;
}

ccfunc cctree_t **
ccread_param_type_list(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t **list=ccread_param_decl_list(reader,root,mark);

  // Todo:
  // if(cceat(reader, cctoken_Kliteral_ellipsis))

  return list;
}
// Section: STATEMENTS

ccfunc cctree_t *ccread_block(ccread_t *reader, cctree_t *root, cci32_t mark);
ccfunc cctree_t **ccread_statement_list(ccread_t *reader, cctree_t *root, cci32_t mark);
ccfunc cctree_t *ccread_statement(ccread_t *reader, cctree_t *root, cci32_t mark);

ccfunc cctree_t *
ccread_block_or_single_stmt(ccread_t *reader, cctree_t *root, cci32_t mark)
{ cctree_t *stmt=ccread_block(reader,root,mark);
  if(!stmt) stmt=ccread_statement(reader,root,mark);
  return stmt;
}

// Badger Tadger
ccfunc cctree_t *
ccread_statement(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *child=ccnil;

  if(ccsee(reader,cctoken_Krcurly))
  {
    // Note: don't do anything ...
  } else
  if(cceat(reader,cctoken_Klcurly))
  { child=ccread_block(reader,root,mark);
    if(!cceat(reader,cctoken_Krcurly)) ccsynerr(reader, 0, "expected '}'");
    return child;
  } else
  if(child=ccread_init_decl(reader,root,mark))
  { if(!reader->bed->term_expl) ccsynerr(reader, 0, "expected ';'");
    return child;
  } else
  if(cceat(reader,cctoken_Kreturn))
  {
  	cctree_t *expr_tree=ccread_expression(reader,root,mark);
  	child=cctree_return(root,mark,expr_tree);

    if(!expr_tree) ccsynerr(reader,0,"expected expression");

    if(!reader->bed->term_expl) ccsynerr(reader, 0, "expected ';'");
  } else
  if(cceat(reader,cctoken_Kwhile))
  {
    cctree_t *cond_tree=ccnil,*then_tree=ccnil;

    if(!cceat(reader,cctoken_kLPAREN)) ccsynerr(reader,0,"expected '('");
    cond_tree=ccread_expression(reader,root,mark);
    if(!cceat(reader,cctoken_kRPAREN)) ccsynerr(reader,0,"expected ')'");

    if(!cond_tree) ccsynerr(reader,0,"expected expression");

    if(!reader->bed->term_expl)
    {
      then_tree=ccread_block_or_single_stmt(reader,root,mark);
      if(!then_tree) ccsynerr(reader,0,"expected statement");
    }
    child=cctree_while(root,mark,cond_tree,then_tree);
  } else
  if(cceat(reader,cctoken_Kgoto))
  {
    cctree_t *ident=ccread_identifier(reader,root,mark);
    if(!ident) ccsynerr(reader,0,"missing goto label identifier");

    child=cctree_goto(root,mark,ident);

    if(!reader->bed->term_expl) ccsynerr(reader, 0, "expected ';'");
  } else
  if(ccsee(reader,cctoken_Kelse))
  {
    // Note: don't do anything, this could be intentional ...
  } else
  if(cceat(reader,cctoken_Kif))
  {
    cctree_t *cond_tree=ccnil;
    cctree_t *then_tree=ccnil;
    cctree_t *else_tree=ccnil;

    if(!cceat(reader,cctoken_kLPAREN)) ccsynerr(reader,0,"expected '('");
    cond_tree=ccread_expression(reader,root,mark);
    if(!cceat(reader,cctoken_kRPAREN)) ccsynerr(reader,0,"expected ')'");

    if(!cond_tree) ccsynerr(reader,0,"expected expression");

    if(!reader->bed->term_expl)
    { then_tree=ccread_block_or_single_stmt(reader,root,mark);
      if(!then_tree) ccsynerr(reader,0,"expected statement");
    }
    if(cceat(reader,cctoken_Kelse)&&!reader->bed->term_expl)
    { else_tree=ccread_block_or_single_stmt(reader,root,mark);
      if(!else_tree) ccsynerr(reader,0,"expected statement");
    }

    child=cctree_ternary(root,mark,cond_tree,then_tree,else_tree);

  } else
  // Note: make sure you call this last, simply because this is a rather expensive function ... not that I care ...
  if(child=ccread_expression(reader,root,mark))
  {
    // Todo: better way of doing this?
    if(!reader->bed->term_expl)
    {
      if(child->kind==cctree_kIDENTIFIER)
      {
        if(cceat(reader,cctoken_Kcolon))
        {

        	cctree_t **list=ccnil;
        	if(!reader->bed->term_expl)
        	{ list=ccread_statement_list(reader,root,mark);
        	}
          child=cctree_label(root,mark,child,list);
        } else
          ccsynerr(reader, 0, "invalid statement, missing ':' for label statement?");
      } else
        ccsynerr(reader, 0, "invalid statement");

    }
    return child;
  }
  return child;
}

ccfunc cctree_t **
ccread_statement_list(ccread_t *reader, cctree_t *root, cci32_t mark)
{ cctree_t *next,**list=ccnil;
  while(next=ccread_statement(reader,root,mark))
  { *ccarradd(list,1)=next;
    if(ccsee_end(reader)) break;
  }
  return list;
}

ccfunc cctree_t *
ccread_block(ccread_t *reader, cctree_t *root, cci32_t mark)
{ cctree_t *tree=ccnil;
	if(cceat(reader,cctoken_Klcurly))
  { tree=cctree_block(root,mark,ccnil);
  	tree->list=ccread_statement_list(reader,tree,mark);
    if(!cceat(reader,cctoken_Krcurly))
      ccsynerr(reader, 0, "expected '}'");
  }
  return tree;
}




// Section: TRANSLATION UNIT

ccfunc cctree_t *
ccread_external_declaration(ccread_t *reader, cctree_t *root, cci32_t mark);

ccfunc cctree_t *
ccread_translation_unit(ccread_t *reader)
{
	cctree_t *tree=cctree_translation_unit();

  while(cctree_t *next=ccread_external_declaration(reader,tree,0))
  	*ccarradd(tree->list,1)=next;

  return tree;
}

ccfunc cctree_t *
ccread_external_declaration(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *decl=ccread_init_decl(reader,root,mark|cctree_mLVALUE|cctree_mEXTERNAL);

  if(!decl) return ccnil;

  ccassert(decl->kind==cctree_kDECL);
  ccnotnil(decl->list);

  cctree_t *name=*decl->list;

  ccassert(name->kind==cctree_kDECLNAME);
  ccnotnil(name->type);
  ccnotnil(name->name);

  if(name->type->kind==cctree_kFUNC)
  {
    // Note: You can't define multiple functions within the same declaration ...
    ccassert(ccarrlen(decl->list)==1);

    name->blob=ccread_block(reader,name,mark);

    if(!name->blob&&!reader->bed->term_expl) ccsynerr(reader,0,"expected ';'");
  } else
  {
    // Note: applies to every declaration other than a function declaration with its definition ...
    if(!reader->bed->term_expl) ccsynerr(reader,0,"expected ';'");
  }

  if(!decl&&!ccsee_end(reader)) ccsynerr(reader,0,"invalid external declaration");
  return decl;
}


