/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
/*****************************************************************/


ccfunc cctree_t *
ccread_arglist_expr(ccread_t *reader, cctree_t *root, cci32_t mark);
ccfunc cctree_t *
ccread_expression(ccread_t *parser, cctree_t *root, cci32_t mark);
ccfunc cctree_t *
ccread_cast_expr(ccread_t *parser, cctree_t *root, cci32_t mark);

ccfunc ccinle cctree_t *
ccread_solve_symbol(ccread_t *reader, cctree_t *root, const char *name)
{
	cctree_t **local=ccnil;
	cctree_t  *scope=root;

	ccerrset(ccerr_kNIT);
	while(ccerrnit()&&scope)
	{
		while((scope->kind!=cctree_kBLOCK)&&
				  (scope->kind!=cctree_kTUNIT)) scope=scope->root;

		ccassert((scope->kind==cctree_kBLOCK)||
						 (scope->kind==cctree_kTUNIT));

		local=cctblgetS(scope->decl,name);
		scope=scope->root;
	}

	if(!ccerrnon()) cctracewar("undefined symbol \"%s\"",name);

	return ccerrnon()?*local:ccnil;
}

ccfunc ccinle cctree_t *
ccread_identifier(ccread_t *reader, cctree_t *root, cci32_t mark)
{ cctoken_t *token=cceat(reader,cctoken_Kliteral_identifier);
	cctree_t *tree=ccnil;
	if(token)
	{ cctree_t *symbol=ccread_solve_symbol(reader,root,token->str);
		tree=cctree_identifier(root,mark,symbol,token->str);
	}
	return tree;
}


//
//  primary-expression:
//    identifier
//    constant
//    string-literal
//    ( expression )
//    generic-selection
//
ccfunc cctree_t *
ccread_primary_expr(ccread_t *reader, cctree_t *root, cci32_t mark)
{ cctoken_t *token=ccpeep(reader);
	if(!token) return ccnil;

	switch(token->bit)
	{ case cctoken_Kliteral_identifier:
			return ccread_identifier(reader,root,mark);
	  case cctoken_Kliteral_integer:
  		return cctree_constant(root,mark,ctype_int64, ccgobble(reader));
	  case cctoken_Kliteral_float:
  		return cctree_constant(root,mark,ctype_flo64, ccgobble(reader));
	  case cctoken_Kliteral_string:
  		return cctree_constant(root,mark,cctype_new_arr(ctype_int8), ccgobble(reader));

	  case cctoken_Klparen:
	  { cctree_t *group=ccread_expression(reader,root,mark);
	    if(!cceat(reader,cctoken_Klparen)) ccsynerr(reader, 0, "expected ')'");
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
ccread_postfix_expr(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *lhs = ccread_primary_expr(reader,root,mark);

  if(cceat(reader, cctoken_Klparen))
  { cctree_t *args = ccread_arglist_expr(reader,root,mark);
    if(!cceat(reader,cctoken_Krparen)) ccsynerr(reader, 0, "expected ')'");
    return cctree_call(root,mark,lhs,args);
  } else
  if(ccsee(reader, cctoken_Klsquare))
  { cctree_t *expression = ccread_expression(reader,root,mark);
    (void) expression;
    if(! cceat(reader, cctoken_Krsquare))
    { ccsynerr(reader, 0, "expected ']', in postfix expression!");
    }
  } else
  if(ccsee(reader, cctoken_Kmso) ||
     ccsee(reader, cctoken_Kmsp))
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

  if(ccsee(reader,cctoken_Kadd))
  { cctoken_t *tok=ccgobble(reader);
    if(cceat(reader,cctoken_Kadd))
    { // TODO(RJ): this is dumb, remove...
      cctoken_t clo=*tok;
      clo.sig=cctoken_Kpre_increment;
      result=cctree_unary(root,mark, & clo, ccread_cast_expr(reader,root,mark));
    } else
    { result=cctree_unary(root,mark, tok, ccread_cast_expr(reader,root,mark));
    }
  } else
  if(ccsee(reader,cctoken_Ksub))
  { cctoken_t *tok=ccgobble(reader);
    if(cceat(reader,cctoken_Ksub))
    { // TODO(RJ): this is dumb, remove...
      cctoken_t clo=*tok;
      clo.sig=cctoken_Kpre_decrement;
      result=cctree_unary(root,mark, & clo, ccread_cast_expr(reader,root,mark));
    } else
    { result=cctree_unary(root,mark, tok, ccread_cast_expr(reader,root,mark));
    }
  } else
  if(ccsee(reader, cctoken_Kmul))
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
  { result = ccread_postfix_expr(reader,root,mark);
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
  if(cctoken_t *tok = cceat(parser, cctoken_Klparen))
  { // TODO(RJ):
    result = 0;

    if(! cceat(parser, cctoken_Krparen))
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
  while(  ccsee(parser, cctoken_Kmul) ||
          ccsee(parser, cctoken_Kdiv) ||
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
  while (ccsee(parser, cctoken_Kadd) ||
         ccsee(parser, cctoken_Ksub))
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
  while ( ccsee(parser, cctoken_Kless_than)      ||
          ccsee(parser, cctoken_Kless_than_eql)  ||
          ccsee(parser, cctoken_Kgreater_than)   ||
          ccsee(parser, cctoken_Kgreater_than_eql) )
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
