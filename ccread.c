// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCREAD_C
#define _CCREAD_C

ccfunc void ccread_register_defaults(ccread_t *reader);

ccfunc ccinle void
ccread_init(ccread_t *reader)
{
  memset(reader,ccnull,sizeof(*reader));

  ccread_register_defaults(reader);
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
{
  reader->tok_min=reader->tok_max;
  reader->tok_max=ccread_blank(reader->tok_max,reader->doc_max);

  for(;;)
  {
    ccread_token(reader);

    if(reader->tok.kind!=cctoken_kEND)
    {
      *ccarradd(reader->buf,1)=reader->tok;
    }
      else break;
  }
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
  return ccpeep(reader)->kind==kind;
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

ccfunc cctree_t * ccread_arglist(ccread_t *reader, cctree_t *root, cci32_t mark);
ccfunc cctree_t * ccread_expression(ccread_t *parser, cctree_t *root, cci32_t mark);
ccfunc cctree_t * ccread_cast(ccread_t *parser, cctree_t *root, cci32_t mark);



ccfunc ccinle cctree_t *
ccread_identifier(ccread_t *reader, cctree_t *root, cci32_t mark)
{ cctoken_t *token=cceat(reader,cctoken_kLITIDENT);
  cctree_t *tree=ccnil;
  if(token) tree=cctree_identifier(root,mark,token);
  return tree;
}

// Todo: not complete ...
ccfunc cctree_t *
ccread_primary_suffix(ccread_t *reader, cctree_t *result, cctree_t *root, cci32_t mark)
{ while(ccread_continues(reader))
  { cctoken_t *token=ccpeep(reader);
    switch(token->kind)
    { cctree_t *i;
      case cctoken_kLPAREN:
        ccgobble(reader);
        i=ccread_arglist(reader,root,mark);
        if(!cceat(reader,cctoken_kRPAREN))
          ccsynerr(reader, 0, "expected ')'");

        result=cctree_suffixed(root,mark,cctree_kCALL,result,i);
      continue;
      case cctoken_kLSQUARE:
        ccgobble(reader);
        i=ccread_expression(reader,root,mark);
        if(!cceat(reader,cctoken_kRSQUARE))
          ccsynerr(reader, 0, "expected ']'");

        result=cctree_suffixed(root,mark,cctree_kINDEX,result,i);
      continue;
      case cctoken_kDOT:
        ccgobble(reader);
        i=ccread_identifier(reader,root,mark);
        ccassert(i!=0); // Todo: error message ...

        result=cctree_suffixed(root,mark,cctree_kSELECTOR,result,i);
      continue;
    }
    break;
  }
  return result;
}

// Todo: not complete ...
// Note: merged unary and postfix into primary, will have to check the specs again to ensure
// we're still following the grammar rules ...
ccfunc cctree_t *
ccread_primary(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccnull;

  cctoken_t *token=ccpeep(reader);

  switch(token->kind)
  { case cctoken_kLITIDENT:
      result=cctree_identifier(root,mark,ccgobble(reader));
      result=ccread_primary_suffix(reader,result,root,mark);
    break;
    case cctoken_kLITINT:
    case cctoken_kLITFLO:
    case cctoken_kLITSTR:
      result=cctree_constant(root,mark,ccgobble(reader));
      result=ccread_primary_suffix(reader,result,root,mark);
    break;
    case cctoken_kLPAREN:
      ccgobble(reader);
      cctree_t *group=ccread_expression(reader,root,mark);
      if(!cceat(reader,cctoken_kRPAREN))
        ccsynerr(reader,0,"expected ')'");
      result=cctree_unary_ex(root,mark,cctree_kGROUP,group);
      result=ccread_primary_suffix(reader,result,root,mark);
    break;
    case cctoken_kSIZEOF:
      ccgobble(reader);
      if(cceat(reader,cctoken_kLPAREN))
      { ccassert(!"internal");

        if(!cceat(reader,cctoken_kRPAREN))
          ccsynerr(reader,0,"expected ')'");
      } else
      {
        result=cctree_unary_ex(root,mark,cctree_kSIZEOF,ccread_primary(reader,root,mark));
      }
    break;
    case cctoken_kBWAND:
      ccgobble(reader);
      result=cctree_unary_ex(root,mark,cctree_kADDRESSOF,ccread_cast(reader,root,mark));
    break;
    case cctoken_kMUL:
      ccgobble(reader);
      result=cctree_unary_ex(root,mark,cctree_kDEREFERENCE,ccread_cast(reader,root,mark));
    break;
    // Todo:
    case cctoken_kADD:
    case cctoken_kSUB:
      ccgobble(reader);
      result=cctree_unary(root,mark,token->kind,ccread_cast(reader,root,mark));
    break;
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
    result=ccread_primary(reader,root,mark);

  return result;
}

ccfunc cctree_t *
ccread_multiplicative(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_primary(reader,root,mark);

  while(ccread_continues(reader))
  {
    cctoken_t *token=ccpeep(reader);
    switch(token->kind)
    {
      case cctoken_kMUL:
      case cctoken_kDIV:
      case cctoken_kMOD:
        ccgobble(reader);
        result=cctree_binary(root,mark,token,result,ccread_primary(reader,root,mark));
      continue;
    }

    break;
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

  while(ccread_continues(reader))
  {
    cctoken_t *token=ccpeep(reader);
    switch(token->kind)
    {
      case cctoken_kLTN:
      case cctoken_kLTE:
      case cctoken_kGTN:
      case cctoken_kGTE:
        ccgobble(reader);
        result=cctree_binary(root,mark,token,result,ccread_shift(reader,root,mark));
      continue;
    }

    break;
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

  while(ccread_continues(reader) && ccsee(reader,cctoken_kBWAND))
  { cctoken_t *token=ccgobble(reader);
    result=cctree_binary(root,mark,token,result,ccread_equality(reader,root,mark));
  }

  return result;
}

ccfunc cctree_t *
ccread_bitwise_xor(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_bitwise_and(reader,root,mark);

  while(ccread_continues(reader) && ccsee(reader,cctoken_kBWAND))
  { cctoken_t *token = ccgobble(reader);
    result=cctree_binary(root,mark,token,result,ccread_bitwise_and(reader,root,mark));
  }

  return result;
}

// Todo:
ccfunc cctree_t *
ccread_bitwise_or(ccread_t *parser, cctree_t *root, cci32_t mark)
{
  cctree_t *lhs = ccread_bitwise_xor(parser,root,mark);
  while(ccsee(parser, cctoken_kBWOR))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_bitwise_xor(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
  return lhs;
}

ccfunc cctree_t *
ccread_logical_and(ccread_t *parser, cctree_t *root, cci32_t mark)
{
  cctree_t *lhs = ccread_bitwise_or(parser,root,mark);
  while(ccsee(parser, cctoken_kLGAND))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_bitwise_or(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
  return lhs;
}

ccfunc cctree_t *
ccread_logical_or(ccread_t *parser, cctree_t *root, cci32_t mark)
{
  cctree_t *lhs = ccread_logical_and(parser,root,mark);
  while(ccsee(parser, cctoken_kLGOR))
  { cctoken_t *tok = ccgobble(parser);
    cctree_t  *rhs = ccread_logical_and(parser,root,mark);
    lhs = cctree_binary(root,mark,tok, lhs, rhs);
  }
  return lhs;
}

ccfunc cctree_t *
ccread_conditional(ccread_t *parser, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_logical_or(parser,root,mark);

  if(cceat(parser, cctoken_kQMRK))
  { cctree_t *lhs=ccnil,*rhs=ccnil;
    lhs=ccread_logical_or(parser,root,mark);
    if(cceat(parser, cctoken_kCOLON)) // Note: some compiler allow omitting this ...
      rhs=ccread_conditional(parser,root,mark);
    else
      ccsynerr(parser,0,"expected ':' invalid conditional expression");
    result=cctree_ternary(root,mark,result,lhs,rhs);
  }
  return result;
}

ccfunc cctree_t *
ccread_assignment(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_conditional(reader,root,mark);

  // Todo: ensure ltree is unary, which would never be the case ...
  if(ccread_continues(reader))
  {
    cctoken_t *token=ccpeep(reader);
    if(cctoken_is_assignment(token->kind))
    { ccgobble(reader);
      result=cctree_binary(root,mark,token,result,ccread_assignment(reader,root,mark));
    }
  }

  return result;
}

ccfunc ccinle cctree_t *
ccread_constant_expression(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_conditional(reader,root,mark);
  return result;
}

ccfunc cctree_t *
ccread_expression(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_assignment(reader,root,mark);

  cctoken_t *token=cceat(reader,cctoken_kCMA);

  if(token)
    result=cctree_binary(root,mark,token,result,ccread_expression(reader,root,mark));
  return result;
}

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
ccread_decl_name(ccread_t *reader, cctree_t *root, cci32_t mark, cctree_t *type, int, int, int);

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
  if(cceat(reader, cctoken_kLCURLY))
  {
    cctree_t *list = ccread_initializer_list(reader,root,mark);

    if(! cceat(reader, cctoken_kRCURLY))
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

// Todo:
// ccsynwar(reader,0,"function that returns function");
// ccsynwar(reader,0,"function that returns array");

ccfunc cctree_t *
ccread_direct_decl_name_modifier(ccread_t *reader, cctree_t *root, cci32_t mark, cctree_t *result)
{
  cctoken_t *token=ccpeep(reader);

  switch(token->kind)
  { case cctoken_kLPAREN:
    { ccgobble(reader);

      cctree_t **list=ccread_param_type_list(reader,root,mark&~cctree_mEXTERNAL);
      if(!cceat(reader,cctoken_kRPAREN))
        ccsynerr(reader,0,"expected ')'");

      result=cctree_function_modifier(ccread_direct_decl_name_modifier(reader,root,mark,result),list);
    } break;
    case cctoken_kLSQUARE:
    { ccgobble(reader);

      cctree_t *rval=ccread_expression(reader,root,mark);
      if(!cceat(reader,cctoken_kRSQUARE))
        ccsynerr(reader,0,"expected ']'");

      result=cctree_array_modifier(ccread_direct_decl_name_modifier(reader,root,mark,result),rval);

    } break;
  }

  return result;
}

ccfunc ccinle cctree_t *
ccread_decl_name(
  ccread_t *reader, cctree_t *root, cci32_t mark, cctree_t *typed,
    int allow_bitsize, int allow_initializer, int allow_name)
{
  ccassert(typed!=0);

  while(cceat(reader,cctoken_kMUL))
    typed=cctree_pointer_modifier(typed);

  cctree_t *result=typed;
  cctree_t *name=ccnull,*size=ccnull,*init=ccnull;

  if(cceat(reader,cctoken_kLPAREN))
  {
    cctree_t *envoy=cctree_clone(result);
    result=ccread_decl_name(reader,root,mark,envoy,ccfalse,ccfalse,allow_name);

    if(!cceat(reader, cctoken_kRPAREN))
      ccsynerr(reader, 0, "expected ')'");

    cctree_t *dummy=ccread_direct_decl_name_modifier(reader,root,mark,typed);
    *envoy=*dummy;
    cctree_del(dummy);

  } else
  {
    if(ccsee(reader,cctoken_kLITIDENT))
      name=ccread_identifier(reader,root,mark);

    if(name)
      ccsynerr(reader,"'%s': unexpected identifier in abstract context",name->name);

    if(ccread_continues(reader))
      result=ccread_direct_decl_name_modifier(reader,root,mark,result);

    result=cctree_decl_name(root,mark,result,name,size,init);
  }

  if(result)
  {
    if(allow_bitsize)
    {
      if(cceat(reader,cctoken_kCOLON))
      { result->size=ccread_constant_expression(reader,root,mark);
        if(!result->size) ccsynerr(reader,0,"expected constant expression after ':'");
      }
    }

    if(allow_initializer)
    {
      if(cceat(reader,cctoken_kASSIGN))
      { result->init=ccread_initializer(reader,root,mark);
        if(!result->init)
          ccsynerr(reader,0,"expected initializer after '='");
      }
    }
  }

  return result;
}

ccfunc cctree_t *
ccread_init_decl_name(ccread_t *reader, cctree_t *root, cci32_t mark, cctree_t *type)
{
  return ccread_decl_name(reader,root,mark,type,ccfalse,cctrue,cctrue);
}

ccfunc cctree_t *
ccread_struct_or_union_specifier(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccnull;

  if(cceat(reader,cctoken_kSTRUCT))
  {
    cctree_t *name=ccread_identifier(reader,root,mark);

    if(!cceat(reader,cctoken_kLCURLY))
      ccsynerr(reader,0,"expected '{' for struct specifier");

    cctree_t *next,**list,*type;

    next=ccnull;
    list=ccnull;
    type=ccnull;

    while(!ccsee(reader,cctoken_kRCURLY))
    {
      type=ccread_specifier_qualifier_list(reader,root,mark);

      do
      {
        next=ccread_decl_name(reader,root,mark,type,cctrue,ccfalse,cctrue);

        if(next)
          *ccarradd(list,1)=next;
        else
          break;

        if(ccread_continues(reader))
          ccsynerr(reader,0,"expected ';'");

      } while(cceat(reader,cctoken_kCMA));

      // Note: we won't strictly force this ...
      if(ccread_continues(reader))
        ccsynerr(reader,0,"expected ';'");
    }

    if(!cceat(reader, cctoken_kRCURLY))
      ccsynerr(reader,0,"expected '}' for struct specifier");

    if(list!=0)
    {
      result=cctree_new(cctree_kSTRUCT,root,mark);
      result->list=list;
      result->name=cctree_name(name);

    } else
        ccsynerr(reader,0,"'%s': struct or union requires at least one member",name?name->name:"nameless-struct");

  } else
  if(cceat(reader, cctoken_kENUM))
  {
    ccassert(!"noimpl");
  }
  return result;
}

ccfunc cctree_t *
ccread_type_specifier(ccread_t *reader)
{
  cctree_t *result=ccnull;

  cctoken_t *token=ccpeep(reader);
  switch(token->kind)
  { case cctoken_kENUM:
    case cctoken_kSTRUCT:
      result=ccread_struct_or_union_specifier(reader,ccnull,ccnull);
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
    case cctoken_kSTDC_SIGNED:
    case cctoken_kSTDC_UNSIGNED:
      ccgobble(reader);
      result=cctree_new(cctree_kTYPENAME,ccnull,ccnull);
      result->sort=token->kind;
      result->loca=token->loca;
    break;
  }

  return result;
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
  cctoken_t *token=ccpeep(reader);

  cctree_t *result=ccnull;

// ccread_type_specifier:

  // Todo: properly parse this ...
  switch(token->kind)
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
      result=cctree_new(cctree_kTYPENAME,root,mark);
      result->sort=token->kind;
      result->loca=token->loca;
    break;
  }

// ccread_type_qualifier:

// ccread_alignment_specifier:

  // Note: this is a hint that this is a function ...
// ccread_function_specifier:

  return result;
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
  // ccread_storage_class_specifier(reader,root,mark);

  cctree_t *type = ccread_specifier_qualifier_list(reader,root,mark);

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
  if(spec) decl=ccread_decl_name(reader,root,mark,spec,ccfalse,ccfalse,cctrue);

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

ccfunc cctree_t *ccread_statement(ccread_t *reader, cctree_t *root, cci32_t mark);

// Note: what if in the seer stage we created some sort of jump entity... and it would map to an
// instruction index, instead of ...
// cctree_t **list=ccnil;
// if(ccread_continues(reader))
//   list=ccread_statement_list(reader,root,mark);
// result=cctree_label(root,mark,result,list);
ccfunc cctree_t *
ccread_statement(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccnull;
  cctree_t *cond_tree=ccnull;
  cctree_t *then_tree=ccnull;
  cctree_t *else_tree=ccnull;

  cctoken_t *token=ccpeep(reader);

  switch(token->kind)
  {
    // Todo: how would we do this properly, should instead we peek ahead, by token, I think we should ...
    default:
    {
      result=ccread_declaration_specifiers(reader,root,mark);

      if(result!=0)
      {
        if(ccread_continues(reader))
          goto read_decl;
        else
          goto leave;

      } else
      {
        result=ccread_expression(reader,root,mark);

        if(!ccread_continues(reader))
          goto leave;
      }

      if(result!=0 && result->kind==cctree_kIDENTIFIER)
      {
        if(cceat(reader,cctoken_kCOLON))
        {
          ccassert(!"internal");
        }
          else goto read_decl;
      }
        else goto leave;

      if(result!=0)
      {
read_decl:

        cctree_t *next,**list=ccnull;
        while(ccread_continues(reader))
        {
          next=ccread_init_decl_name(reader,root,mark,result);

          if(next)
          {
            *ccarradd(list,1)=next;
          } else
            ccsynerr(reader,0,"expected declarator");

          if(!cceat(reader,cctoken_kCMA))
            break;
        }
        if(list!=0)
        {
          result=cctree_decl(root,mark,result,list);
        }
      }
    } break;
    case cctoken_kLCURLY:
    { if(!cceat(reader,cctoken_kLCURLY))
        ccsynerr(reader,0,"expected '{'");

      result=cctree_block(root,mark,ccnull);

      while(!ccsee(reader,cctoken_kEND) && !ccsee(reader,cctoken_kRCURLY))
      {
        cctree_t *next=ccread_statement(reader,result,mark);

        if(!next) break;

        *ccarradd(result->list,1)=next;
      }

      if(!cceat(reader,cctoken_kRCURLY))
        ccsynerr(reader,0,"expected '}'");
    } break;
    case cctoken_kGOTO:
    { ccgobble(reader);

      cctree_t *ident=ccread_identifier(reader,root,mark);
      ccassert(ident!=0); // Todo: error message

      result=cctree_goto(root,mark,ident);

      if(ccread_continues(reader))
        ccsynerr(reader, 0, "expected ';'");
    } break;
    case cctoken_kRETURN:
    { ccgobble(reader);

      result=cctree_return(root,mark,ccread_expression(reader,root,mark));
      if(!result->rval)
        ccsynerr(reader,0,"expected expression");
      if(!reader->term_expl)
        ccsynerr(reader, 0, "expected ';'");
    } break;
    case cctoken_kWHILE:
    { ccgobble(reader);

      if(!cceat(reader,cctoken_kLPAREN))
        ccsynerr(reader,0,"expected '('");

      cond_tree=ccread_expression(reader,root,mark);

      if(!cceat(reader,cctoken_kRPAREN))
        ccsynerr(reader,0,"expected ')'");

      if(!cond_tree)
        ccsynerr(reader,0,"expected expression");

      if(ccread_continues(reader))
      {
        then_tree=ccread_statement(reader,root,mark);

        if(!then_tree)
          ccsynerr(reader,0,"expected statement");
      }

      result=cctree_while(root,mark,cond_tree,then_tree);
    } break;
    case cctoken_kIF:
    { ccgobble(reader);

      if(!cceat(reader,cctoken_kLPAREN))
        ccsynerr(reader,0,"expected '('");

      cond_tree=ccread_expression(reader,root,mark);

      if(!cceat(reader,cctoken_kRPAREN))
        ccsynerr(reader,0,"expected ')'");

      if(!cond_tree) ccsynerr(reader,0,"expected expression");

      if(ccread_continues(reader))
      {
        then_tree=ccread_statement(reader,root,mark);

        if(!then_tree)
          ccsynerr(reader,0,"expected statement");

        if(cceat(reader,cctoken_kELSE))
        { else_tree=ccread_statement(reader,root,mark);
          if(!else_tree)
            ccsynerr(reader,0,"expected statement");
        }
      }
      result=cctree_ternary(root,mark,cond_tree,then_tree,else_tree);
    } break;
  }
leave:
  return result;
}

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
  cctree_t *decl=ccread_statement(reader,root,mark|cctree_mEXTERNAL);

  if(!decl)
    return ccnull;

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

    name->blob=ccread_statement(reader,name,mark);

    ccassert(!name->blob || name->blob->kind==cctree_kBLOCK); // Todo: error message

    if(!name->blob&&!reader->term_expl) ccsynerr(reader,0,"expected ';'");
  } else
  {
    // Note: applies to every declaration other than a function declaration with its definition ...
    if(!reader->term_expl) ccsynerr(reader,0,"expected ';'");
  }

  if(!decl&&!ccsee_end(reader)) ccsynerr(reader,0,"invalid external declaration");
  return decl;
}



#endif