#if 0
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
    result=cctree_conditional(root,mark,result,lhs,rhs);
  }
  return result;
}
#endif

#if 0
ccfunc cctree_t *
ccread_multiplicative(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_primary(reader,root,mark);
  while(ccread_continues(reader))
  { cctoken_t *token=ccpeep(reader);
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
  while(ccread_continues(reader))
  { cctoken_t *token=ccpeep(reader);
    switch(token->kind)
    { case cctoken_kADD:
      case cctoken_kSUB:
        ccgobble(reader);
        result=cctree_binary(root,mark,token,result,ccread_multiplicative(reader,root,mark));
      continue;
    }
    break;
  }
  return result;
}

ccfunc cctree_t *
ccread_shift(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_additive(reader,root,mark,3);
  while(ccread_continues(reader))
  { cctoken_t *token=ccpeep(reader);
    switch(token->kind)
    { case cctoken_kBWSHL:
      case cctoken_kBWSHR:
        ccgobble(reader);
        result=cctree_binary(root,mark,token,result,ccread_additive(reader,root,mark,3));
      continue;
    }
    break;
  }
  return result;
}

ccfunc cctree_t *
ccread_relational(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_shift(reader,root,mark);
  while(ccread_continues(reader))
  { cctoken_t *token=ccpeep(reader);
    switch(token->kind)
    { case cctoken_kLTN:
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
  while(ccread_continues(reader))
  { cctoken_t *token=ccpeep(reader);
    switch(token->kind)
    { case cctoken_kTEQ:
      case cctoken_kFEQ:
        ccgobble(reader);
        result=cctree_binary(root,mark,token,result,ccread_equality(reader,root,mark));
      continue;
    }
    break;
  }
  return result;
}

ccfunc cctree_t *
ccread_bitwise_and(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_equality(reader,root,mark);
  while(ccread_continues(reader))
  { cctoken_t *token=ccpeep(reader);
    switch(token->kind)
    { case cctoken_kBWAND:
        ccgobble(reader);
        result=cctree_binary(root,mark,token,result,ccread_equality(reader,root,mark));
      continue;
    }
    break;
  }
  return result;
}

ccfunc cctree_t *
ccread_bitwise_xor(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *result=ccread_bitwise_and(reader,root,mark);
  while(ccread_continues(reader))
  { cctoken_t *token=ccpeep(reader);
    switch(token->kind)
    { case cctoken_kBWXOR:
        ccgobble(reader);
        result=cctree_binary(root,mark,token,result,ccread_bitwise_and(reader,root,mark));
      continue;
    }
    break;
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
#endif

#if 0
ccfunc void
ccctree_print(cctree_t *tree)
{
  if(tree->kind==cctree_kBINARY)
  { ccprintf("(");
    switch(tree->sort)
    { case cctoken_kMUL:
        ccctree_print(tree->lval),ccprintf("*"),ccctree_print(tree->rval); break;
      case cctoken_kDIV:
        ccctree_print(tree->lval),ccprintf("/"),ccctree_print(tree->rval); break;
      case cctoken_kMOD:
        ccctree_print(tree->lval),ccprintf("%"),ccctree_print(tree->rval); break;
      case cctoken_kADD:
        ccctree_print(tree->lval),ccprintf("+"),ccctree_print(tree->rval); break;
      case cctoken_kSUB:
        ccctree_print(tree->lval),ccprintf("-"),ccctree_print(tree->rval); break;
      case cctoken_kBWSHL:
        ccctree_print(tree->lval),ccprintf("<<"),ccctree_print(tree->rval); break;
      case cctoken_kBWSHR:
        ccctree_print(tree->lval),ccprintf(">>"),ccctree_print(tree->rval); break;
      case cctoken_kTEQ:
        ccctree_print(tree->lval),ccprintf("=="),ccctree_print(tree->rval); break;
      case cctoken_kFEQ:
        ccctree_print(tree->lval),ccprintf("!="),ccctree_print(tree->rval); break;
    }
    ccprintf(")");
  } else
  if(tree->kind==cctree_kCONSTANT)
  {
    if(tree->sort==cctoken_kLITINT)
    {
      ccprintf("%lli",(cci64_t)tree->name);
    }
  }
}
#endif
