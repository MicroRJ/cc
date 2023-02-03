/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
/*****************************************************************/

ccfunc cctree_t *
ccread_external_declaration(ccreader_t *reader);

ccfunc cctree_t *
ccread_translation_unit(ccreader_t *reader)
{ cctree_t *list=ccarrnil;
  while(cctree_t *next=ccread_external_declaration(reader))
  { *ccarradd(list,1)=*next; cctree_del(next);
  }
  return list;
}

ccfunc cctree_t *
ccread_external_declaration(ccreader_t *reader)
{
  cctree_t *decl=ccread_init_decl(reader);

  if(!decl) return ccnil;


  ccassert(decl->kind==cctree_Kdecl);

  ccnotnil(decl->decl_name);
  ccassert(decl->decl_name->kind==cctree_Kdecl_name);
  ccnotnil(decl->decl_name->decl_name_type);
  ccnotnil(decl->decl_name->decl_name_iden);

  if(decl->decl_name->decl_name_type->kind==cctype_Kfunc)
  {
    // You can't declarte multiple functions within the same declaration ...
    ccassert(ccarrlen(decl->decl_name)==1);

    decl->decl_name->body_tree=ccread_mixed_statement(reader);

    if(!decl->decl_name->body_tree&&!reader->bed->term_expl) ccsynerr(reader,0,"expected ';'");
  } else
  {
    // Note: applies to every declaration other than a function definition, with its definition ...
    if(!reader->bed->term_expl) ccsynerr(reader,0,"expected ';'");
  }

  if(!decl&&!ccsee_end(reader)) ccsynerr(reader,0,"invalid external declaration");
  return decl;
}























