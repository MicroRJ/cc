
ccfunc cctree_t *
cctree_mixed_statement(cctree_t *stat)
{ cctree_t *tree=cctree_new(cctree_Kmixed_statement);
  tree->mix_statement.stat=stat;
  return tree;
}

ccfunc cctree_t *
cctree_conditional_statement(cctree_t *cond_tree, cctree_t *then_tree, cctree_t *else_tree)
{ ccnotnil(cond_tree);
  cctree_t *tree=cctree_new(cctree_Kconditional_statement);
  tree->cond_tree=cond_tree;
  tree->then_tree=then_tree;
  tree->else_tree=else_tree;
  return tree;
}

ccfunc cctree_t *
cctree_while_statement(cctree_t *cond_tree, cctree_t *then_tree)
{ ccnotnil(cond_tree);
  cctree_t *tree=cctree_new(cctree_Kwhile_statement);
  tree->cond_tree=cond_tree;
  tree->then_tree=then_tree;
  return tree;
}

ccfunc cctree_t *
cctree_label_statement(cctree_t *name, cctree_t *list)
{ ccassert(name!=0&&name->kind==cctree_Kidentifier);
  cctree_t *tree=cctree_new(cctree_Klabel_statement);
  tree->label_statement.name=cctree_idenname(name);
  tree->label_statement.list=list;
  return tree;
}


// Note: jump statements ...

ccfunc cctree_t *
cctree_goto_statement(cctree_t *name)
{ ccassert(name!=0&&name->kind==cctree_Kidentifier);
  cctree_t *tree=cctree_new(cctree_Kgoto_statement);
  tree->goto_statement.name=cctree_idenname(name);
  return tree;
}

ccfunc cctree_t *
cctree_return_statement(cctree_t *expr_tree)
{ cctree_t *tree=cctree_new(cctree_Kreturn_statement);
  tree->expr_tree=expr_tree;
  return tree;
}

ccfunc cctree_t *
ccread_mixed_statement(ccreader_t *reader);
ccfunc cctree_t *
ccread_statement_list(ccreader_t *reader);
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
    if(!cceat(reader,cctoken_Krcurly)) ccsynerr(reader, 0, "expected '}'");
    return child;
  } else
  if(child=ccread_init_decl(reader))
  { if(!reader->bed->term_expl) ccsynerr(reader, 0, "expected ';'");
    return child;
  } else
  if(cceat(reader,cctoken_Kreturn))
  {
  	cctree_t *expr_tree=ccread_expression(reader);
  	child=cctree_return_statement(expr_tree);

    if(!expr_tree) ccsynerr(reader,0,"expected expression");

    if(!reader->bed->term_expl) ccsynerr(reader, 0, "expected ';'");
  } else
  if(cceat(reader,cctoken_Kwhile))
  {
    cctree_t *cond_tree=ccnil,*then_tree=ccnil;

    if(!cceat(reader,cctoken_Klparen)) ccsynerr(reader,0,"expected '('");
    cond_tree=ccread_expression(reader);
    if(!cceat(reader,cctoken_Krparen)) ccsynerr(reader,0,"expected ')'");

    if(!cond_tree) ccsynerr(reader,0,"expected expression");

    if(!reader->bed->term_expl)
    {
      then_tree=ccread_mixed_or_single_statement(reader);
      if(!then_tree) ccsynerr(reader,0,"expected statement");
    }
    child=cctree_while_statement(cond_tree,then_tree);
  } else
  if(cceat(reader,cctoken_Kgoto))
  {
    cctree_t *ident=ccread_identifier(reader);
    if(!ident) ccsynerr(reader,0,"missing goto label identifier");

    child=cctree_goto_statement(ident);

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

    if(!cceat(reader,cctoken_Klparen)) ccsynerr(reader,0,"expected '('");
    cond_tree=ccread_expression(reader);
    if(!cceat(reader,cctoken_Krparen)) ccsynerr(reader,0,"expected ')'");

    if(!cond_tree) ccsynerr(reader,0,"expected expression");

    if(!reader->bed->term_expl)
    { then_tree=ccread_mixed_or_single_statement(reader);
      if(!then_tree) ccsynerr(reader,0,"expected statement");
    }
    if(cceat(reader,cctoken_Kelse)&&!reader->bed->term_expl)
    { else_tree=ccread_mixed_or_single_statement(reader);
      if(!else_tree) ccsynerr(reader,0,"expected statement");
    }

    child=cctree_conditional_statement(cond_tree,then_tree,else_tree);

  } else
  // Note: make sure you call this last, simply because this is a rather expensive function ... not that I care ...
  if(child=ccread_expression(reader))
  {
    // Todo: better way of doing this?
    if(!reader->bed->term_expl)
    {
      if(child->kind==cctree_Kidentifier)
      {
        if(cceat(reader,cctoken_Kcolon))
        {

        	cctree_t *list=ccnil;
        	if(!reader->bed->term_expl)
        	{ list=ccread_statement_list(reader);
        	}
          child=cctree_label_statement(child,list);
        } else
          ccsynerr(reader, 0, "invalid statement, missing ':' for label statement?");
      } else
        ccsynerr(reader, 0, "invalid statement");

    }
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
      ccsynerr(reader, 0, "expected '}'");
    return cctree_mixed_statement(stat);
  }
  return ccnil;
}
