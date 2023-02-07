/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
/*****************************************************************/
ccfunc cctree_t *
ccread_arglist_expr(ccread_t *reader);


ccfunc cctree_t *
cctree_paren_expr(cctree_t *init)
{ cctree_t *tree=cctree_new(cctree_kGROUP);
	tree->init=init;
	return tree;
}

ccfunc cctree_t *
cctree_call_expr(cctree_t *lval, cctree_t *rval)
{ cctree_t *tree=cctree_new(cctree_kCALL);
	tree->lval=lval;
	tree->rval=rval;
	return tree;
}


ccfunc cctree_t *
ccread_identifier(ccread_t *parser)
{
	return cctree_new_identifier(cceat(parser,cctoken_Kliteral_identifier));
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
ccread_primary_expr(ccread_t *parser)
{
  if(ccsee(parser, cctoken_Kliteral_identifier))
  { return ccread_identifier(parser);
  } else
  if(ccsee(parser, cctoken_Kliteral_integer))
  { return cctree_new_constant(ctype_int64, ccgobble(parser));
  } else
  if(ccsee(parser, cctoken_Kliteral_float))
  { return cctree_new_constant(ctype_flo64, ccgobble(parser));
  } else
  if(ccsee(parser, cctoken_Kliteral_string))
  { return cctree_new_constant(cctype_new_arr(ctype_int8), ccgobble(parser));
  } else
  if(cceat(parser, cctoken_Klparen))
  { cctree_t *inner=ccread_expression(parser);
    if(!cceat(parser,cctoken_Klparen)) ccsynerr(parser, 0, "expected ')'");
    return cctree_paren_expr(inner);
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
ccfunc cctree_t *
ccread_postfix_expr(ccread_t *parser)
{
  cctree_t *lhs = ccread_primary_expr(parser);

  if(cceat(parser, cctoken_Klparen))
  { cctree_t *args = ccread_arglist_expr(parser);
    if(!cceat(parser,cctoken_Krparen)) ccsynerr(parser, 0, "expected ')'");
    return cctree_call_expr(lhs,args);
  } else
  if(ccsee(parser, cctoken_Klsquare))
  { cctree_t *expression = ccread_expression(parser);
    (void) expression;
    if(! cceat(parser, cctoken_Krsquare))
    { ccsynerr(parser, 0, "expected ']', in postfix expression!");
    }
  } else
  if(ccsee(parser, cctoken_Kmso) ||
     ccsee(parser, cctoken_Kmsp))
  { lhs = cctree_new_uop(ccgobble(parser), lhs);
  } else
  if(cctoken_t *inc = cceat(parser, cctoken_Kpos_increment))
  { lhs = cctree_new_uop(inc, lhs);
  } else
  if(cctoken_t *dec = cceat(parser, cctoken_Kpos_decrement))
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
ccread_unary_expr(ccread_t *parser)
{ cctree_t *result = ccnil;

  if(ccsee(parser,cctoken_Kadd))
  { cctoken_t *tok=ccgobble(parser);
    if(cceat(parser,cctoken_Kadd))
    { // TODO(RJ): this is dumb, remove...
      cctoken_t clo=*tok;
      clo.sig=cctoken_Kpre_increment;
      result=cctree_new_uop(& clo, ccread_cast_expr(parser));
    } else
    { result=cctree_new_uop(tok, ccread_cast_expr(parser));
    }
  } else
  if(ccsee(parser,cctoken_Ksub))
  { cctoken_t *tok=ccgobble(parser);
    if(cceat(parser,cctoken_Ksub))
    { // TODO(RJ): this is dumb, remove...
      cctoken_t clo=*tok;
      clo.sig=cctoken_Kpre_decrement;
      result=cctree_new_uop(& clo, ccread_cast_expr(parser));
    } else
    { result=cctree_new_uop(tok, ccread_cast_expr(parser));
    }
  } else
  if(ccsee(parser, cctoken_Kmul))
  {
    // TODO(RJ): this is dumb, remove...
    cctoken_t *tok = ccgobble(parser);
    cctoken_t clo = *tok;
    clo.sig = cctoken_Kptr_dereference;

    result = cctree_new_uop(& clo, ccread_cast_expr(parser));
  } else
  if(ccsee(parser, cctoken_Kbitwise_invert) ||
     ccsee(parser, cctoken_Knegate))
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
ccread_cast_expr(ccread_t *parser)
{ cctree_t *result = 0;
  if(cctoken_t *tok = cceat(parser, cctoken_Klparen))
  { // TODO(RJ):
    result = 0;

    if(! cceat(parser, cctoken_Krparen))
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
ccread_multiplicative_expr(ccread_t *parser)
{ cctree_t *lhs = ccread_unary_expr(parser);
  while(  ccsee(parser, cctoken_Kmul) ||
          ccsee(parser, cctoken_Kdiv) ||
          ccsee(parser, cctoken_Kmod) )
  {
    cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_unary_expr(parser);
    lhs = cctree_binary(tok, lhs, rhs);
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
ccread_additive_expr(ccread_t *parser)
{ cctree_t *lhs = ccread_multiplicative_expr(parser);
  while (ccsee(parser, cctoken_Kadd) ||
         ccsee(parser, cctoken_Ksub))
  {
    cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_multiplicative_expr(parser);
    lhs = cctree_binary(tok, lhs, rhs);
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
ccread_shift_expr(ccread_t *parser)
{ cctree_t *lhs = ccread_additive_expr(parser);
  while(ccsee(parser, cctoken_Kbitwise_shl) ||
        ccsee(parser, cctoken_Kbitwise_shr))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_additive_expr(parser);
    lhs = cctree_binary(tok, lhs, rhs);
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
ccread_relational_expr(ccread_t *parser)
{ cctree_t *lhs = ccread_shift_expr(parser);
  while ( ccsee(parser, cctoken_Kless_than)      ||
          ccsee(parser, cctoken_Kless_than_eql)  ||
          ccsee(parser, cctoken_Kgreater_than)   ||
          ccsee(parser, cctoken_Kgreater_than_eql) )
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_shift_expr(parser);
    lhs = cctree_binary(tok, lhs, rhs);
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
ccread_equality_expr(ccread_t *parser)
{ cctree_t *lhs = ccread_relational_expr(parser);
  if(ccsee(parser, cctoken_Kequals)     ||
     ccsee(parser, cctoken_Knot_equals))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_equality_expr(parser);
    lhs = cctree_binary(tok, lhs, rhs);
  }
  return lhs;
}

/**
 * AND-expression:
 *   equality-expression
 *   AND-expression & equality-expression
 **/
ccfunc cctree_t *
ccread_bitwise_and_expr(ccread_t *parser)
{ cctree_t *lhs = ccread_equality_expr(parser);
  while(ccsee(parser, cctoken_Kbitwise_and))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_equality_expr(parser);
    lhs = cctree_binary(tok, lhs, rhs);
  }
  return lhs;
}
/**
 * exclusive-OR-expression:
 *   AND-expression
 *   exclusive-OR-expression ^ AND-expression
 **/
ccfunc cctree_t *
ccread_bitwise_xor_expr(ccread_t *parser)
{ cctree_t *lhs = ccread_bitwise_and_expr(parser);
  while(ccsee(parser, cctoken_Kbitwise_and))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_bitwise_and_expr(parser);
    lhs = cctree_binary(tok, lhs, rhs);
  }
  return lhs;
}
/**
 * inclusive-OR-expression:
 *   exclusive-OR-expression
 *   inclusive-OR-expression | exclusive-OR-expression
 **/
ccfunc cctree_t *
ccread_bitwise_or_expr(ccread_t *parser)
{ cctree_t *lhs = ccread_bitwise_xor_expr(parser);
  while(ccsee(parser, cctoken_Kbitwise_or))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_bitwise_xor_expr(parser);
    lhs = cctree_binary(tok, lhs, rhs);
  }
  return lhs;
}
/**
 * logical-AND-expression:
 *   inclusive-OR-expression
 *   logical-AND-expression && inclusive-OR-expression
 **/
ccfunc cctree_t *
ccread_logical_and_expr(ccread_t *parser)
{ cctree_t *lhs = ccread_bitwise_or_expr(parser);
  while(ccsee(parser, cctoken_Klogical_and))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_bitwise_or_expr(parser);
    lhs = cctree_binary(tok, lhs, rhs);
  }
  return lhs;
}
/**
 * logical-OR-expression:
 *   logical-AND-expression
 *   logical-OR-expression || logical-AND-expression
 **/
ccfunc cctree_t *
ccread_logical_or_expr(ccread_t *parser)
{ cctree_t *lhs = ccread_logical_and_expr(parser);
  while(ccsee(parser, cctoken_Klogical_or))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_logical_and_expr(parser);
    lhs = cctree_binary(tok, lhs, rhs);
  }
  return lhs;
}
/**
 * conditional-expression:
 *   logical-OR-expression
 *   logical-OR-expression ? expression : conditional-expression
 **/
ccfunc cctree_t *
ccread_conditional_expr(ccread_t *parser)
{ cctree_t *rad = ccread_logical_or_expr(parser);

  if(cctoken_t *tok = cceat(parser, cctoken_Kconditional))
  {
    cctree_t *lhs = 0, *rhs = 0;
    lhs = ccread_logical_or_expr(parser);

    if(cceat(parser, cctoken_Kcolon)) // <-- some compilers allow omitting this.
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
ccread_assignment_expr(ccread_t *parser)
{ cctree_t *lhs = ccread_conditional_expr(parser);
  if(cctoken_t *tok = cceat(parser, cctoken_kASSIGN))
  { cctree_t *rhs = ccread_assignment_expr(parser);
    lhs = cctree_binary(tok, lhs, rhs);
  }
  return lhs;
}
/**
 * constant-expression:
 *   conditional-expression
 **/
ccfunc cctree_t *
ccread_constant_expression(ccread_t *parser)
{
  return ccread_conditional_expr(parser);
}
/**
 * expression:
 *   assignment-expression
 *   expression , assignment-expression
 **/
ccfunc cctree_t *
ccread_expression(ccread_t *parser)
{ cctree_t *result = ccread_assignment_expr(parser);
  if(ccsee(parser, cctoken_Kcomma))
  { result = cctree_binary(ccgobble(parser), result, ccread_expression(parser));
  }
  return result;
}
/**
 * argument-expression-list:
 *   assignment-expression
 *   argument-expression-list , assignment-expression
 **/
ccfunc cctree_t *
ccread_arglist_expr(ccread_t *reader) // <-- TODO(RJ): return list instead! like other functions!
{ cctree_t *list=ccnil,*next;
  do
  { next=ccread_assignment_expr(reader);
    if(next) *ccarradd(list,1)=*next;
    cctree_del(next);
  } while(next!=0&&cceat(reader,cctoken_Kcomma));
  return list;
}
