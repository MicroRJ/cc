/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
/*****************************************************************/
#ifndef KTT_HEADER_FILE_S
#define KTT_HEADER_FILE_S

ccfunc void
kttc__token2s(cclex_t *lexer, char **buf, cctok_t *token)
{
  switch(token->bit)
  { case cctokentype_endexpl:           ccstr__catf(buf, ";"); break;
    case cctokentype_literal_ellipsis:  ccstr__catf(buf, "..."); break;
    case cctokentype_literal_character: ccstr__catf(buf, "%c", token->sig); break;
    case cctokentype_literal_integer:   ccstr__catf(buf, "%lli", token->sig); break;
    case cctokentype_literal_float:     ccstr__catf(buf, "%f", token->flo); break;
    case cctokentype_literal_identifier:
    case cctokentype_literal_string:
      ccstr__catf(buf, "\"");
      ccstr__catf(buf, cclex_tokstr(lexer, token));
      ccstr__catf(buf, "\""); break;
    case cctokentype_lparen:  ccstr__catf(buf, "("); break;
    case cctokentype_rparen:  ccstr__catf(buf, ")"); break;
    case cctokentype_lcurly:  ccstr__catf(buf, "["); break;
    case cctokentype_rcurly:  ccstr__catf(buf, "]"); break;
    case cctokentype_lsquare: ccstr__catf(buf, "["); break;
    case cctokentype_rsquare: ccstr__catf(buf, "]"); break;
    case cctokentype_conditional:   ccstr__catf(buf, "?"); break;
    case cctokentype_colon:   ccstr__catf(buf, ":"); break;
    case cctokentype_mso:     ccstr__catf(buf, "."); break;
    case cctokentype_msp:     ccstr__catf(buf, "->"); break;
    case cctokentype_comma:   ccstr__catf(buf, ","); break;
    case cctokentype_mul: ccstr__catf(buf, "*"); break;
    case cctokentype_div: ccstr__catf(buf, "/"); break;
    case cctokentype_mod: ccstr__catf(buf, "%"); break;
    case cctokentype_sub: ccstr__catf(buf, "-"); break;
    case cctokentype_add: ccstr__catf(buf, "+"); break;
    case cctokentype_ptr_dereference: ccstr__catf(buf, "*"); break;
    case cctokentype_ptr_address_of: ccstr__catf(buf, "&"); break;
    case cctokentype_pos_decrement: ccstr__catf(buf, "--"); break;
    case cctokentype_pos_increment: ccstr__catf(buf, "++"); break;
    case cctokentype_pre_decrement: ccstr__catf(buf, "--"); break;
    case cctokentype_pre_increment: ccstr__catf(buf, "++"); break;
    case cctokentype_negate: ccstr__catf(buf, "!"); break;
    case cctokentype_greater_than: ccstr__catf(buf, ">"); break;
    case cctokentype_less_than: ccstr__catf(buf, "<"); break;
    case cctokentype_bitwise_invert: ccstr__catf(buf, "~"); break;
    case cctokentype_bitwise_xor: ccstr__catf(buf, "^"); break;
    case cctokentype_bitwise_or: ccstr__catf(buf, "^"); break;
    case cctokentype_bitwise_and: ccstr__catf(buf, "&"); break;
    case cctokentype_bitwise_shl: ccstr__catf(buf, "<<"); break;
    case cctokentype_bitwise_shr: ccstr__catf(buf, ">>"); break;
    case cctokentype_logical_or: ccstr__catf(buf, "||"); break;
    case cctokentype_logical_and: ccstr__catf(buf, "&&"); break;
    case cctokentype_equals: ccstr__catf(buf, "=="); break;
    case cctokentype_not_equals: ccstr__catf(buf, "!="); break;
    case cctokentype_assign: ccstr__catf(buf, "=" ); break;
    case cctokentype_mul_eql: ccstr__catf(buf, "*="); break;
    case cctokentype_div_eql: ccstr__catf(buf, "/="); break;
    case cctokentype_mod_eql: ccstr__catf(buf, "%="); break;
    case cctokentype_sub_eql: ccstr__catf(buf, "-="); break;
    case cctokentype_add_eql: ccstr__catf(buf, "+="); break;
    case cctokentype_less_than_eql: ccstr__catf(buf, "<="); break;
    case cctokentype_greater_than_eql: ccstr__catf(buf, ">="); break;
    case cctokentype_bitwise_xor_eql: ccstr__catf(buf, "^="); break;
    case cctokentype_bitwise_and_eql: ccstr__catf(buf, "&="); break;
    case cctokentype_bitwise_or_eql: ccstr__catf(buf, "^="); break;
    default: ccstr__catf(buf, "error"); break;
  }
}

ccfunc void
cctree_t2s(ccreader_t *parser, char **buf, cctree_t *tree);
ccfunc void
kttc__type2s(ccreader_t *parser, char **buf, cctype_t *type);


ccfunc void
ccstructdecl_tos(ccreader_t *reader, char **buf, cctree_t *tree)
{
  ccstr__catf(buf,"decl: ");
	kttc__type2s(reader,buf,tree->struct_decl.type);
  ccstr__catf(buf," ");

  cctree_t *it;
	ccarr_for(tree->struct_decl.list,it)
  { if(it!=tree->struct_decl.list) ccstr__catf(buf,",");

    kttc__type2s(reader,buf,it->struct_decl_name.decl->decl_name.type);
    ccstr__catf(buf,"=");
    cctree_t2s(reader,buf,it->struct_decl_name.decl->decl_name.name);
  }
  ccstr__catf(buf,"\n");
}

ccfunc void
cctypestruct_tos(ccreader_t *reader, char **buf, cctype_t *type)
{
  if(type->kind==cctype_struct_spec)
  {
    if(type->name)
    {
      ccstr__catf(buf,"struct %s:",cclex_tokstr(&reader->lex,&type->name->constant.token));

      cctree_t *it;
      ccarr_for(type->list,it)
      { ccstr__catf(buf, "\n");
      	ccstructdecl_tos(reader,buf,it);
      }

    } else
    {
      ccstr__catf(buf,"struct <unnamed>");
    }
  }

}


ccfunc void
kttc__type2s(ccreader_t *parser, char **buf, cctype_t *type)
{ switch(type->kind)
  { case cctype_arr: ccstr__catf(buf, "(arr: "); kttc__type2s(parser, buf, type->modifier_of); ccstr__catf(buf, ")"); break;
    case cctype_fun: ccstr__catf(buf, "(fun: "); kttc__type2s(parser, buf, type->modifier_of); ccstr__catf(buf, ")"); break;
    case cctype_ptr: ccstr__catf(buf, "(ptr: "); kttc__type2s(parser, buf, type->modifier_of); ccstr__catf(buf, ")"); break;
    case cctype_float32: ccstr__catf(buf, "float32"); break;
    case cctype_float64: ccstr__catf(buf, "float64"); break;
    case cctype_int64:   ccstr__catf(buf, "int64"); break;
    case cctype_int32:   ccstr__catf(buf, "int32"); break;
    case cctype_int16:   ccstr__catf(buf, "int16"); break;
    case cctype_int8:    ccstr__catf(buf, "int8"); break;
    case cctype_uint64:  ccstr__catf(buf, "uint64"); break;
    case cctype_uint32:  ccstr__catf(buf, "uint32"); break;
    case cctype_uint16:  ccstr__catf(buf, "uint16"); break;
    case cctype_uint8:   ccstr__catf(buf, "uint8"); break;
    case cctype_void:    ccstr__catf(buf, "void"); break;
    case cctype_struct_spec:  cctypestruct_tos(parser,buf,type); break;
  }
}

ccfunc void
cctree_t2s(ccreader_t *parser, char **buf, cctree_t *tree)
{ if(tree == ktt__nullptr)
  {
    ccstr__catf(buf, "cctree_t::nil");
    return;
  }

  switch(tree->kind)
  {
    case cctree_t_compound_statement:
    {
      // ccstr__catf(buf, "(compound_statement\n");
      cctree_t *it;
      ccarr_for(tree->compound_statement.list,it)
      { if(it!=tree->compound_statement.list)
        { ccstr__catf(buf, "\n");
        }
        // ktt__catchr(buf,' ',2);
        cctree_t2s(parser, buf, it->statement.decl_or_expr);
      }
      // ccstr__catf(buf, ")");
    } break;
    case cctree_init_decl:
    {
    	cctree_t *it;
    	ccarr_for(tree->init_decl.list,it)
      {
      	kttc__type2s(parser, buf, it->init_decl_name.decl->decl_name.type);
        ccstr__catf(buf, "=");
        cctree_t2s(parser, buf, it->init_decl_name.init);
      }
    } break;
    case cctree_t_top:
    {
      ccstr__catf(buf, "(");
      cctree_t2s(parser, buf, tree->top.lhs);
      ccstr__catf(buf, " ? ");
      cctree_t2s(parser, buf, tree->top.mhs);
      ccstr__catf(buf, " : ");
      cctree_t2s(parser, buf, tree->top.rhs);
      ccstr__catf(buf, ")");
    } break;
    case cctree_t_bop:
    {
      ccstr__catf(buf, "(");
      cctree_t2s(parser, buf, tree->bop.lhs);
      ccstr__catf(buf, " ");
      kttc__token2s(& parser->lex, buf, & tree->bop.opr);
      ccstr__catf(buf, " ");
      cctree_t2s(parser, buf, tree->bop.rhs);
      ccstr__catf(buf, ")");
    } break;
    case cctree_t_uop:
    {
      ccstr__catf(buf, "(");
      kttc__token2s(& parser->lex, buf, & tree->uop.opr);
      ccstr__catf(buf, " ");
      cctree_t2s(parser, buf, tree->uop.mhs);
      ccstr__catf(buf, ")");
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