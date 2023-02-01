/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
/*****************************************************************/

ccfunc cctree_t * // <-- will return null if there wasn't an identifier token.
ccread_identifier(ccreader_t *parser)
{ return cctree_new_identifier(cceat(parser, cctokentype_literal_identifier));
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
ccread_primary_expr(ccreader_t *parser)
{
  if(ccsee(parser, cctokentype_literal_identifier))
  { return ccread_identifier(parser);
  } else
  if(ccsee(parser, cctokentype_literal_integer))
  { return cctree_new_constant(ctype_int64, ccgobble(parser));
  } else
  if(ccsee(parser, cctokentype_literal_float))
  { return cctree_new_constant(ctype_flo64, ccgobble(parser));
  } else
  if(ccsee(parser, cctokentype_literal_string))
  { return cctree_new_constant(cctype_new_arr(ctype_int8), ccgobble(parser));
  } else
  if(cceat(parser, cctokentype_lparen))
  { cctree_t *result = ccread_expression(parser);
    if(! cceat(parser, cctokentype_lparen))
    { ccsynerr(parser, 0, "expected ')' for primary expression");
    }
    return result;
  }
  return ktt__nullptr;
}
/**
 * argument-expression-list:
 *   assignment-expression
 *   argument-expression-list , assignment-expression
 **/
ccfunc cctree_t *
ccread_arglist_expr(ccreader_t *reader) // <-- TODO(RJ): return list instead! like other functions!
{ cctree_t *list=ccarr_empty,*next;
	do
	{ next=ccread_assignment_expr(reader);
		if(next) *ccarr_add(list,1)=*next;
		cctree_del(next);
	} while(next!=0&&cceat(reader,cctokentype_comma));
  return list;
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
ccfunc cctree_t *
ccread_postfix_expr(ccreader_t *parser)
{
  cctree_t *lhs = ccread_primary_expr(parser);

  if(cceat(parser, cctokentype_lparen))
  { cctree_t *arglist = ccread_arglist_expr(parser);
    (void) arglist;

    if(! cceat(parser, cctokentype_rparen))
    { ccsynerr(parser, 0, "expected ')', in postfix expression!");
    }
  } else
  if(ccsee(parser, cctokentype_lsquare))
  { cctree_t *expression = ccread_expression(parser);
    (void) expression;
    if(! cceat(parser, cctokentype_rsquare))
    { ccsynerr(parser, 0, "expected ']', in postfix expression!");
    }
  } else
  if(ccsee(parser, cctokentype_mso) ||
     ccsee(parser, cctokentype_msp))
  { lhs = cctree_new_uop(ccgobble(parser), lhs);
  } else
  if(cctok_t *inc = cceat(parser, cctokentype_pos_increment))
  { lhs = cctree_new_uop(inc, lhs);
  } else
  if(cctok_t *dec = cceat(parser, cctokentype_pos_decrement))
  { lhs = cctree_new_uop(dec, lhs);
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
ccread_unary_expr(ccreader_t *parser)
{ cctree_t *result = ktt__nullptr;

  if(ccsee(parser,cctokentype_add))
  { cctok_t *tok=ccgobble(parser);
    if(cceat(parser,cctokentype_add))
    { // TODO(RJ): this is dumb, remove...
      cctok_t clo=*tok;
      clo.sig=cctokentype_pre_increment;
      result=cctree_new_uop(& clo, ccread_cast_expr(parser));
    } else
    { result=cctree_new_uop(tok, ccread_cast_expr(parser));
    }
  } else
  if(ccsee(parser,cctokentype_sub))
  { cctok_t *tok=ccgobble(parser);
    if(cceat(parser,cctokentype_sub))
    { // TODO(RJ): this is dumb, remove...
      cctok_t clo=*tok;
      clo.sig=cctokentype_pre_decrement;
      result=cctree_new_uop(& clo, ccread_cast_expr(parser));
    } else
    { result=cctree_new_uop(tok, ccread_cast_expr(parser));
    }
  } else
  if(ccsee(parser, cctokentype_mul))
  {
    // TODO(RJ): this is dumb, remove...
    cctok_t *tok = ccgobble(parser);
    cctok_t clo = *tok;
    clo.sig = cctokentype_ptr_dereference;

    result = cctree_new_uop(& clo, ccread_cast_expr(parser));
  } else
  if(ccsee(parser, cctokentype_bitwise_invert) ||
     ccsee(parser, cctokentype_negate))
  { result = cctree_new_uop(ccgobble(parser), ccread_cast_expr(parser));
  } else
  { result = ccread_postfix_expr(parser);
  }
  return result;
}
/**
 * cast-expression:
 *   unary-expression
 *   ( type-name ) cast-expression
 **/
ccfunc cctree_t *
ccread_cast_expr(ccreader_t *parser)
{ cctree_t *result = 0;
  if(cctok_t *tok = cceat(parser, cctokentype_lparen))
  { // TODO(RJ):
    result = 0;

    if(! cceat(parser, cctokentype_rparen))
    { ccsynerr(parser, 0, "expected ')'");
    }
  } else
  { result = ccread_unary_expr(parser);
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
ccread_multiplicative_expr(ccreader_t *parser)
{ cctree_t *lhs = ccread_unary_expr(parser);
  while(  ccsee(parser, cctokentype_mul) ||
          ccsee(parser, cctokentype_div) ||
          ccsee(parser, cctokentype_mod) )
  {
    cctok_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_unary_expr(parser);
    lhs = cctree_new_bop(tok, lhs, rhs);
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
ccread_additive_expr(ccreader_t *parser)
{ cctree_t *lhs = ccread_multiplicative_expr(parser);
  while (ccsee(parser, cctokentype_add) ||
         ccsee(parser, cctokentype_sub))
  {
    cctok_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_multiplicative_expr(parser);
    lhs = cctree_new_bop(tok, lhs, rhs);
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
ccread_shift_expr(ccreader_t *parser)
{ cctree_t *lhs = ccread_additive_expr(parser);
  while(ccsee(parser, cctokentype_bitwise_shl) ||
        ccsee(parser, cctokentype_bitwise_shr))
  { cctok_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_additive_expr(parser);
    lhs = cctree_new_bop(tok, lhs, rhs);
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
ccread_relational_expr(ccreader_t *parser)
{ cctree_t *lhs = ccread_shift_expr(parser);
  while ( ccsee(parser, cctokentype_less_than)      ||
          ccsee(parser, cctokentype_less_than_eql)  ||
          ccsee(parser, cctokentype_greater_than)   ||
          ccsee(parser, cctokentype_greater_than_eql) )
  { cctok_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_shift_expr(parser);
    lhs = cctree_new_bop(tok, lhs, rhs);
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
ccread_equality_expr(ccreader_t *parser)
{ cctree_t *lhs = ccread_relational_expr(parser);
  if(ccsee(parser, cctokentype_equals)     ||
     ccsee(parser, cctokentype_not_equals))
  { cctok_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_equality_expr(parser);
    lhs = cctree_new_bop(tok, lhs, rhs);
  }
  return lhs;
}

/**
 * AND-expression:
 *   equality-expression
 *   AND-expression & equality-expression
 **/
ccfunc cctree_t *
ccread_bitwise_and_expr(ccreader_t *parser)
{ cctree_t *lhs = ccread_equality_expr(parser);
  while(ccsee(parser, cctokentype_bitwise_and))
  { cctok_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_equality_expr(parser);
    lhs = cctree_new_bop(tok, lhs, rhs);
  }
  return lhs;
}
/**
 * exclusive-OR-expression:
 *   AND-expression
 *   exclusive-OR-expression ^ AND-expression
 **/
ccfunc cctree_t *
ccread_bitwise_xor_expr(ccreader_t *parser)
{ cctree_t *lhs = ccread_bitwise_and_expr(parser);
  while(ccsee(parser, cctokentype_bitwise_and))
  { cctok_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_bitwise_and_expr(parser);
    lhs = cctree_new_bop(tok, lhs, rhs);
  }
  return lhs;
}
/**
 * inclusive-OR-expression:
 *   exclusive-OR-expression
 *   inclusive-OR-expression | exclusive-OR-expression
 **/
ccfunc cctree_t *
ccread_bitwise_or_expr(ccreader_t *parser)
{ cctree_t *lhs = ccread_bitwise_xor_expr(parser);
  while(ccsee(parser, cctokentype_bitwise_or))
  { cctok_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_bitwise_xor_expr(parser);
    lhs = cctree_new_bop(tok, lhs, rhs);
  }
  return lhs;
}
/**
 * logical-AND-expression:
 *   inclusive-OR-expression
 *   logical-AND-expression && inclusive-OR-expression
 **/
ccfunc cctree_t *
ccread_logical_and_expr(ccreader_t *parser)
{ cctree_t *lhs = ccread_bitwise_or_expr(parser);
  while(ccsee(parser, cctokentype_logical_and))
  { cctok_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_bitwise_or_expr(parser);
    lhs = cctree_new_bop(tok, lhs, rhs);
  }
  return lhs;
}
/**
 * logical-OR-expression:
 *   logical-AND-expression
 *   logical-OR-expression || logical-AND-expression
 **/
ccfunc cctree_t *
ccread_logical_or_expr(ccreader_t *parser)
{ cctree_t *lhs = ccread_logical_and_expr(parser);
  while(ccsee(parser, cctokentype_logical_or))
  { cctok_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_logical_and_expr(parser);
    lhs = cctree_new_bop(tok, lhs, rhs);
  }
  return lhs;
}
/**
 * conditional-expression:
 *   logical-OR-expression
 *   logical-OR-expression ? expression : conditional-expression
 **/
ccfunc cctree_t *
ccread_conditional_expr(ccreader_t *parser)
{ cctree_t *rad = ccread_logical_or_expr(parser);

  if(cctok_t *tok = cceat(parser, cctokentype_conditional))
  {
    cctree_t *lhs = 0, *rhs = 0;
    lhs = ccread_logical_or_expr(parser);

    if(cceat(parser, cctokentype_colon)) // <-- some compilers allow omitting this.
    { rhs = ccread_conditional_expr(parser);
    } else
    { ccsynerr(parser, 0, "expected ':' invalid conditional expression"); // <-- but we issue a syntax error anyways.
    }

    return cctree_new_top(tok,rad,lhs,rhs);
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
ccread_assignment_expr(ccreader_t *parser)
{ cctree_t *lhs = ccread_conditional_expr(parser);
  if(cctok_t *tok = cceat(parser, cctokentype_assign))
  { cctree_t *rhs = ccread_assignment_expr(parser);
    lhs = cctree_new_bop(tok, lhs, rhs);
  }
  return lhs;
}
/**
 * constant-expression:
 *   conditional-expression
 **/
ccfunc cctree_t *
ccread_constant_expression(ccreader_t *parser)
{
  return ccread_conditional_expr(parser);
}
/**
 * expression:
 *   assignment-expression
 *   expression , assignment-expression
 **/
ccfunc cctree_t *
ccread_expression(ccreader_t *parser)
{ cctree_t *result = ccread_assignment_expr(parser);
  if(ccsee(parser, cctokentype_comma))
  { result = cctree_new_bop(ccgobble(parser), result, ccread_expression(parser));
  }
  return result;
}
