
// Note: this is C's compound statement, where declarations and statements are split ...
ccfunc cctree_t *
cctree_block_stmt(cctree_t *decl, cctree_t *stmt)
{ cctree_t *tree=cctree_new(cctree_kBLOCK);
  tree->decl_list=decl;
  tree->stmt_list=stmt;
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
{ ccassert(name!=0&&name->kind==cctree_kIDENTIFIER);
  cctree_t *tree=cctree_new(cctree_kLABEL);
  tree->label_name=cctree_idenname(name);
  tree->stmt_list=list;
  return tree;
}

ccfunc cctree_t *
cctree_goto_statement(cctree_t *name)
{ ccassert(name!=0&&name->kind==cctree_kIDENTIFIER);
  cctree_t *tree=cctree_new(cctree_Kgoto_statement);
  tree->label_name=cctree_idenname(name);
  return tree;
}

ccfunc cctree_t *
cctree_return_statement(cctree_t *expr_tree)
{ cctree_t *tree=cctree_new(cctree_kRETRN);
  tree->expr_tree=expr_tree;
  return tree;
}

ccfunc cctree_t *ccread_block_stmt(ccread_t *reader);
ccfunc cctree_t *ccread_stmt_list(ccread_t *reader);
ccfunc cctree_t *ccread_stmt(ccread_t *reader);

ccfunc cctree_t *
ccread_block_or_single_stmt(ccread_t *reader)
{ cctree_t *stmt=ccread_block_stmt(reader);
  if(!stmt) stmt=ccread_stmt(reader);
  return stmt;
}

// Badger Tadger
ccfunc cctree_t *
ccread_stmt(ccread_t *reader)
{
  cctree_t *child=ccnil;

  if(ccsee(reader,cctoken_Krcurly))
  {
    // Note: don't do anything ...
  } else
  if(cceat(reader,cctoken_Klcurly))
  { child=ccread_block_stmt(reader);
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
      then_tree=ccread_block_or_single_stmt(reader);
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
    { then_tree=ccread_block_or_single_stmt(reader);
      if(!then_tree) ccsynerr(reader,0,"expected statement");
    }
    if(cceat(reader,cctoken_Kelse)&&!reader->bed->term_expl)
    { else_tree=ccread_block_or_single_stmt(reader);
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
      if(child->kind==cctree_kIDENTIFIER)
      {
        if(cceat(reader,cctoken_Kcolon))
        {

        	cctree_t *list=ccnil;
        	if(!reader->bed->term_expl)
        	{ list=ccread_stmt_list(reader);
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
ccread_stmt_list(ccread_t *reader)
{
  cctree_t *next,*list=ccnil;
  while(next=ccread_stmt(reader))
  {
    *ccarradd(list,1)=*next;
    cctree_del(next);

    if(ccsee_end(reader)) break;
  }
  return list;
}

ccfunc cctree_t *
ccread_block_stmt(ccread_t *reader)
{ if(cceat(reader,cctoken_Klcurly))
  { cctree_t *stmt=ccread_stmt_list(reader);
    if(!cceat(reader,cctoken_Krcurly))
      ccsynerr(reader, 0, "expected '}'");
    return cctree_block_stmt(ccnil,stmt);
  }
  return ccnil;
}
