/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
/*****************************************************************/
#ifndef KTT_HEADER_FILE_S
#define KTT_HEADER_FILE_S

#if 0
ccfunc void
kttc__token2s(char **buf, cctoken_t *token)
{
  switch(token->bit)
  { case cctoken_Kendexpl:           ccstr_catf(buf, ";"); break;
    case cctoken_Kliteral_ellipsis:  ccstr_catf(buf, "..."); break;
    case cctoken_Kliteral_character: ccstr_catf(buf, "%c", token->sig); break;
    case cctoken_Kliteral_integer:   ccstr_catf(buf, "%lli", token->sig); break;
    case cctoken_Kliteral_float:     ccstr_catf(buf, "%f", token->flo); break;
    case cctoken_Kliteral_identifier: ccstr_catf(buf, "%s",token->str); break;
    case cctoken_Kliteral_string: ccstr_catf(buf, "\"%s\"",token->str); break;
    case cctoken_Klparen:  ccstr_catf(buf, "("); break;
    case cctoken_Krparen:  ccstr_catf(buf, ")"); break;
    case cctoken_Klcurly:  ccstr_catf(buf, "["); break;
    case cctoken_Krcurly:  ccstr_catf(buf, "]"); break;
    case cctoken_Klsquare: ccstr_catf(buf, "["); break;
    case cctoken_Krsquare: ccstr_catf(buf, "]"); break;
    case cctoken_Kconditional:   ccstr_catf(buf, "?"); break;
    case cctoken_Kcolon:   ccstr_catf(buf, ":"); break;
    case cctoken_Kmso:     ccstr_catf(buf, "."); break;
    case cctoken_Kmsp:     ccstr_catf(buf, "->"); break;
    case cctoken_Kcomma:   ccstr_catf(buf, ","); break;
    case cctoken_Kmul: ccstr_catf(buf, "*"); break;
    case cctoken_Kdiv: ccstr_catf(buf, "/"); break;
    case cctoken_Kmod: ccstr_catf(buf, "%"); break;
    case cctoken_Ksub: ccstr_catf(buf, "-"); break;
    case cctoken_Kadd: ccstr_catf(buf, "+"); break;
    case cctoken_Kptr_dereference: ccstr_catf(buf, "*"); break;
    case cctoken_Kptr_address_of: ccstr_catf(buf, "&"); break;
    case cctoken_Kpos_decrement: ccstr_catf(buf, "--"); break;
    case cctoken_Kpos_increment: ccstr_catf(buf, "++"); break;
    case cctoken_Kpre_decrement: ccstr_catf(buf, "--"); break;
    case cctoken_Kpre_increment: ccstr_catf(buf, "++"); break;
    case cctoken_Knegate: ccstr_catf(buf, "!"); break;
    case cctoken_Kgreater_than: ccstr_catf(buf, ">"); break;
    case cctoken_Kless_than: ccstr_catf(buf, "<"); break;
    case cctoken_Kbitwise_invert: ccstr_catf(buf, "~"); break;
    case cctoken_Kbitwise_xor: ccstr_catf(buf, "^"); break;
    case cctoken_Kbitwise_or: ccstr_catf(buf, "^"); break;
    case cctoken_Kbitwise_and: ccstr_catf(buf, "&"); break;
    case cctoken_Kbitwise_shl: ccstr_catf(buf, "<<"); break;
    case cctoken_Kbitwise_shr: ccstr_catf(buf, ">>"); break;
    case cctoken_Klogical_or: ccstr_catf(buf, "||"); break;
    case cctoken_Klogical_and: ccstr_catf(buf, "&&"); break;
    case cctoken_Kequals: ccstr_catf(buf, "=="); break;
    case cctoken_Knot_equals: ccstr_catf(buf, "!="); break;
    case cctoken_Kassign: ccstr_catf(buf, "=" ); break;
    case cctoken_Kmul_eql: ccstr_catf(buf, "*="); break;
    case cctoken_Kdiv_eql: ccstr_catf(buf, "/="); break;
    case cctoken_Kmod_eql: ccstr_catf(buf, "%="); break;
    case cctoken_Ksub_eql: ccstr_catf(buf, "-="); break;
    case cctoken_Kadd_eql: ccstr_catf(buf, "+="); break;
    case cctoken_Kless_than_eql: ccstr_catf(buf, "<="); break;
    case cctoken_Kgreater_than_eql: ccstr_catf(buf, ">="); break;
    case cctoken_Kbitwise_xor_eql: ccstr_catf(buf, "^="); break;
    case cctoken_Kbitwise_and_eql: ccstr_catf(buf, "&="); break;
    case cctoken_Kbitwise_or_eql: ccstr_catf(buf, "^="); break;
    default: ccstr_catf(buf, "error"); break;
  }
}

ccfunc void
cctree_t2s(ccreader_t *parser, char **buf, cctree_t *tree);
ccfunc void
kttc__type2s(ccreader_t *parser, char **buf, cctype_t *type);


ccfunc void
ccstructdecl_tos(ccreader_t *reader, char **buf, cctree_t *tree)
{
  ccstr_catf(buf,"decl: ");
	kttc__type2s(reader,buf,tree->struct_decl.type);
  ccstr_catf(buf," ");

  cctree_t *it;
	ccarrfor(tree->struct_decl.list,it)
  { if(it!=tree->struct_decl.list) ccstr_catf(buf,",");

    kttc__type2s(reader,buf,it->struct_decl_name.decl->decl_name.type);
    ccstr_catf(buf,"=%s",it->struct_decl_name.decl->decl_name.name);
  }
  ccstr_catf(buf,"\n");
}

ccfunc void
cctypestruct_tos(ccreader_t *reader, char **buf, cctype_t *type)
{
  if(type->kind==cctype_struct_spec)
  {
    if(type->name)
    {
      ccstr_catf(buf,"struct %s:",type->name);

      cctree_t *it;
      ccarrfor(type->list,it)
      { ccstr_catf(buf, "\n");
      	ccstructdecl_tos(reader,buf,it);
      }

    } else
    {
      ccstr_catf(buf,"struct <unnamed>");
    }
  }

}


ccfunc void
kttc__type2s(ccreader_t *parser, char **buf, cctype_t *type)
{ switch(type->kind)
  { case cctype_arr: ccstr_catf(buf, "(arr: "); kttc__type2s(parser, buf, type->modifier_of); ccstr_catf(buf, ")"); break;
    case cctype_Kfunc: ccstr_catf(buf, "(fun: "); kttc__type2s(parser, buf, type->modifier_of); ccstr_catf(buf, ")"); break;
    case cctype_ptr: ccstr_catf(buf, "(ptr: "); kttc__type2s(parser, buf, type->modifier_of); ccstr_catf(buf, ")"); break;
    case cctype_float32: ccstr_catf(buf, "float32"); break;
    case cctype_float64: ccstr_catf(buf, "float64"); break;
    case cctype_int64:   ccstr_catf(buf, "int64"); break;
    case cctype_int32:   ccstr_catf(buf, "int32"); break;
    case cctype_int16:   ccstr_catf(buf, "int16"); break;
    case cctype_int8:    ccstr_catf(buf, "int8"); break;
    case cctype_uint64:  ccstr_catf(buf, "uint64"); break;
    case cctype_uint32:  ccstr_catf(buf, "uint32"); break;
    case cctype_uint16:  ccstr_catf(buf, "uint16"); break;
    case cctype_uint8:   ccstr_catf(buf, "uint8"); break;
    case cctype_void:    ccstr_catf(buf, "void"); break;
    case cctype_struct_spec:  cctypestruct_tos(parser,buf,type); break;
  }
}

ccfunc void
cctree_t2s(ccreader_t *parser, char **buf, cctree_t *tree)
{ if(tree == ccnil)
  {
    ccstr_catf(buf, "cctree_t::nil");
    return;
  }

  switch(tree->kind)
  {
    case cctree_Kmixed_statement:
    {
      // ccstr_catf(buf, "(compound_statement\n");
      cctree_t *it;
      ccarrfor(tree->compound_statement.list,it)
      { if(it!=tree->compound_statement.list)
        { ccstr_catf(buf, "\n");
        }
        // ktt__catchr(buf,' ',2);
        cctree_t2s(parser, buf, it->statement.decl_or_expr);
      }
      // ccstr_catf(buf, ")");
    } break;
    case cctree_Kinit_decl:
    {
    	cctree_t *it;
    	ccarrfor(tree->init_decl.list,it)
      {
      	kttc__type2s(parser, buf, it->init_decl_name.decl->decl_name.type);
        ccstr_catf(buf, "=");
        cctree_t2s(parser, buf, it->init_decl_name.init);
      }
    } break;
    case cctree_t_top:
    {
      ccstr_catf(buf, "(");
      cctree_t2s(parser, buf, tree->top.lhs);
      ccstr_catf(buf, " ? ");
      cctree_t2s(parser, buf, tree->top.mhs);
      ccstr_catf(buf, " : ");
      cctree_t2s(parser, buf, tree->top.rhs);
      ccstr_catf(buf, ")");
    } break;
    case cctree_Kbinary:
    {
      ccstr_catf(buf, "(");
      cctree_t2s(parser, buf, tree->binary.lhs);
      ccstr_catf(buf, " ");
      kttc__token2s(buf, & tree->binary.opr);
      ccstr_catf(buf, " ");
      cctree_t2s(parser, buf, tree->binary.rhs);
      ccstr_catf(buf, ")");
    } break;
    case cctree_t_uop:
    {
      ccstr_catf(buf, "(");
      kttc__token2s(buf, & tree->uop.opr);
      ccstr_catf(buf, " ");
      cctree_t2s(parser, buf, tree->uop.mhs);
      ccstr_catf(buf, ")");
    } break;
    case cctree_Kidentifier:
    case cctree_t_tname:
    case cctree_Kint:
    case cctree_t_float:
    case cctree_t_string:
    { kttc__token2s(buf, & tree->constant.token);
    } break;
  }
}
#endif

#endif