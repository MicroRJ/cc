
ccfunc cctree_t *
cctree_new_mix_statement(cctree_t *stat)
{ cctree_t *tree=cctree_new(cctree_Kmixed_statement);
  tree->mix_statement.stat=stat;
  return tree;
}

ccfunc cctree_t *
cctree_conditional_statement(cctree_t *cond, cctree_t **stat)
{ ccassert(cond!=0);
  ccassert(stat!=0);
  cctree_t *tree=cctree_new(cctree_Kconditional_statement);
  tree->conditional_statement.cond=cond;
  tree->conditional_statement.stat[0]=stat[0];
  tree->conditional_statement.stat[1]=stat[1];
  return tree;
}

ccfunc cctree_t *
cctree_while_statement(cctree_t *cond, cctree_t *stat)
{ ccassert(cond!=0);
  ccassert(stat!=0);
  cctree_t *tree=cctree_new(cctree_Kwhile_statement);
  tree->while_statement.cond=cond;
  tree->while_statement.stat=stat;
  return tree;
}

ccfunc cctree_t *
ccread_mixed_statement(ccreader_t *reader);

ccfunc cctree_t *
ccread_statement(ccreader_t *reader);

ccfunc cctree_t *
ccread_mixed_or_single_statement(ccreader_t *reader)
{ cctree_t *stat;
  stat=ccread_mixed_statement(reader);
  if(!stat)stat=ccread_statement(reader);
  return stat;
}

// Badger Tadger
ccfunc cctree_t *
ccread_statement(ccreader_t *reader)
{
  cctree_t *child=ccnil;

  if(ccsee(reader,cctoken_Krcurly))
  {
    // Note: don't do anything ...
  } else
  if(cceat(reader,cctoken_Klcurly))
  { child=ccread_mixed_statement(reader);
    if(!cceat(reader,cctoken_Krcurly)) ccsynerr(reader, 0, "expected ending '}' of compound statement");
    return child;
  } else
  if(child=ccread_init_decl(reader))
  { if(!reader->bed->term_expl) ccsynerr(reader, 0, "expected ';' at end of statement");
    return child;
  } else
  if(cceat(reader,cctoken_Kwhile))
  {
    cctree_t *cond=ccnil;
    cctree_t *stat=ccnil;

    if(!cceat(reader,cctoken_Klparen)) ccsynerr(reader,0,"expected '(' for while statement");
    cond=ccread_expression(reader);
    if(!cceat(reader,cctoken_Krparen)) ccsynerr(reader,0,"expected ')' for while statement");

    if(!reader->bed->term_expl)
    {
      stat=ccread_mixed_or_single_statement(reader);
      if(!stat) ccsynerr(reader,0,"expected while statement");
    }
    child=cctree_while_statement(cond,stat);
  } else
  if(ccsee(reader,cctoken_Kelse))
  {
    // Note: don't do anything, this could be intentional ...
  } else
  if(cceat(reader,cctoken_Kif))
  {
    cctree_t *cond=ccnil;
    cctree_t *stat[2]={ccnil,ccnil};

    if(!cceat(reader,cctoken_Klparen)) ccsynerr(reader,0,"expected '(' for if statement");
    cond=ccread_expression(reader);
    if(!cceat(reader,cctoken_Krparen)) ccsynerr(reader,0,"expected ')' for if statement");

    if(!cond) ccsynerr(reader,0,"expected conditional expression for if statement");

    if(!reader->bed->term_expl)
    { stat[0]=ccread_mixed_or_single_statement(reader);
      if(!stat[0]) ccsynerr(reader,0,"expected true statement");
    }
    if(cceat(reader,cctoken_Kelse)&&!reader->bed->term_expl)
    { stat[1]=ccread_mixed_or_single_statement(reader);
      if(!stat[1]) ccsynerr(reader,0,"expected false statement");
    }
    child=cctree_conditional_statement(cond,stat);
  } else
  // Note: make sure you call this last, simply because this is a rather expensive function ... not that I care ...
  if(child=ccread_expression(reader))
  { if(!reader->bed->term_expl) ccsynerr(reader, 0, "expected ';' at end of statement");
    return child;
  }
  return child;
}

ccfunc cctree_t *
ccread_statement_list(ccreader_t *reader)
{
  cctree_t *next,*list=ccarrnil;
  for(next=ccread_statement(reader);next;next=ccread_statement(reader))
  {
    *ccarradd(list,1)=*next;
    cctree_del(next);

    if(ccsee_end(reader)) break;
  }
  return list;
}

ccfunc cctree_t *
ccread_mixed_statement(ccreader_t *reader)
{ if(cceat(reader,cctoken_Klcurly))
  { cctree_t *stat=ccread_statement_list(reader);
    if(!cceat(reader,cctoken_Krcurly))
      ccsynerr(reader, 0, "expected ending '}' of compound statement");
    return cctree_new_mix_statement(stat);
  }
  return ccnil;
}
