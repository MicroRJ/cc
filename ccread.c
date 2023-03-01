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

ccfunc cctree_t * ccread_arglist(ccread_t *reader, cctree_t *root, int mark);
ccfunc cctree_t * ccread_expression(ccread_t *parser, cctree_t *root, int mark);
ccfunc cctree_t * ccread_cast(ccread_t *parser, cctree_t *root, int mark);
ccfunc cctree_t * ccread_type_name(ccread_t *reader, cctree_t *root, int mark);


ccfunc ccinle cctree_t *
ccread_identifier(ccread_t *reader, cctree_t *root, int mark)
{ cctoken_t *token=cceat(reader,cctoken_kLITIDENT);
  cctree_t *tree=ccnil;
  if(token) tree=cctree_identifier(root,mark,token);
  return tree;
}

// Todo: not complete ...

// TODO: FIX THIS
ccfunc cctree_t *
ccread_primary_suffix(ccread_t *reader, cctree_t *root, int mark, cctree_t *result)
{
  while(ccread_continues(reader))
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
    } break;
  }
  return result;
}

// Todo: not complete ...
// Note: merged unary and postfix into primary, will have to check the specs again to ensure
// we're still following the grammar rules ...
ccfunc cctree_t *
ccread_primary(ccread_t *reader, cctree_t *root, int mark)
{
  cctree_t *result=ccnull;

  cctoken_t *token=ccpeep(reader);

  switch(token->kind)
  { case cctoken_kLITIDENT:
      result=cctree_identifier(root,mark,ccgobble(reader));
      result=ccread_primary_suffix(reader,root,mark,result);
    break;
    case cctoken_kLITINT:
    case cctoken_kLITFLO:
    case cctoken_kLITSTR:
      result=cctree_constant(root,mark,ccgobble(reader));
      result=ccread_primary_suffix(reader,root,mark,result);
    break;
    case cctoken_kLPAREN:
      ccgobble(reader);
      cctree_t *group=ccread_expression(reader,root,mark);
      if(!cceat(reader,cctoken_kRPAREN))
        ccsynerr(reader,0,"expected ')'");
      result=cctree_unary_ex(root,mark,cctree_kGROUP,group);
      result=ccread_primary_suffix(reader,root,mark,result);
    break;
    case cctoken_kSIZEOF:
      ccgobble(reader);
      if(cceat(reader,cctoken_kLPAREN))
      { result=ccread_type_name(reader,root,mark);
        if(!cceat(reader,cctoken_kRPAREN))
          ccsynerr(reader,0,"expected ')'");
      } else
      {
        result=ccread_primary(reader,root,mark);
      }
      result=cctree_unary_ex(root,mark,cctree_kSIZEOF,result);
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

  // Debugger helper:
  if(result!=0)
  {
    result->loca=token->loca;
  }

  return result;
}

ccfunc cctree_t *
ccread_cast(ccread_t *reader, cctree_t *root, int mark)
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

// Note: there's probably a better way of doing this, and I haven't tested this method
// to be reliable nor efficient...
ccfunc cctree_t *
ccread_binary(ccread_t *reader, cctree_t *root, int mark, int c)
{ cctree_t *result=ccread_primary(reader,root,mark);

  while(ccread_continues(reader))
  { cctoken_t *token=ccpeep(reader);
    switch(token->kind)
    { case cctoken_kMUL:
      case cctoken_kDIV:
      case cctoken_kMOD:
        if(c<0) break;
        ccgobble(reader);
        result=cctree_binary(root,mark,token,result,ccread_binary(reader,root,mark,0));
      continue;
      case cctoken_kADD:
      case cctoken_kSUB:
        if(c<1) break;
        ccgobble(reader);
        result=cctree_binary(root,mark,token,result,ccread_binary(reader,root,mark,1));
      continue;
      case cctoken_kBWSHL:
      case cctoken_kBWSHR:
        if(c<2) break;
        ccgobble(reader);
        result=cctree_binary(root,mark,token,result,ccread_binary(reader,root,mark,2));
      continue;
      case cctoken_kLTN:
      case cctoken_kLTE:
      case cctoken_kGTN:
      case cctoken_kGTE:
        if(c<3) break;
        ccgobble(reader);
        result=cctree_binary(root,mark,token,result,ccread_binary(reader,root,mark,3));
      continue;
      case cctoken_kTEQ:
      case cctoken_kFEQ:
        if(c<4) break;
        ccgobble(reader);
        result=cctree_binary(root,mark,token,result,ccread_binary(reader,root,mark,4));
      continue;
      case cctoken_kBWAND:
        if(c<5) break;
        ccgobble(reader);
        result=cctree_binary(root,mark,token,result,ccread_binary(reader,root,mark,5));
      continue;
      case cctoken_kBWXOR:
        if(c<6) break;
        ccgobble(reader);
        result=cctree_binary(root,mark,token,result,ccread_binary(reader,root,mark,6));
      continue;
      case cctoken_kBWOR:
        if(c<7) break;
        ccgobble(reader);
        result=cctree_binary(root,mark,token,result,ccread_binary(reader,root,mark,7));
      continue;
      case cctoken_kLGAND:
        if(c<8) break;
        ccgobble(reader);
        result=cctree_binary(root,mark,token,result,ccread_binary(reader,root,mark,8));
      continue;
      case cctoken_kLGOR:
        if(c<9) break;
        ccgobble(reader);
        result=cctree_binary(root,mark,token,result,ccread_binary(reader,root,mark,9));
      continue;
    }

    break;
  }

  return result;
}

ccfunc cctree_t *
ccread_conditional(ccread_t *reader, cctree_t *root, int mark)
{
  cctree_t *result=ccread_binary(reader,root,mark,10);

  if(ccread_continues(reader))
  {
    if(cceat(reader, cctoken_kQMRK))
    { cctree_t *lhs=ccnull,*rhs=ccnull;
      lhs=ccread_binary(reader,root,mark,10);
      if(cceat(reader,cctoken_kCOLON))
        rhs=ccread_conditional(reader,root,mark);
      else
        ccsynerr(reader,0,"expected ':' invalid conditional expression");
      result=cctree_conditional(root,mark,result,lhs,rhs);
    }
  }

  return result;
}


ccfunc cctree_t *
ccread_assignment(ccread_t *reader, cctree_t *root, int mark)
{
  cctree_t *result=ccread_conditional(reader,root,mark);

  // Todo: ensure ltree is unary ...
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
ccread_constant_expression(ccread_t *reader, cctree_t *root, int mark)
{
  cctree_t *result=ccread_conditional(reader,root,mark);
  return result;
}

ccfunc cctree_t *
ccread_expression(ccread_t *reader, cctree_t *root, int mark)
{
  cctree_t *result=ccread_assignment(reader,root,mark);

  cctoken_t *token=cceat(reader,cctoken_kCMA);

  if(token)
    result=cctree_binary(root,mark,token,result,ccread_expression(reader,root,mark));
  return result;
}

ccfunc cctree_t *
ccread_arglist(ccread_t *reader, cctree_t *root, int mark)
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
ccread_param_type_list(ccread_t *reader, cctree_t *root, int mark);

ccfunc cctree_t *
ccread_initializer(ccread_t *reader, cctree_t *root, int mark);

ccfunc cctree_t *ccread_decl_spec(ccread_t *reader, cctree_t *root, int mark);
ccfunc cctree_t *ccread_decl(ccread_t *reader, cctree_t *root, int mark, cctree_t *type, int, int);

#if 0
ccfunc cctree_t *
ccread_designator(ccread_t *reader, cctree_t *root, int mark)
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
ccread_designator_list(ccread_t *reader, cctree_t *root, int mark)
{
  cctree_t *result = ccread_designator(reader,root,mark);
  if(result)
  { result->designator.next = ccread_designator_list(reader,root,mark);
  }
  return result;
  return ccnil;
}

ccfunc cctree_t *
ccread_init_designation(ccread_t *reader, cctree_t *root, int mark)
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
ccread_initializer_list(ccread_t *reader, cctree_t *root, int mark)
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
ccread_initializer(ccread_t *reader, cctree_t *root, int mark)
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

// Todo: not finished ...
ccfunc cctree_t *
ccread_record_spec(ccread_t *reader, cctree_t *root, int mark)
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

    while(!ccsee(reader,cctoken_kEND) && !ccsee(reader,cctoken_kRCURLY))
    {
      type=ccread_decl_spec(reader,root,mark);

      do
      { next=ccread_decl(reader,root,mark,type,cctrue,ccfalse);

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
    };

    if(!cceat(reader,cctoken_kRCURLY))
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
ccread_type_spec(ccread_t *reader)
{
  cctree_t *result=ccnull;

  cctoken_t *token=ccpeep(reader);
  switch(token->kind)
  { case cctoken_kENUM:
    case cctoken_kSTRUCT:
      result=ccread_record_spec(reader,ccnull,ccnull);
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
      result->name=token->name;
      result->sort=token->kind;
      result->loca=token->loca;
    break;
  }

  return result;
}

ccfunc cctree_t *
ccread_affixed_modifiers(ccread_t *reader, cctree_t *root, int mark, cctree_t *result)
{
  if(ccread_continues(reader))
  { cctoken_t *token=ccpeep(reader);
    switch(token->kind)
    { case cctoken_kLPAREN:
      { ccgobble(reader);

        cctree_t **list=ccread_param_type_list(reader,root,mark&~cctree_mEXTERNAL);
        if(!cceat(reader,cctoken_kRPAREN))
          ccsynerr(reader,0,"expected ')'");

        cctree_t *modifier=ccread_affixed_modifiers(reader,root,mark,result);
        result=cctree_function_modifier(modifier,list);
      } break;
      case cctoken_kLSQUARE:
      { ccgobble(reader);

        cctree_t *rval=ccread_expression(reader,root,mark);
        if(!cceat(reader,cctoken_kRSQUARE))
          ccsynerr(reader,0,"expected ']'");

        cctree_t *modifier=ccread_affixed_modifiers(reader,root,mark,result);
        result=cctree_array_modifier(modifier,rval);
      } break;
    }
  }
  return result;
}

ccfunc cctree_t *
ccread_type_name_modifiers(
  ccread_t *reader, cctree_t *root, int mark, cctree_t *result)
{
  ccassert(result!=0);

  while(cceat(reader,cctoken_kMUL))
    result=cctree_pointer_modifier(result);

  if(cceat(reader,cctoken_kLPAREN))
  {
    cctree_t *clone=cctree_clone(result);
    cctree_t *child=ccread_type_name_modifiers(reader,root,mark,clone);

    if(!cceat(reader,cctoken_kRPAREN))
      ccsynerr(reader, 0, "expected ')'");

    cctree_t *dummy=ccread_affixed_modifiers(reader,root,mark,result);
    *clone=*dummy;

    cctree_del(dummy);

    result=child;
  } else
  {
    if(cceat(reader,cctoken_kLITIDENT))
      ccsynerr(reader,0,"'%s': unexpected identifier in abstract type-name",name);

    result=ccread_affixed_modifiers(reader,root,mark,result);
  }
  return result;
}

ccfunc cctree_t *
ccread_type_name(
  ccread_t *reader, cctree_t *root, int mark)
{
  cctree_t *result=ccread_type_spec(reader);

  if(result)
  {
    result=ccread_type_name_modifiers(reader,root,mark,result);
  }

  return result;
}

// Todo: follow the actual spec
ccfunc cctree_t *
ccread_decl_spec(ccread_t *reader, cctree_t *root, int mark)
{
  cctree_t *type=ccread_type_spec(reader);
  return type;
}


ccfunc cctree_t *
ccread_decl(
  ccread_t *reader, cctree_t *root, int mark, cctree_t *typed, int allow_bitsize, int allow_initializer)
{
  ccassert(typed!=0);

  while(cceat(reader,cctoken_kMUL))
    typed=cctree_pointer_modifier(typed);

  cctree_t *result=typed;
  cctree_t *name=ccnull,*size=ccnull,*init=ccnull;

  if(cceat(reader,cctoken_kLPAREN))
  {
    cctree_t *envoy=cctree_clone(result);
    result=ccread_decl(reader,root,mark,envoy,ccfalse,ccfalse);

    if(!cceat(reader,cctoken_kRPAREN))
      ccsynerr(reader,0,"expected ')'");

    cctree_t *dummy=ccread_affixed_modifiers(reader,root,mark,typed);
    *envoy=*dummy;
    cctree_del(dummy);

  } else
  {
    if(ccsee(reader,cctoken_kLITIDENT))
      name=ccread_identifier(reader,root,mark);

    result=ccread_affixed_modifiers(reader,root,mark,result);

    result=cctree_decl(root,mark,result,name,size,init);
  }

  if(result)
  {
    if(ccread_continues(reader))
    {
      if(allow_bitsize)
      {
        if(cceat(reader,cctoken_kCOLON))
        { result->size=ccread_constant_expression(reader,root,mark);
          if(!result->size)
            ccsynerr(reader,0,"expected constant expression after ':'");
        }
      }
    }

    if(ccread_continues(reader))
    {
      if(allow_initializer)
      {
        if(cceat(reader,cctoken_kASSIGN))
        { result->init=ccread_initializer(reader,root,mark);
          if(!result->init)
            ccsynerr(reader,0,"expected initializer after '='");
        }
      }
    }
  }




  return result;
}

ccfunc cctree_t *
ccread_param_decl(ccread_t *reader, cctree_t *root, int mark)
{
  cctree_t *spec=ccnil;
  cctree_t *decl=ccnil;

  if(ccsee(reader,cctoken_Kliteral_ellipsis))
    ccsynerr(reader,0,"unexpected '...', must be at end of function");

  spec=ccread_decl_spec(reader,root,mark);
  if(spec) decl=ccread_decl(reader,root,mark,spec,ccfalse,ccfalse);

  return decl;
}

ccfunc cctree_t **
ccread_param_decl_list(ccread_t *reader, cctree_t *root, int mark)
{ cctree_t *next,**list=ccnil;
  while(next=ccread_param_decl(reader,root,mark))
  { *ccarradd(list,1)=next;
    if(!cceat(reader,cctoken_kCMA)) break;
  }
  return list;
}

ccfunc cctree_t **
ccread_param_type_list(ccread_t *reader, cctree_t *root, int mark)
{
  cctree_t **list=ccread_param_decl_list(reader,root,mark);
  return list;
}

ccfunc cctree_t *
ccread_statement(ccread_t *reader, cctree_t *root, int mark)
{
  cctree_t *result=ccnull;

  cctree_t *cond_tree=ccnull;
  cctree_t *then_tree=ccnull;
  cctree_t *else_tree=ccnull;

  cctoken_t *token=ccpeep(reader);

  switch(token->kind)
  {
    case cctoken_kEND:
    break;

    default:
    {
      result=ccread_expression(reader,root,mark);
    } break;
    case cctoken_kLITIDENT:
      // Note: check if this identifier could be part of a declaration...
      if(!token->term_expl)
      {
        cctoken_t *second=ccpeek(reader,1);

        if((second->kind!=cctoken_kMUL) &&
           (second->kind!=cctoken_kLITIDENT))
        {
          result=ccread_expression(reader,root,mark);
          break;
        }
      }
    case cctoken_kENUM:
    case cctoken_kSTRUCT:
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
    {
      cctree_t *type;
      if((token->kind==cctoken_kSTRUCT)||
         (token->kind==cctoken_kENUM))
      {
        type=ccread_record_spec(reader,root,mark);
      } else
      {
        ccgobble(reader);

        if(token->kind!=cctoken_kLITIDENT)
          type=cctree_new(cctree_kTYPENAME,root,mark);
        else
          type=cctree_new(cctree_kIDENTIFIER,root,mark);

        type->sort=token->kind;
        type->loca=token->loca;
        type->name=token->name;
      }

      if(!token->term_expl)
      {
        result=ccread_decl(reader,root,mark,type,ccfalse,cctrue);

        // Debugger helper:
        ccassert(result->name!=0);

        cctree_t *next=result;
        while(ccread_continues(reader) && cceat(reader,cctoken_kCMA))
          next=next->next=ccread_decl(reader,root,mark,type,ccfalse,cctrue);
      }

    } break;
    case cctoken_kLCURLY:
    { if(!cceat(reader,cctoken_kLCURLY))
        ccsynerr(reader,0,"expected '{'");

      result=cctree_block(root,mark,ccnull);

      while(!ccsee(reader,cctoken_kEND) && !ccsee(reader,cctoken_kRCURLY))
      {
        cctree_t *next=ccread_statement(reader,result,mark);

        if(ccread_continues(reader))
          ccsynerr(reader, 0, "expected ';'");

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
    } break;
    case cctoken_kRETURN:
    { ccgobble(reader);

      result=cctree_return(root,mark,ccread_expression(reader,root,mark));

      if(!result->rval)
        ccsynerr(reader,0,"expected expression");
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

        if(then_tree->kind!=cctree_kBLOCK)
        {
          if(ccread_continues(reader))
            ccsynerr(reader, 0, "expected ';'");
        }

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

        if(ccread_continues(reader))
          ccsynerr(reader, 0, "expected ';'");

        if(cceat(reader,cctoken_kELSE))
        { else_tree=ccread_statement(reader,root,mark);
          if(!else_tree)
            ccsynerr(reader,0,"expected statement");

          if(ccread_continues(reader))
            ccsynerr(reader, 0, "expected ';'");
        }
      }
      result=cctree_conditional(root,mark,cond_tree,then_tree,else_tree);
    } break;
  }

  return result;
}

ccfunc cctree_t *
ccread_external_declaration(ccread_t *reader, cctree_t *root, int mark);

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
ccread_external_declaration(ccread_t *reader, cctree_t *root, int mark)
{
  cctree_t *decl=ccread_statement(reader,root,mark|cctree_mEXTERNAL);

  if(!decl)
    return ccnull;

  ccassert(decl->kind==cctree_kDECL);
  ccassert(decl->type!=0);
  ccassert(decl->name!=0);

  // Todo: is this something that we wan't to do here or in decl?

  // Todo: are we following the spec by doing this?
  if(decl->type->kind==cctree_kFUNCTION)
  {
    // Note: You can't define multiple functions within the same declaration ...
    ccassert(decl->next==0);

    decl->blob=ccread_statement(reader,decl,mark);

    ccassert(!decl->blob || decl->blob->kind==cctree_kBLOCK); // Todo: error messages

    if(!decl->blob)
    {
      if(ccread_continues(reader))
        ccsynerr(reader,0,"expected ';'");
    }

  } else
  {
    if(ccread_continues(reader))
      ccsynerr(reader,0,"expected ';'");
  }

  return decl;
}



#endif