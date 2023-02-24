// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCREAD_C
#define _CCREAD_C

ccfunc void ccread_hash_init(ccread_t *reader);

ccfunc ccinle void
ccread_init(ccread_t *reader)
{
  memset(reader,ccnull,sizeof(*reader));

  ccread_hash_init(reader);
}

ccfunc ccinle void
ccread_uninit(ccread_t *reader)
{
  ccarrdel(reader->buf);
  ccarrdel(reader->tok_tbl);
}

ccfunc ccinle void
ccread_all_tokens(ccread_t *_r);

ccfunc void
ccread_include(ccread_t *reader, const char *name)
{
  ccu32_t size=0;
  void *file=ccopenfile(name,"r");
  char *data=ccpullfile(file,0,&size);
  ccclosefile(file);

  reader->doc_max=data+size;
  reader->doc_min=reader->tok_min=reader->tok_max=data;

  ccread_all_tokens(reader);

  reader->min=reader->buf;
  reader->max=reader->buf+ccarrlen(reader->buf);

  // Todo:
  // ccfree(data);
}

// Todo:
ccfunc void
ccread_all_tokens(ccread_t *reader)
{ while(ccread_next_token(reader))
    ccread_token(reader,ccarradd(reader->buf,1));
}

ccfunc ccinle int
ccread_continues(ccread_t *reader)
{
  int e=reader->term_expl;
  return !e;
}

ccfunc cctoken_t *
ccpeek(ccread_t *reader, cci32_t offset)
{
  if(reader->min+offset<reader->max)
    return reader->min+offset;

  // Todo:
  static cctoken_t end_tok = { cctoken_kEND };
  return & end_tok;
}

ccfunc ccinle cctoken_t *
ccpeep(ccread_t *reader)
{
  return ccpeek(reader,0);
}

ccfunc ccinle int
ccsee(ccread_t *reader, cctoken_k kind)
{
  return ccpeep(reader)->bit==kind;
}

ccfunc ccinle int
ccsee_end(ccread_t *reader)
{
  return ccsee(reader,cctoken_kEND);
}

ccfunc ccinle cctoken_t *
ccgobble(ccread_t *reader)
{
  if(reader->min<reader->max)
  {
    reader->term_expl=reader->min->term_expl;
    reader->term_impl=reader->min->term_impl;

    return reader->min++;
  }

  return ccpeep(reader);
}

ccfunc ccinle cctoken_t *
cceat(ccread_t *reader, cctoken_k kind)
{
  if(ccsee(reader,kind))
    return ccgobble(reader);

  return ccnull;
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
kttc__peek_storage_class(ccread_t *parser)
{
  cctoken_t *token = ccpeep(parser);

  // if(token->bit > kttc__scls_spec_0 &&
 // token->bit < kttc__scls_spec_1)
  // {
  // return token;
  // }
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
ccread_arglist(ccread_t *reader, cctree_t *root, cci32_t mark);
ccfunc cctree_t *
ccread_expression(ccread_t *parser, cctree_t *root, cci32_t mark);
ccfunc cctree_t *
ccread_cast(ccread_t *parser, cctree_t *root, cci32_t mark);

ccfunc ccinle cctree_t *
ccread_litide(ccread_t *reader, cctree_t *root, cci32_t mark)
{ cctoken_t *token=cceat(reader,cctoken_kLITIDENT);
  cctree_t *tree=ccnil;

  if(token) tree=cctree_litide(root,mark,token->loc,token->str);

  return tree;
}

// Section: EXPRESSIONS
// https://learn.microsoft.com/en-us/cpp/c-language/summary-of-expressions
ccfunc cctree_t *
ccread_primary(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccnil;
  cctoken_t *token=ccpeep(reader);
  if(token)
  { switch(token->bit)
    { case cctoken_kLITIDENT:
        result=cctree_litide(root,mark,token->loc,token->str);
        ccgobble(reader);
      break;
      case cctoken_kLITINT:
        result=cctree_litint(root,mark,ccgobble(reader));
      break;
      case cctoken_kLITFLO:
        result=cctree_litflo(root,mark,ccgobble(reader));
      break;
      case cctoken_kLITSTR:
        result=cctree_litstr(root,mark,ccgobble(reader));
      break;
      case cctoken_kLPAREN:
        cctree_t *group=ccread_expression(reader,root,mark);
        if(!cceat(reader,cctoken_kRPAREN))
          ccsynerr(reader,0,"expected ')'");
        result=cctree_group(root,mark,group);
      break;
    }
  }
  return result;
}
//
// <postfix-expression> ::=
//   <primary-expression>
//   <postfix-expression> [ <expression> ]
//   <postfix-expression> ( <argument-expression-listopt> )
//   <postfix-expression> . <identifier>
//   <postfix-expression> -> <identifier>
//   <postfix-expression> ++
//   <postfix-expression> --
//
// Todo: remove the 'arrow token', we have to figure it out here ...
ccfunc cctree_t *
ccread_postfix_suffix_maybe(ccread_t *reader, cctree_t *lhs, cctree_t *root, cci32_t mark)
{
  cctree_t *result=lhs;

  if(cceat(reader,cctoken_kLPAREN))
  {
    cctree_t *args=ccread_arglist(reader,root,mark);

    if(!cceat(reader,cctoken_kRPAREN))
      ccsynerr(reader, 0, "expected ')'");

    result=cctree_call(root,mark,lhs,args,lhs->name);

    if(ccread_continues(reader))
    {
      result=ccread_postfix_suffix_maybe(reader,result,root,mark);
    }
  } else
  if(cceat(reader,cctoken_kLSQUARE))
  {
    cctree_t *args=ccread_expression(reader,root,mark);
    if(!cceat(reader,cctoken_kRSQUARE))
      ccsynerr(reader, 0, "expected ']'");

    result=cctree_index(root,mark,lhs,args,lhs->name);

    if(ccread_continues(reader))
    {
      result=ccread_postfix_suffix_maybe(reader,result,root,mark);
    }
  } else
  if(ccsee(reader,cctoken_kDOT))
  { result=cctree_unary(root,mark,cctoken_kDOT,lhs);

    if(ccread_continues(reader))
    {
      result=ccread_postfix_suffix_maybe(reader,result,root,mark);
    }
  }

  // Todo: ++ and --
  return result;
}

ccfunc cctree_t *
ccread_postfix(ccread_t *reader, cctree_t *root, cci32_t mark)
{
ccdbenter("postfix");
  cctree_t *identifier=ccread_primary(reader,root,mark);
  cctree_t *result=ccread_postfix_suffix_maybe(reader,identifier,root,mark);
ccdbleave("postfix");
  return result;
}

ccfunc cctree_t *
ccread_unary(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result;

  // Todo: this is flawed!
  if(cceat(reader,cctoken_kBWAND))
  {
    result=cctree_unary(root,mark,cctoken_kADDRESSOF,ccread_cast(reader,root,mark));
  } else
  if(cceat(reader,cctoken_kADD))
  {
    if(cceat(reader,cctoken_kADD))
      result=cctree_unary(root,mark,cctoken_kPIN,ccread_cast(reader,root,mark));
    else
      result=cctree_unary(root,mark,cctoken_kADD,ccread_cast(reader,root,mark));
  } else
  if(cceat(reader,cctoken_kSUB))
  {
    if(cceat(reader,cctoken_kSUB))
      result=cctree_unary(root,mark,cctoken_kPDE,ccread_cast(reader,root,mark));
    else
      result=cctree_unary(root,mark,cctoken_kSUB,ccread_cast(reader,root,mark));
  } else
  if(cceat(reader, cctoken_kMUL))
  {
    result=cctree_unary(root,mark,cctoken_kDEREFERENCE,ccread_cast(reader,root,mark));
  } else
  {
    result=ccread_postfix(reader,root,mark);
  }

  return result;
}

ccfunc cctree_t *
ccread_cast(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result;

  if(cceat(reader,cctoken_kLPAREN))
  { // Todo:
    result=ccnull;

    if(!cceat(reader,cctoken_kRPAREN))
      ccsynerr(reader,0,"expected ')'");
  } else
    result=ccread_unary(reader,root,mark);

  return result;
}

ccfunc cctree_t *
ccread_multiplicative(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_unary(reader,root,mark);

  if(ccread_continues(reader))
  {
    while(ccsee(reader,cctoken_kMUL) ||
          ccsee(reader,cctoken_kDIV) ||
          ccsee(reader,cctoken_kMOD) )
    {
      cctoken_t *token=ccgobble(reader);
      result=cctree_binary(root,mark,token,result,ccread_unary(reader,root,mark));
    }
  }
  return result;
}

ccfunc cctree_t *
ccread_additive(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_multiplicative(reader,root,mark);

  if(ccread_continues(reader))
  {
    while (ccsee(reader,cctoken_kADD) ||
           ccsee(reader,cctoken_kSUB))
    {
      cctoken_t *token=ccgobble(reader);
      result=cctree_binary(root,mark,token,result,ccread_multiplicative(reader,root,mark));
    }
  }
  return result;
}

ccfunc cctree_t *
ccread_shift(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_additive(reader,root,mark);

  if(ccread_continues(reader))
  {
    while(ccsee(reader,cctoken_kBWSHL) ||
          ccsee(reader,cctoken_kBWSHR))
    {
      cctoken_t *token=ccgobble(reader);
      result=cctree_binary(root,mark,token,result,ccread_additive(reader,root,mark));
    }
  }
  return result;

}

ccfunc cctree_t *
ccread_relational(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_shift(reader,root,mark);

  if(ccread_continues(reader))
  {
    while(ccsee(reader,cctoken_kLTN) ||
          ccsee(reader,cctoken_kLTE) ||
          ccsee(reader,cctoken_kGTN) ||
          ccsee(reader,cctoken_kGTE) )
    {
      cctoken_t *token=ccgobble(reader);
      result=cctree_binary(root,mark,token,result,ccread_shift(reader,root,mark));
    }
  }

  return result;
}

ccfunc cctree_t *
ccread_equality(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_relational(reader,root,mark);

  if(ccread_continues(reader))
  {
    if(ccsee(reader,cctoken_kTEQ) ||
       ccsee(reader,cctoken_kFEQ))
    {
      cctoken_t *token=ccgobble(reader);
      result=cctree_binary(root,mark,token,result,ccread_equality(reader,root,mark));
    }
  }

  return result;
}

ccfunc cctree_t *
ccread_bitwise_and(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_equality(reader,root,mark);

  if(ccread_continues(reader))
  {
    while(ccsee(reader,cctoken_kBWAND))
    { cctoken_t *token=ccgobble(reader);
      result=cctree_binary(root,mark,token,result,ccread_equality(reader,root,mark));
    }
  }

  return result;
}
/**
 * exclusive-OR-expression:
 *   AND-expression
 *   exclusive-OR-expression ^ AND-expression
 **/
ccfunc cctree_t *
ccread_bitwise_xor(ccread_t *parser, cctree_t *root, cci32_t mark)
{
ccdbenter("bitwise_xor_expr");
  cctree_t *lhs = ccread_bitwise_and(parser,root,mark);
  while(ccsee(parser, cctoken_kBWAND))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_bitwise_and(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
ccdbleave("bitwise_xor_expr");
  return lhs;
}
/**
 * inclusive-OR-expression:
 *   exclusive-OR-expression
 *   inclusive-OR-expression | exclusive-OR-expression
 **/
ccfunc cctree_t *
ccread_bitwise_or(ccread_t *parser, cctree_t *root, cci32_t mark)
{
ccdbenter("bitwise_or_expr");
  cctree_t *lhs = ccread_bitwise_xor(parser,root,mark);
  while(ccsee(parser, cctoken_kBWOR))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_bitwise_xor(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
ccdbleave("bitwise_or_expr");
  return lhs;
}
/**
 * logical-AND-expression:
 *   inclusive-OR-expression
 *   logical-AND-expression && inclusive-OR-expression
 **/
ccfunc cctree_t *
ccread_logical_and(ccread_t *parser, cctree_t *root, cci32_t mark)
{
ccdbenter("logical_and_expr");
  cctree_t *lhs = ccread_bitwise_or(parser,root,mark);
  while(ccsee(parser, cctoken_kLGAND))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_bitwise_or(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
ccdbleave("logical_and_expr");
  return lhs;
}
/**
 * logical-OR-expression:
 *   logical-AND-expression
 *   logical-OR-expression || logical-AND-expression
 **/
ccfunc cctree_t *
ccread_logical_or(ccread_t *parser, cctree_t *root, cci32_t mark)
{
ccdbenter("logical_or_expr");
  cctree_t *lhs = ccread_logical_and(parser,root,mark);
  while(ccsee(parser, cctoken_kLGOR))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_logical_and(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
ccdbleave("logical_or_expr");
  return lhs;
}
/**
 * conditional-expression:
 *   logical-OR-expression
 *   logical-OR-expression ? expression : conditional-expression
 **/
ccfunc cctree_t *
ccread_conditional(ccread_t *parser, cctree_t *root, cci32_t mark)
{
ccdbenter("conditional");
  cctree_t *result=ccread_logical_or(parser,root,mark);

  if(cceat(parser, cctoken_kQMRK))
  { cctree_t *lhs=ccnil,*rhs=ccnil;
    lhs=ccread_logical_or(parser,root,mark);
    if(cceat(parser, cctoken_Kcolon)) // Note: some compiler allow omitting this ...
      rhs=ccread_conditional(parser,root,mark);
    else
      ccsynerr(parser,0,"expected ':' invalid conditional expression");
    result=cctree_ternary(root,mark,result,lhs,rhs);
  }
ccdbleave("conditional");
  return result;
}
/**
 * assignment-expression:
 *   conditional-expression
 *   unary-expression assignment-operator assignment-expression
 *
 * assignment-operator: one of
 *   = *= /= %= += -= <<= >>= &= ^= |=
 **/
// Todo: ensure ltree is unary, which could never be the case ...
ccfunc cctree_t *
ccread_assignment(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_conditional(reader,root,mark);

  if(!reader->term_expl)
  { cctoken_t *token=ccpeep(reader);
    if(cctoken_is_assignment(token->bit))
    { ccgobble(reader);
      result=cctree_binary(root,mark,token,result,ccread_assignment(reader,root,mark));
    }
  }

  return result;
}
/**
 * constant-expression:
 *   conditional-expression
 **/
ccfunc ccinle cctree_t *
ccread_constant_expression(ccread_t *reader, cctree_t *root, cci32_t mark)
{
ccdbenter("constant_expression");
  cctree_t *result=ccread_conditional(reader,root,mark);
ccdbleave("constant_expression");
  return result;
}

/**
 * expression:
 *   assignment-expression
 *   expression , assignment-expression
 **/
ccfunc cctree_t *
ccread_expression(ccread_t *reader, cctree_t *root, cci32_t mark)
{
ccdbenter("expression");

  cctree_t *result=ccread_assignment(reader,root,mark);

  cctoken_t *token=cceat(reader,cctoken_kCMA);

  if(token)
    result=cctree_binary(root,mark,token,result,ccread_expression(reader,root,mark));

ccdbleave("expression");
  return result;
}

/**
 * argument-expression-list:
 *   assignment-expression
 *   argument-expression-list , assignment-expression
 **/
ccfunc cctree_t *
ccread_arglist(ccread_t *reader, cctree_t *root, cci32_t mark)
{ cctree_t *list=ccnil,*next;

  do
  { next=ccread_assignment(reader,root,mark);
    if(!next) break;

    *ccarradd(list,1)=*next;
    cctree_del(next);

  } while(cceat(reader,cctoken_kCMA));

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
    cctree_t  *cex = ccread_litide(reader,root,mark);
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

  if(cceat(reader,cctoken_kCMA))
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
    return ccread_assignment(reader,root,mark);
  }
#endif
  return ccread_assignment(reader,root,mark);
}

ccfunc cctree_t *
ccread_direct_decl_name_modifier(ccread_t *reader, cctree_t *root, cci32_t mark, cctree_t *type)
{ // Note: document this ...

  if(cceat(reader, cctoken_kLPAREN))
  {
    // Note: remove the external flag
    cctree_t **list=ccread_param_type_list(reader,root,mark&~cctree_mEXTERNAL);

    if(!cceat(reader, cctoken_kRPAREN)) ccsynerr(reader, 0, "expected ')'");
    cctree_t *modifier=ccread_direct_decl_name_modifier(reader,root,mark,type);
    if(modifier->kind==cctree_kFUNCTION) ccsynwar(reader,0,"function that returns function");
    if(modifier->kind==cctree_kARRAY) ccsynwar(reader,0,"function that returns array");
    return cctree_function_modifier(modifier,list);
  } else
  if(cceat(reader, cctoken_kLSQUARE))
  {
    cctree_t *rval=ccread_expression(reader,root,mark);

    if(!cceat(reader,cctoken_kRSQUARE))
      ccsynerr(reader,0,"expected ']'");

    cctree_t *modi=ccread_direct_decl_name_modifier(reader,root,mark,type);
    return cctree_array_modifier(modi,rval);
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
    if(!cceat(reader, cctoken_kRPAREN))
      ccsynerr(reader, 0, "expected ')'");
    // Todo: check if we the expression is terminated, in which case don't do this ...
    tmp=ccread_direct_decl_name_modifier(reader,root,mark,type);
    *mod=*tmp;
    cctree_del(tmp);
    return res;
  } else
  if(ccsee(reader, cctoken_kLITIDENT))
  { cctree_t *name=ccread_litide(reader,root,mark);
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
    return cctree_pointer_modifier(ccread_decl_name_modifier_maybe(reader,root,mark,type));
  return type;
}

ccfunc ccinle cctree_t *
ccread_decl_name(ccread_t *reader, cctree_t *root, cci32_t mark, cctree_t *type)
{ ccassert(type!=0);
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
{ ccassert(root!=0);
  cctree_t *next,**list=ccnil;
  do
  { next=ccread_init_decl_name(reader,root,mark,type);
    if(!next) break;
    *ccarradd(list,1)=next;
  } while(cceat(reader,cctoken_kCMA));
  return list;
}

ccfunc cctree_t **
ccread_struct_decl_name_list(ccread_t *reader, cctree_t *root, cci32_t mark, cctree_t *type)
{ cctree_t *next,**list=ccnil;
  do
  { next=ccread_struct_decl_name(reader,root,mark,type);
    if(next) *ccarradd(list,1)=next;
  } while(next!=0&&cceat(reader,cctoken_kCMA));
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
{ if(cceat(reader,cctoken_kSTRUCT))
  {
    cctree_t *name=ccread_litide(reader,root,mark);

    if(!cceat(reader, cctoken_Klcurly))
      ccsynerr(reader,0,"expected '{' for struct specifier");

    // Todo: semicolon handling, proper root ...
    cctree_t *next,**list=ccnil;
    while(next=ccread_struct_decl(reader,root,mark))
    { *ccarradd(list,1)=next;
      if(!reader->term_expl) ccsynerr(reader,0,"expected ';'");
    }

    if(!cceat(reader, cctoken_Krcurly))
      ccsynerr(reader,0,"expected '}' for struct specifier");

    cctree_t *result=cctree_new(cctree_kSTRUCT,root,mark);
    result->list=list;
    result->name=cctree_name(name);

    return result;
  } else
  if(cceat(reader, cctoken_kENUM))
  {
    ccassert(!"noimpl");
  }
  return 0;
}

// Todo: properly parse this ...
ccfunc cctree_t *
ccread_type_specifier(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctoken_t *token=ccpeep(reader);

  cctree_t *result=ccnull;

  switch(token->bit)
  { case cctoken_kENUM:
    case cctoken_kSTRUCT:
      result=ccread_struct_or_union_specifier(reader,root,mark);
    break;
    case cctoken_kVOID:
    case cctoken_kMSVC_INT8:
    case cctoken_kMSVC_INT16:
    case cctoken_kMSVC_INT32:
    case cctoken_kMSVC_INT64:
    case cctoken_kSTDC_BOOL:
    case cctoken_kSTDC_CHAR:
    case cctoken_kSTDC_SHORT:
    case cctoken_kSTDC_INT:
    case cctoken_kSTDC_FLOAT:
    case cctoken_kSTDC_DOUBLE:
      ccgobble(reader);
      result=cctree_new(cctree_kSPECIFIER,root,mark);
      result->sort=token->bit;
      result->loca=token->loc;
    break;
  }
  return result;
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
// Todo: remove this...
ccfunc cci32_t
ccread_storage_class_specifier(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  if(kttc__peek_storage_class(reader))
  {
    cctoken_t *tok = ccgobble(reader);
    (void) tok;

    return cctrue;
  }
  return ccfalse;
}
//
// type-qualifier:
//   const
//   restrict
//   volatile
//   _Atomic
//
// Todo: remove this...
ccfunc cci32_t
ccread_type_qualifier(ccread_t *reader, cctree_t *root, cci32_t mark)
{ if(kttc__peek_type_qualifier(reader))
  { cctoken_t *tok = ccgobble(reader);
    (void) tok;
    return cctrue;
  }
  return ccfalse;
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
    return cctrue;
  }
  return ccfalse;
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
    return cctrue;
  }
  return ccfalse;
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
  return ccfalse;
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
    if(!cceat(reader,cctoken_kCMA)) break;
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
  { if(!reader->term_expl) ccsynerr(reader, 0, "expected ';'");
    return child;
  } else
  if(cceat(reader,cctoken_Kreturn))
  {
    cctree_t *expr_tree=ccread_expression(reader,root,mark);
    child=cctree_return(root,mark,expr_tree);

    if(!expr_tree) ccsynerr(reader,0,"expected expression");

    if(!reader->term_expl) ccsynerr(reader, 0, "expected ';'");
  } else
  if(cceat(reader,cctoken_Kwhile))
  {
    cctree_t *cond_tree=ccnil,*then_tree=ccnil;

    if(!cceat(reader,cctoken_kLPAREN)) ccsynerr(reader,0,"expected '('");
    cond_tree=ccread_expression(reader,root,mark);
    if(!cceat(reader,cctoken_kRPAREN)) ccsynerr(reader,0,"expected ')'");

    if(!cond_tree) ccsynerr(reader,0,"expected expression");

    if(!reader->term_expl)
    {
      then_tree=ccread_block_or_single_stmt(reader,root,mark);
      if(!then_tree) ccsynerr(reader,0,"expected statement");
    }
    child=cctree_while(root,mark,cond_tree,then_tree);
  } else
  if(cceat(reader,cctoken_Kgoto))
  {
    cctree_t *ident=ccread_litide(reader,root,mark);
    if(!ident) ccsynerr(reader,0,"missing goto label identifier");

    child=cctree_goto(root,mark,ident);

    if(!reader->term_expl) ccsynerr(reader, 0, "expected ';'");
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

    if(!reader->term_expl)
    { then_tree=ccread_block_or_single_stmt(reader,root,mark);
      if(!then_tree) ccsynerr(reader,0,"expected statement");
    }
    if(cceat(reader,cctoken_Kelse)&&!reader->term_expl)
    { else_tree=ccread_block_or_single_stmt(reader,root,mark);
      if(!else_tree) ccsynerr(reader,0,"expected statement");
    }

    child=cctree_ternary(root,mark,cond_tree,then_tree,else_tree);

  } else
  // Note: make sure you call this last, simply because this is a rather expensive function ... not that I care ...
  if(child=ccread_expression(reader,root,mark))
  {
    // Todo: better way of doing this?
    if(!reader->term_expl)
    {
      if(child->kind==cctree_kLITIDE)
      {
        if(cceat(reader,cctoken_Kcolon))
        {

          cctree_t **list=ccnil;
          if(!reader->term_expl)
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

  cctree_t *next;
  while(next=ccread_external_declaration(reader,tree,0))
    *ccarradd(tree->list,1)=next;

  return tree;
}

ccfunc cctree_t *
ccread_external_declaration(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *decl=ccread_init_decl(reader,root,mark|cctree_mEXTERNAL);

  if(!decl) return ccnil;

  ccassert(decl->kind==cctree_kDECL);
  ccassert(decl->list!=0);

  cctree_t *name=*decl->list;

  ccassert(name->kind==cctree_kDECLNAME);
  ccassert(name->type!=0);
  ccassert(name->name!=0);

  if(name->type->kind==cctree_kFUNCTION)
  {
    // Note: You can't define multiple functions within the same declaration ...
    ccassert(ccarrlen(decl->list)==1);

    name->blob=ccread_block(reader,name,mark);

    if(!name->blob&&!reader->term_expl) ccsynerr(reader,0,"expected ';'");
  } else
  {
    // Note: applies to every declaration other than a function declaration with its definition ...
    if(!reader->term_expl) ccsynerr(reader,0,"expected ';'");
  }

  if(!decl&&!ccsee_end(reader)) ccsynerr(reader,0,"invalid external declaration");
  return decl;
}





// Todo: this is not good ...
ccfunc void
ccread_hash_init(ccread_t *reader)
{
  *cctblputL(reader->tok_tbl,"__asm")=cctoken_Kmsvc_attr_asm; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"__based")=cctoken_Kmsvc_attr_based; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"__cdecl")=cctoken_Kmsvc_attr_cdecl; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"__clrcall")=cctoken_Kmsvc_attr_clrcall; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"__fastcall")=cctoken_Kmsvc_attr_fastcall; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"__inline")=cctoken_Kmsvc_attr_inline; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"__stdcall")=cctoken_Kmsvc_attr_stdcall; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"__thiscall")=cctoken_Kmsvc_attr_thiscall; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"__vectorcal")=cctoken_Kmsvc_attr_vectorcal; ccassert(ccerrnon());

  *cctblputL(reader->tok_tbl,"_Alignof")=cctoken_Kalign_of; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"_Alignas")=cctoken_Kalign_as; ccassert(ccerrnon());

  *cctblputL(reader->tok_tbl,"const")=cctoken_Kconst; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"restrict")=cctoken_Krestrict; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"volatile")=cctoken_Kvolatile; ccassert(ccerrnon());

  *cctblputL(reader->tok_tbl,"inline")=cctoken_Kinline; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"_Noreturn")=cctoken_Kno_return; ccassert(ccerrnon());

  *cctblputL(reader->tok_tbl,"signed")=cctoken_kSTDC_SIGNED; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"unsigned")=cctoken_kSTDC_UNSIGNED; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"__int8")=cctoken_kMSVC_INT8; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"__int16")=cctoken_kMSVC_INT16; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"__int32")=cctoken_kMSVC_INT32; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"__int64")=cctoken_kMSVC_INT64; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"double")=cctoken_kSTDC_DOUBLE; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"float")=cctoken_kSTDC_FLOAT; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"long")=cctoken_kSTDC_LONG; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"int")=cctoken_kSTDC_INT; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"short")=cctoken_kSTDC_SHORT; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"char")=cctoken_kSTDC_CHAR; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"void")=cctoken_kVOID; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"_Bool")=cctoken_kSTDC_BOOL; ccassert(ccerrnon());

  // *cctblputL(reader->tok_tbl,"_Complex")=cctoken_Kcomplex; ccassert(ccerrnon());
  // *cctblputL(reader->tok_tbl,"_Atomic")=cctoken_Katomic; ccassert(ccerrnon());

  *cctblputL(reader->tok_tbl,"enum")=cctoken_kENUM; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"struct")=cctoken_kSTRUCT; ccassert(ccerrnon());

  *cctblputL(reader->tok_tbl,"typedef")=cctoken_Ktypedef; ccassert(ccerrnon());

  *cctblputL(reader->tok_tbl,"auto")=cctoken_Kauto; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"extern")=cctoken_Kextern; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"register")=cctoken_Kregister; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"static")=cctoken_Kstatic; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"_Thread_local")=cctoken_Kthread_local; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"__declspec")=cctoken_Kmsvc_declspec; ccassert(ccerrnon());

  *cctblputL(reader->tok_tbl,"if")=cctoken_Kif; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"switch")=cctoken_Kswitch; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"else")=cctoken_Kelse; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"case")=cctoken_Kcase; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"default")=cctoken_Kdefault; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"for")=cctoken_Kfor; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"while")=cctoken_Kwhile; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"do")=cctoken_Kdo; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"goto")=cctoken_Kgoto; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"return")=cctoken_Kreturn; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"break")=cctoken_Kbreak; ccassert(ccerrnon());
  *cctblputL(reader->tok_tbl,"continue")=cctoken_Kcontinue; ccassert(ccerrnon());


  // Todo:

  // *cctblputL(reader->tok_tbl,"ccassert")=cctoken_kCCASSERT; ccassert(ccerrnon());
  // *cctblputL(reader->tok_tbl,"ccbreak") =cctoken_kCCBREAK;  ccassert(ccerrnon());
  // *cctblputL(reader->tok_tbl,"ccerror") =cctoken_kCCERROR;  ccassert(ccerrnon());

}



#endif