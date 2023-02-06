/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
/*****************************************************************/

ccfunc cctree_t *
ccread_external_declaration(ccread_t *reader);

ccfunc cctree_t *
ccread_translation_unit(ccread_t *reader)
{ cctree_t *list=ccnil;
  while(cctree_t *next=ccread_external_declaration(reader))
  { *ccarradd(list,1)=*next; cctree_del(next);
  }
  return list;
}

ccfunc cctree_t *
ccread_external_declaration(ccread_t *reader)
{
  cctree_t *decl=ccread_init_decl(reader);

  if(!decl) return ccnil;


  ccassert(decl->kind==cctree_Kdecl);

  ccnotnil(decl->decl_list);
  ccassert(decl->decl_list->kind==cctree_Kdecl_name);
  ccnotnil(decl->decl_list->decl_type);
  ccnotnil(decl->decl_list->decl_name);

  if(decl->decl_list->decl_type->kind==cctype_Kfunc)
  {
    // You can't declare multiple functions within the same declaration ...
    ccassert(ccarrlen(decl->decl_list)==1);

    decl->decl_list->body_tree=ccread_block_stmt(reader);

    if(!decl->decl_list->body_tree&&!reader->bed->term_expl) ccsynerr(reader,0,"expected ';'");
  } else
  {
    // Note: applies to every declaration other than a function definition, with its definition ...
    if(!reader->bed->term_expl) ccsynerr(reader,0,"expected ';'");
  }

  if(!decl&&!ccsee_end(reader)) ccsynerr(reader,0,"invalid external declaration");
  return decl;
}























