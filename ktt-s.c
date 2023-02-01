/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
/*****************************************************************/
#ifndef KTT_HEADER_FILE_S
#define KTT_HEADER_FILE_S

ccfunc void
kttc__token2s(cclex_t *lexer, char **buf, cctok_t *token)
{
  switch(token->bit)
  { case cctokentype_endexpl:           ccstr_catf(buf, ";"); break;
    case cctokentype_literal_ellipsis:  ccstr_catf(buf, "..."); break;
    case cctokentype_literal_character: ccstr_catf(buf, "%c", token->sig); break;
    case cctokentype_literal_integer:   ccstr_catf(buf, "%lli", token->sig); break;
    case cctokentype_literal_float:     ccstr_catf(buf, "%f", token->flo); break;
    case cctokentype_literal_identifier:
    case cctokentype_literal_string:
      ccstr_catf(buf, "\"");
      ccstr_catf(buf, cclex_tokstr(lexer, token));
      ccstr_catf(buf, "\""); break;
    case cctokentype_lparen:  ccstr_catf(buf, "("); break;
    case cctokentype_rparen:  ccstr_catf(buf, ")"); break;
    case cctokentype_lcurly:  ccstr_catf(buf, "["); break;
    case cctokentype_rcurly:  ccstr_catf(buf, "]"); break;
    case cctokentype_lsquare: ccstr_catf(buf, "["); break;
    case cctokentype_rsquare: ccstr_catf(buf, "]"); break;
    case cctokentype_conditional:   ccstr_catf(buf, "?"); break;
    case cctokentype_colon:   ccstr_catf(buf, ":"); break;
    case cctokentype_mso:     ccstr_catf(buf, "."); break;
    case cctokentype_msp:     ccstr_catf(buf, "->"); break;
    case cctokentype_comma:   ccstr_catf(buf, ","); break;
    case cctokentype_mul: ccstr_catf(buf, "*"); break;
    case cctokentype_div: ccstr_catf(buf, "/"); break;
    case cctokentype_mod: ccstr_catf(buf, "%"); break;
    case cctokentype_sub: ccstr_catf(buf, "-"); break;
    case cctokentype_add: ccstr_catf(buf, "+"); break;
    case cctokentype_ptr_dereference: ccstr_catf(buf, "*"); break;
    case cctokentype_ptr_address_of: ccstr_catf(buf, "&"); break;
    case cctokentype_pos_decrement: ccstr_catf(buf, "--"); break;
    case cctokentype_pos_increment: ccstr_catf(buf, "++"); break;
    case cctokentype_pre_decrement: ccstr_catf(buf, "--"); break;
    case cctokentype_pre_increment: ccstr_catf(buf, "++"); break;
    case cctokentype_negate: ccstr_catf(buf, "!"); break;
    case cctokentype_greater_than: ccstr_catf(buf, ">"); break;
    case cctokentype_less_than: ccstr_catf(buf, "<"); break;
    case cctokentype_bitwise_invert: ccstr_catf(buf, "~"); break;
    case cctokentype_bitwise_xor: ccstr_catf(buf, "^"); break;
    case cctokentype_bitwise_or: ccstr_catf(buf, "^"); break;
    case cctokentype_bitwise_and: ccstr_catf(buf, "&"); break;
    case cctokentype_bitwise_shl: ccstr_catf(buf, "<<"); break;
    case cctokentype_bitwise_shr: ccstr_catf(buf, ">>"); break;
    case cctokentype_logical_or: ccstr_catf(buf, "||"); break;
    case cctokentype_logical_and: ccstr_catf(buf, "&&"); break;
    case cctokentype_equals: ccstr_catf(buf, "=="); break;
    case cctokentype_not_equals: ccstr_catf(buf, "!="); break;
    case cctokentype_assign: ccstr_catf(buf, "=" ); break;
    case cctokentype_mul_eql: ccstr_catf(buf, "*="); break;
    case cctokentype_div_eql: ccstr_catf(buf, "/="); break;
    case cctokentype_mod_eql: ccstr_catf(buf, "%="); break;
    case cctokentype_sub_eql: ccstr_catf(buf, "-="); break;
    case cctokentype_add_eql: ccstr_catf(buf, "+="); break;
    case cctokentype_less_than_eql: ccstr_catf(buf, "<="); break;
    case cctokentype_greater_than_eql: ccstr_catf(buf, ">="); break;
    case cctokentype_bitwise_xor_eql: ccstr_catf(buf, "^="); break;
    case cctokentype_bitwise_and_eql: ccstr_catf(buf, "&="); break;
    case cctokentype_bitwise_or_eql: ccstr_catf(buf, "^="); break;
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
	ccarr_for(tree->struct_decl.list,it)
  { if(it!=tree->struct_decl.list) ccstr_catf(buf,",");

    kttc__type2s(reader,buf,it->struct_decl_name.decl->decl_name.type);
    ccstr_catf(buf,"=");
    cctree_t2s(reader,buf,it->struct_decl_name.decl->decl_name.name);
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
      ccstr_catf(buf,"struct %s:",cclex_tokstr(&reader->lex,&type->name->constant.token));

      cctree_t *it;
      ccarr_for(type->list,it)
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
    case cctype_fun: ccstr_catf(buf, "(fun: "); kttc__type2s(parser, buf, type->modifier_of); ccstr_catf(buf, ")"); break;
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
{ if(tree == ktt__nullptr)
  {
    ccstr_catf(buf, "cctree_t::nil");
    return;
  }

  switch(tree->kind)
  {
    case cctree_t_compound_statement:
    {
      // ccstr_catf(buf, "(compound_statement\n");
      cctree_t *it;
      ccarr_for(tree->compound_statement.list,it)
      { if(it!=tree->compound_statement.list)
        { ccstr_catf(buf, "\n");
        }
        // ktt__catchr(buf,' ',2);
        cctree_t2s(parser, buf, it->statement.decl_or_expr);
      }
      // ccstr_catf(buf, ")");
    } break;
    case cctree_init_decl:
    {
    	cctree_t *it;
    	ccarr_for(tree->init_decl.list,it)
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
    case cctree_t_bop:
    {
      ccstr_catf(buf, "(");
      cctree_t2s(parser, buf, tree->bop.lhs);
      ccstr_catf(buf, " ");
      kttc__token2s(& parser->lex, buf, & tree->bop.opr);
      ccstr_catf(buf, " ");
      cctree_t2s(parser, buf, tree->bop.rhs);
      ccstr_catf(buf, ")");
    } break;
    case cctree_t_uop:
    {
      ccstr_catf(buf, "(");
      kttc__token2s(& parser->lex, buf, & tree->uop.opr);
      ccstr_catf(buf, " ");
      cctree_t2s(parser, buf, tree->uop.mhs);
      ccstr_catf(buf, ")");
    } break;
    case cctree_t_iname:
    case cctree_t_tname:
    case cctree_t_integer:
    case cctree_t_float:
    case cctree_t_string:
    { kttc__token2s(& parser->lex, buf, & tree->constant.token);
    } break;
  }
}


#endif