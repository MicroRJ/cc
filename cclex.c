#ifndef KTTC_SOURCE_FILE_READ_TOKEN
#define KTTC_SOURCE_FILE_READ_TOKEN

// ** Hashing **

// Note: I literally know nothing about cryptography ....
ccfunc unsigned int
cclex_hashfunc(int len, const char *str)
{ unsigned int h = 5381;
  while(len) h += (h << 5) + str[-- len];
  return h;
}

ccfunc ktt_i32
cclex_hashex(cclex_t *l, int len, const char *key, unsigned int hash, cctokentype_kind *bit, int *off)
{
  ccassert((l->tbl_max & 0x1) == 0);
  ccassert(bit != ktt__nullptr);
  ccassert(off != ktt__nullptr);

  ccentry_t *slot = l->tbl + (hash & (l->tbl_max - 1));

  int eql;
  eql=0;
  do
  { if(eql=(slot->len==len))
    { const char *m,*k;
      m=slot->key;
      k=key;
      while((k<key+len)&&(eql=(*k++==*m++)));
    }
  } while((!eql) && (slot->nex) && (slot = slot->nex));

  if(! eql)
  { if(slot->key) // <-- otherwise this was the first slot and there wasn't a key set.
      slot=slot->nex=(ccentry_t *)ccmalloc(sizeof(ccentry_t));
    slot->nex = 0;
    slot->key = key;
    slot->len = len;
    slot->bit = * bit;
    slot->off = * off;
    ++ l->tbl_min;
    return false;
  }

  * off = slot->off;
  * bit = slot->bit;

  return true;
}

ccfunc ktt_i32
cclex_hash(cclex_t *l, int len, const char *key, cctokentype_kind *bit, int *off)
{
  return cclex_hashex(l,len,key,cclex_hashfunc(len,key),bit,off);
}

ccfunc void
cclex_hashonly(cclex_t *l, int len, const char *key, cctokentype_kind bit, int off)
{
  if(cclex_hash(l,len,key,&bit,&off)) cctraceerr("invalid key, already in hash table");
}

// I don't like this! Remove!
ccfunc void
cclex_hash_init(cclex_t *lexer)
{
  /**
   * Group: msvc attributes.
   *
   *  ** these are reserved keywords **
   **/
  cclex_hashonly(lexer,cclit("__asm"),cctokentype_msvc_attr_asm,0);
  cclex_hashonly(lexer,cclit("__based"),cctokentype_msvc_attr_based,0);
  cclex_hashonly(lexer,cclit("__cdecl"),cctokentype_msvc_attr_cdecl,0);
  cclex_hashonly(lexer,cclit("__clrcall"),cctokentype_msvc_attr_clrcall,0);
  cclex_hashonly(lexer,cclit("__fastcall"),cctokentype_msvc_attr_fastcall,0);
  cclex_hashonly(lexer,cclit("__inline"),cctokentype_msvc_attr_inline,0);
  cclex_hashonly(lexer,cclit("__stdcall"),cctokentype_msvc_attr_stdcall,0);
  cclex_hashonly(lexer,cclit("__thiscall"),cctokentype_msvc_attr_thiscall,0);
  cclex_hashonly(lexer,cclit("__vectorcal"),cctokentype_msvc_attr_vectorcal,0);
  /**
   * Group: alignment specifiers
   *
   * ** these are reserved keywords **
   **/
  cclex_hashonly(lexer,cclit("_Alignof"),cctokentype_align_of,0);
  cclex_hashonly(lexer,cclit("_Alignas"),cctokentype_align_as,0);
   /**
   * Group: type qualifiers
   *
   * ** these are reserved keywords **
   **/
  cclex_hashonly(lexer,cclit("const"),cctokentype_const,0);
  cclex_hashonly(lexer,cclit("restrict"),cctokentype_restrict,0);
  cclex_hashonly(lexer,cclit("volatile"),cctokentype_volatile,0);
  /**
   * Group: function specifiers.
   *
   * ** these are reseverd keywords **
   **/
  cclex_hashonly(lexer,cclit("inline"),cctokentype_inline,0);
  cclex_hashonly(lexer,cclit("_Noreturn"),cctokentype_no_return,0);
  /**
   * Group: type specifiers.
   *
   * ** these are reseverd keywords **
   **/
  cclex_hashonly(lexer,cclit("signed"),cctokentype_signed,0);
  cclex_hashonly(lexer,cclit("unsigned"),cctokentype_unsigned,0);
  cclex_hashonly(lexer,cclit("__int8"),cctokentype_msvc_int8,0);
  cclex_hashonly(lexer,cclit("__int16"),cctokentype_msvc_int16,0);
  cclex_hashonly(lexer,cclit("__int32"),cctokentype_msvc_int32,0);
  cclex_hashonly(lexer,cclit("__int64"),cctokentype_msvc_int64,0);
  cclex_hashonly(lexer,cclit("double"),cctokentype_double,0);
  cclex_hashonly(lexer,cclit("float"),cctokentype_float,0);
  cclex_hashonly(lexer,cclit("long"),cctokentype_long,0);
  cclex_hashonly(lexer,cclit("int"),cctokentype_int,0);
  cclex_hashonly(lexer,cclit("short"),cctokentype_short,0);
  cclex_hashonly(lexer,cclit("char"),cctokentype_char,0);
  cclex_hashonly(lexer,cclit("void"),cctokentype_void,0);
  cclex_hashonly(lexer,cclit("_Bool"),cctokentype_bool,0);
  cclex_hashonly(lexer,cclit("_Complex"),cctokentype_complex,0);
  cclex_hashonly(lexer,cclit("_Atomic"),cctokentype_atomic,0);
  cclex_hashonly(lexer,cclit("enum"),cctokentype_enum,0);
  cclex_hashonly(lexer,cclit("struct"),cctokentype_struct,0);
  /**
   * Group: type specifier & storage class.
   **/
  cclex_hashonly(lexer,cclit("typedef"),cctokentype_typedef,0);
  /*
   * Group: storage class.
   * ** these are reseverd keywords **
   **/
  cclex_hashonly(lexer,cclit("auto"),cctokentype_auto,0);
  cclex_hashonly(lexer,cclit("extern"),cctokentype_extern,0);
  cclex_hashonly(lexer,cclit("register"),cctokentype_register,0);
  cclex_hashonly(lexer,cclit("static"),cctokentype_static,0);
  cclex_hashonly(lexer,cclit("_Thread_local"),cctokentype_thread_local,0);
  cclex_hashonly(lexer,cclit("__declspec"),cctokentype_msvc_declspec,0);
}

ccfunc void
cclex_move(cclex_t *l, size_t len, const char *bed)
{
  l->doc_max = bed + len;
  l->doc_min = l->min = l->max = bed;
}

ccfunc void
cclex_init(cclex_t *l)
{
  l->tok = {};
  l->buf = {};

  l->tbl_max = 1024; // <-- for now this is fixed.
  l->tbl_min = 0;
  l->tbl     = (ccentry_t *) ccmalloc(sizeof(ccentry_t) * l->tbl_max);
  memset(l->tbl, 0, sizeof(ccentry_t) * l->tbl_max);

  // commit the first 0-16 block to signify an invalid address.
  ccstr_put(l->buf,"invalid string!");

  cclex_hash_init(l);
}

ccfunc void
cclex_uninit(cclex_t *l)
{
  ccarr_del(l->buf);
}

#define CCLEX_WITHIN(x,l,r) (((x)>=(l))&&((x)<=(r)))

static int
cclex_idenlen(const char *s)
{ int l;
  for(l=0; CCLEX_WITHIN(s[l],'a','z') ||
           CCLEX_WITHIN(s[l],'A','Z') ||
           CCLEX_WITHIN(s[l],'0','9') || s[l]=='_'; ++l);
  return l;
}

ccfunc const char *
cclex_tokstr(cclex_t *l, cctok_t *token)
{
  return l->buf + token->str;
}

ccfunc void
cclex_token(cclex_t *l, cctok_t *token)
{ *token = l->tok;
  l->tok = {};
}

ccfunc void cclex_next_token_internal(cclex_t *l);

ccfunc ktt_i32
cclex_next_token(cclex_t *l)
{
  do
  { cclex_next_token_internal(l);
  } while(l->tok.bit == cctokentype_literal_comment ||
          l->tok.bit == cctokentype_space   ||
          l->tok.bit == cctokentype_endimpl ||
          l->tok.bit == cctokentype_endexpl );

  return l->tok.bit > 0;
}

// Note:
ccfunc const char *
cclex_identifier(cclex_t *l, const char *str)
{
  int len=cclex_idenlen(str);

  l->tok.bit=cctokentype_literal_identifier;
  l->tok.str=ccstr_len(l->buf);

  if(!cclex_hash(l,len,str,&l->tok.bit,&l->tok.str))
  { ccstr_putl(l->buf,str,len);
  }
  return str+len;
}

ccfunc const char *
cclex_readstr(cclex_t *l, const char *str)
{
  l->tok.bit=cctokentype_literal_string_unterminated;
  l->tok.str=ccstr_len(l->buf);

  char end=*str++;
  (void)end;

  char *cur;
  unsigned int res,com;
  for(res=0x20,com=0;cur=ccstr_add(l->buf,res,0);res+=0x20)
  {
    for(;com<res;++com)
    {
      if(*str=='"')
      { * cur++=0;
          str++;
          com++; // Note: account for the null terminator

        l->tok.bit=cctokentype_literal_string;
        ccstr_add(l->buf,0,com);
        return str;
      } else
      if(*str=='\\')
      { switch(str[1])
        { case '\\': *cur++='\\'; break;
          case '\'': *cur++='\''; break;
          case '"':  *cur++='"' ; break;
          case 't':  *cur++='\t'; break;
          case 'f':  *cur++='\f'; break;
          case 'n':  *cur++='\n'; break;
          case 'r':  *cur++='\r'; break;
          // TODO(RJ):
          // ; Octal constants!
          case '0': *cur++='\0'; break;
          // TODO(RJ):
          // ; Hex constants!
          case 'x':
          case 'X': *cur++='\0'; break;
          // TODO(RJ):
          // ; Unicode constants!
          case 'u': *cur++='\0'; break;
          // TODO(RJ):
          // ; Error reporting!
          default:  *cur++='\0'; break;
        }
        str+=2;
      } else *cur++=*str++;
    }
  }
  return str;
}


ccfunc void
cclex_next_token_internal(cclex_t *l)
{
  l->min = l->max;

  // TODO(RJ): TEMPORARY!
  l->tok.doc = l->min;

  if(l->max >= l->doc_max)
  { l->tok.bit = cctokentype_end;
    return;
  }
  switch(* l->max)
  { default:
    { ++ l->max, l->tok.bit = cctokentype_invalid;
    } break;
    // NOTE(RJ):
    // ; Numbers!
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    {
      ktt_i32 b;
      if(l->max[0] == '0')
      { switch (l->max[1])
        { case 'h':
          case 'x':
          { b = 0x10;
            l->max += 2;
          } break;
          case 'd':
          { b = 0x0A;
            l->max += 2;
          } break;
          case 'o':
          { b = 0x08;
            l->max += 2;
          } break;
          case 'b':
          { b = 0x02;
            l->max += 2;
          } break;
          default:
          { b = 0x0A;
          } break;
        }
      } else
      { b = 0x0A;
      }
      ktt_u64_32 u = 0;
      while(l->max < l->doc_max)
      { if((*l->max >= '0' && *l->max <= '9'))
        { u = u * b + 0x00 + *l->max++ - '0';
        } else
        if((*l->max >= 'a' && *l->max <= 'f'))
        { u = u * b + 0x0A + *l->max++ - 'a';
        } else
        if((*l->max >= 'A' && *l->max <= 'F'))
        { u = u * b + 0x0A + *l->max++ - 'A';
        } else
        { break;
        }
      }
      if(*l->max == '.')
      { ++l->max;
        // NOTE(RJ):
        // ; Keep this in a fpu register!
        ktt_f64_32 p = 1;
        ktt_u64_32 d = 0;

        for(; l->max < l->doc_max; p *= b)
        { if((*l->max >= '0' && *l->max <= '9'))
          { d = d * b + 0x00 + *l->max++ - '0';
          } else
          if((*l->max >= 'a' && *l->max <= 'f'))
          { d = d * b + 0x0A + *l->max++ - 'a';
          } else
          if((*l->max >= 'A' && *l->max <= 'F'))
          { d = d * b + 0x0A + *l->max++ - 'A';
          } else
          { break;
          }
        }
        l->tok.bit = cctokentype_literal_float;
        l->tok.flo = u + d / p;
      } else
      { l->tok.bit = cctokentype_literal_integer;
        l->tok.uns = u;
      }
    } break;

    case('A'): case('B'): case('C'): case('D'): case('E'): case('F'):
    case('G'): case('H'): case('I'): case('J'): case('K'): case('L'):
    case('M'): case('N'): case('O'): case('P'): case('Q'): case('R'):
    case('S'): case('T'): case('U'): case('V'): case('W'): case('X'):
    case('Y'): case('Z'):
    case('a'): case('b'): case('c'): case('d'): case('e'): case('f'):
    case('g'): case('h'): case('i'): case('j'): case('k'): case('l'):
    case('m'): case('n'): case('o'): case('p'): case('q'): case('r'):
    case('s'): case('t'): case('u'): case('v'): case('w'): case('x'):
    case('y'): case('z'):
    case('_'):
    { l->max = cclex_identifier(l, l->max);
    } break;
    case '"':
    { l->max = cclex_readstr(l, l->max);
    } break;
    case ':':
    { ++ l->max, l->tok.bit = cctokentype_colon;
    } break;
    case ',':
    { ++ l->max, l->tok.bit = cctokentype_comma;
    } break;
    // case '#':
    // { ++ l->max, l->tok.bit = cctokentype_pound;
    // } break;
    case '(':
    { ++ l->max, l->tok.bit = cctokentype_lparen;
    } break;
    case ')':
    { ++ l->max, l->tok.bit = cctokentype_rparen;
    } break;
    case '[':
    { ++ l->max, l->tok.bit = cctokentype_lsquare;
    } break;
    case ']':
    { ++ l->max, l->tok.bit = cctokentype_rsquare;
    } break;
    case '{':
    { ++ l->max, l->tok.bit = cctokentype_lcurly;
    } break;
    case '}':
    { ++ l->max, l->tok.bit = cctokentype_rcurly;
    } break;
    case '~':
    {
      l->max += 1, l->tok.bit = cctokentype_bitwise_invert;
    } break;
    // .
    // ...
    case '.':
    { if(l->max[1] == '.' && l->max[2] == '.')
      { l->max += 3, l->tok.bit = cctokentype_literal_ellipsis;
      } else
      { l->max += 1, l->tok.bit = cctokentype_mso;
      }
    } break;
    // ^
    // ^=
    case '^':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctokentype_bitwise_xor_eql;
      } else
      { l->max += 1, l->tok.bit = cctokentype_bitwise_xor;
      }
    } break;
    // ||
    // |=
    // |
    case '|':
    {
      if(l->max[1]=='|')
      { l->max += 2, l->tok.bit = cctokentype_logical_or;
      } else
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctokentype_bitwise_or_eql;
      } else
      { l->max += 1, l->tok.bit = cctokentype_bitwise_or;
      }
    } break;
    // &&
    // &
    // &=
    case '&':
    {
      if(l->max[1]=='&')
      { l->max += 2, l->tok.bit = cctokentype_logical_and;
      } else
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctokentype_bitwise_and_eql;
      } else
      { l->max += 1, l->tok.bit = cctokentype_bitwise_and;
      }
    } break;
    // /=
    // /
    case '/':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctokentype_div_eql;
      } else
      { l->max += 1, l->tok.bit = cctokentype_div;
      }
    } break;
    // *=
    // *
    case '*':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctokentype_mul_eql;
      } else
      { l->max += 1, l->tok.bit = cctokentype_mul;
      }
    } break;
    // ++
    // +=
    // +
    case '+':
    {
      // We can't produce these tokens because is up to the parser to interpret what
      // 1++1 means, or what 1++ +1 means, otherwise the parser would have to decompose
      // these tokens, I'd rather it compose them.

      // if(l->max[1]=='+')
      // { l->max += 2, l->tok.bit = cctokentype_increment;
      // } else
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctokentype_add_eql;
      } else
      { l->max += 1, l->tok.bit = cctokentype_add;
      }
    } break;
    // --
    // -=
    // -
    case '-':
    {
      // We can't produce these tokens because is up to the parser to interpret what
      // 1--1 means, or what 1-- -1 means, otherwise the parser would have to decompose
      // these tokens, I'd rather it compose them.

      // if(l->max[1]=='-')
      // { l->max += 2, l->tok.bit = cctokentype_decrement;
      // } else
      if(l->max[1]=='>')
      { l->max += 2, l->tok.bit = cctokentype_msp;
      } else
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctokentype_sub_eql;
      } else
      { l->max += 1, l->tok.bit = cctokentype_sub;
      }
    } break;
    // ==
    // =
    case '=':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctokentype_equals;
      } else
      { l->max += 1, l->tok.bit = cctokentype_assign;
      }
    } break;
    // !=
    // !
    case '!':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctokentype_not_equals;
      } else
      { l->max += 2, l->tok.bit = cctokentype_negate;
      }
    } break;
    // >=
    // >
    case '>':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctokentype_greater_than_eql;
      } else
      { l->max += 1, l->tok.bit = cctokentype_greater_than;
      }
    } break;
    // <=
    // <
    case '<':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctokentype_less_than_eql;
      } else
      { l->max += 1, l->tok.bit = cctokentype_less_than;
      }
    } break;
    // %=
    // %
    case '%':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctokentype_mod_eql;
      } else
      { l->max += 1, l->tok.bit = cctokentype_mod;
      }
    } break;

    case '?':
    { if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctokentype_invalid;
      } else
      { l->max += 2, l->tok.bit = cctokentype_invalid;
      }
    } break;

    case '\0':
    { l->max += 1, l->tok.bit = cctokentype_end;
    } break;

    // NOTE(RJ):
    // ; Handle trailing tokens!
    case  ' ': case '\t': case '\f': case '\v': case '\b':
    { l->max += 1, l->tok.bit = cctokentype_space;
    } break;
    case '\r':
    { if(l->max[1] == '\n')
      { l->max += 2, l->tok.bit = cctokentype_endimpl;
      } else
      { l->max += 1, l->tok.bit = cctokentype_endimpl;
      }
    } break;
    case '\n':
    { l->max += 1, l->tok.bit = cctokentype_endimpl;
    } break;
    case ';':
    { l->max += 1, l->tok.bit = cctokentype_endexpl;
    } break;
  }

  l->tok.term_expl = 0;
  l->tok.term_expl = 0;

  while(l->max < l->doc_max)
  { switch(*l->max)
    { case ' ': case '\t': case '\f': case '\v': case '\b':
      { ++ l->max;
      } continue;
      case '\r':
      { if(l->max[1] == '\n')
        { l->max += 2;
        } else
        { l->max += 1;
        }
        l->tok.term_impl ++;
      } continue;
      case '\n':
      { l->max += 1;
        l->tok.term_impl ++;
      } continue;
      case ';':
      { l->max += 1;
        l->tok.term_expl ++;
      } continue;
      default:
      {
      } return;
    }
  }
} // end of function

#endif