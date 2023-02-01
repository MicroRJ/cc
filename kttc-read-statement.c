ccfunc cctree_t * cctree_new_compound_statement(cctree_t *list);
ccfunc cctree_t * cctree_new_statement(cctree_t *decl_or_expr);
ccfunc cctree_t * ccread_statement_list(ccreader_t *parser);
ccfunc cctree_t * ccread_statement(ccreader_t *parser);

ccfunc cctree_t *
cctree_new_compound_statement(cctree_t *list)
{
  cctree_t *tree = cctree_new(cctree_t_compound_statement);
  tree->compound_statement.list = list;
  return tree;
}

ccfunc cctree_t *
cctree_new_statement(cctree_t *decl_or_expr)
{
  cctree_t *tree = cctree_new(cctree_t_statement);
  tree->statement.decl_or_expr = decl_or_expr;
  return tree;
}

// TODO(RJ): this is wrong, technically a statement is composed of declarations and statements.
// we have to split the init_declarations into declarations and statements ...
ccfunc cctree_t *
ccread_statement(ccreader_t *parser)
{
  cctree_t *child={};

  if(ccsee(parser,cctokentype_rcurly))
  {
    // Note: don't do anything ...
  } else
  if(cceat(parser,cctokentype_lcurly))
  { child=ccread_statement_list(parser);
    if(!cceat(parser,cctokentype_rcurly)) ccsynerr(parser, 0, "expected ending '}' of compound statement");
    return cctree_new_statement(child);
  } else
  if(child=ccread_init_decl(parser))
  { if(!parser->bed->term_expl) ccsynerr(parser, 0, "expected ';' at end of statement");
    return cctree_new_statement(child);
  } else
  if(child=ccread_expression(parser))
  { if(!parser->bed->term_expl) ccsynerr(parser, 0, "expected ';' at end of statement");
    return cctree_new_statement(child);
  }

  return 0;
}

ccfunc cctree_t *
ccread_statement_list(ccreader_t *reader)
{ cctree_t *list=ccarr_empty;
  do
  { // Todo: ccread_statement should take a pointer, if the pointer is null then return a new tree ...
    cctree_t *next=ccread_statement(reader);

    // Note: some sort of error occurred ..
    if(!next) break;

    *ccarr_add(list,1)=*next;
    cctree_del(next);
  } while(!ccsee_end(reader));

  if(list) return cctree_new_compound_statement(list);

  return 0;
}