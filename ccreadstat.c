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

    if(!cceat(reader,cctoken_Klparen)) ccsynerr(reader,0,"expected '('");
    cond_tree=ccread_expression(reader,root,mark);
    if(!cceat(reader,cctoken_Krparen)) ccsynerr(reader,0,"expected ')'");

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

    if(!cceat(reader,cctoken_Klparen)) ccsynerr(reader,0,"expected '('");
    cond_tree=ccread_expression(reader,root,mark);
    if(!cceat(reader,cctoken_Krparen)) ccsynerr(reader,0,"expected ')'");

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
  { tree=cctree_block(root,mark,ccnil,ccnil);
  	tree->list=ccread_statement_list(reader,tree,mark);
    if(!cceat(reader,cctoken_Krcurly))
      ccsynerr(reader, 0, "expected '}'");
  }
  return tree;
}
