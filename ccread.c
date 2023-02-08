/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
/*****************************************************************/

ccfunc cctree_t *
ccread_external_declaration(ccread_t *reader, cctree_t *root, cci32_t mark);

ccfunc cctree_t *
ccread_translation_unit(ccread_t *reader)
{
	cctree_t *tree=cctree_translation_unit();

	cctree_t *list=ccnil;
  while(cctree_t *next=ccread_external_declaration(reader,tree,0))
  { *ccarradd(list,1)=*next; cctree_del(next);
  }

  return list;
}

ccfunc cctree_t *
ccread_external_declaration(ccread_t *reader, cctree_t *root, cci32_t mark)
{
  cctree_t *decl=ccread_init_decl(reader,root,mark|cctree_mLVALUE|cctree_mEXTERNAL);

  if(!decl) return ccnil;

  ccassert(decl->kind==cctree_kDECL);
  ccnotnil(decl->list);

  cctree_t *name=*decl->list;

  ccassert(name->kind==cctree_kDECLNAME);
  ccnotnil(name->type);
  ccnotnil(name->name);

  if(name->type->kind==cctype_Kfunc)
  {
    // Note: You can't define multiple functions within the same declaration ...
    ccassert(ccarrlen(decl->list)==1);

    name->blob=ccread_block(reader,name,mark);

    if(!name->blob&&!reader->bed->term_expl) ccsynerr(reader,0,"expected ';'");
  } else
  {
    // Note: applies to every declaration other than a function declaration with its definition ...
    if(!reader->bed->term_expl) ccsynerr(reader,0,"expected ';'");
  }

  if(!decl&&!ccsee_end(reader)) ccsynerr(reader,0,"invalid external declaration");
  return decl;
}
