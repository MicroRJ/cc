// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCLEX
#define _CCLEX

// ** Hashing **

#define CCLEX_WITHIN(x,l,r) (((x)>=(l))&&((x)<=(r)))

// Todo: this is not good ...
ccfunc void
cclex_hash_init(cclex_t *lexer)
{
  *cctblputL(lexer->tbl,"__asm")=cctoken_Kmsvc_attr_asm; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"__based")=cctoken_Kmsvc_attr_based; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"__cdecl")=cctoken_Kmsvc_attr_cdecl; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"__clrcall")=cctoken_Kmsvc_attr_clrcall; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"__fastcall")=cctoken_Kmsvc_attr_fastcall; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"__inline")=cctoken_Kmsvc_attr_inline; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"__stdcall")=cctoken_Kmsvc_attr_stdcall; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"__thiscall")=cctoken_Kmsvc_attr_thiscall; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"__vectorcal")=cctoken_Kmsvc_attr_vectorcal; ccassert(ccerrnon());

  *cctblputL(lexer->tbl,"_Alignof")=cctoken_Kalign_of; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"_Alignas")=cctoken_Kalign_as; ccassert(ccerrnon());

  *cctblputL(lexer->tbl,"const")=cctoken_Kconst; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"restrict")=cctoken_Krestrict; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"volatile")=cctoken_Kvolatile; ccassert(ccerrnon());

  *cctblputL(lexer->tbl,"inline")=cctoken_Kinline; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"_Noreturn")=cctoken_Kno_return; ccassert(ccerrnon());

  *cctblputL(lexer->tbl,"signed")=cctoken_Ksigned; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"unsigned")=cctoken_Kunsigned; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"__int8")=cctoken_Kmsvc_int8; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"__int16")=cctoken_Kmsvc_int16; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"__int32")=cctoken_Kmsvc_int32; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"__int64")=cctoken_Kmsvc_int64; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"double")=cctoken_Kdouble; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"float")=cctoken_Kfloat; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"long")=cctoken_Klong; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"int")=cctoken_Kint; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"short")=cctoken_Kshort; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"char")=cctoken_Kchar; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"void")=cctoken_Kvoid; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"_Bool")=cctoken_Kbool; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"_Complex")=cctoken_Kcomplex; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"_Atomic")=cctoken_Katomic; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"enum")=cctoken_Kenum; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"struct")=cctoken_kSTRUCT; ccassert(ccerrnon());

  *cctblputL(lexer->tbl,"typedef")=cctoken_Ktypedef; ccassert(ccerrnon());

  *cctblputL(lexer->tbl,"auto")=cctoken_Kauto; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"extern")=cctoken_Kextern; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"register")=cctoken_Kregister; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"static")=cctoken_Kstatic; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"_Thread_local")=cctoken_Kthread_local; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"__declspec")=cctoken_Kmsvc_declspec; ccassert(ccerrnon());

  *cctblputL(lexer->tbl,"if")=cctoken_Kif; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"switch")=cctoken_Kswitch; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"else")=cctoken_Kelse; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"case")=cctoken_Kcase; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"default")=cctoken_Kdefault; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"for")=cctoken_Kfor; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"while")=cctoken_Kwhile; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"do")=cctoken_Kdo; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"goto")=cctoken_Kgoto; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"return")=cctoken_Kreturn; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"break")=cctoken_Kbreak; ccassert(ccerrnon());
  *cctblputL(lexer->tbl,"continue")=cctoken_Kcontinue; ccassert(ccerrnon());
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
  memset(&l->tok,sizeof(l->tok),ccnil);
  cclex_hash_init(l);
}

ccfunc void
cclex_uninit(cclex_t *l)
{
	(void)l;
  // ccarrdel(l->buf);
}

// Todo: return the hash, check if the identifier is already in the table ...
static ccinle int
cclex_idenlen(const char *s)
{ int l;
  for(l=0; CCLEX_WITHIN(s[l],'a','z') ||
           CCLEX_WITHIN(s[l],'A','Z') ||
           CCLEX_WITHIN(s[l],'0','9') || s[l]=='_'; ++l);
  return l;
}

ccfunc void
cclex_token(cclex_t *l, cctoken_t *token)
{ *token = l->tok;
  memset(&l->tok,sizeof(l->tok),ccnil);
}

ccfunc void cclex_next_token_internal(cclex_t *l);

ccfunc cci32_t
cclex_next_token(cclex_t *l)
{
ccdbenter("next_token");
  do
  { cclex_next_token_internal(l);
  } while(l->tok.bit == cctoken_Kliteral_comment ||
          l->tok.bit == cctoken_Kspace   ||
          l->tok.bit == cctoken_Kendimpl ||
          l->tok.bit == cctoken_Kendexpl );

ccdbleave("next_token");
  return l->tok.bit != cctoken_kEND;
}

// Note:
ccfunc const char *
cclex_identifier(cclex_t *l, const char *str)
{
ccdbenter("identifier");
	int len=cclex_idenlen(str);

	l->tok.bit=cctoken_kLITIDENT;

  cctoken_k *k=cctblset(l->tbl,len,str);
  if(*k!=cctoken_kINVALID)
  { l->tok.bit=*k;
  } else
  { *k=l->tok.bit;
  }

  l->tok.str=cckeyget();

ccdbleave("identifier");
  return str+len;
}

ccfunc const char *
cclex_readstr(cclex_t *l, const char *str)
{
ccdbenter("string_token");

	// Todo: re-use this buffer ...
	// Todo: replace this with a legit string arena ... nothing too fancy ...
  l->tok.bit=cctoken_kLITSTR_INVALID;
  l->tok.str=ccnil;

  char end=*str++;

  char *cur;
  unsigned int res,com;
  for(res=0x20,com=0;cur=ccstradd((char*)l->tok.str,res,0);res+=0x20)
  {
    for(;com<res;++com)
    {
      if(*str==end)
      { * cur++=0;
          str++;
          com++; // Note: account for the null terminator

        l->tok.bit=cctoken_kLITSTR;
        ccstradd((char*)l->tok.str,0,com);
        goto leave;
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
leave:
ccdbleave("string_token");
  return str;
}


ccfunc void
cclex_next_token_internal(cclex_t *l)
{
ccdbenter("next_token_internal");
  l->min = l->max;

  l->tok.loc=l->min;

  if(l->max >= l->doc_max)
  { l->tok.bit = cctoken_kEND;
    goto leave;
  }
  switch(* l->max)
  { default:
    { ++ l->max, l->tok.bit = cctoken_kINVALID;
    } break;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    {
      cci32_t b;
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
      ccu64_t u = 0;
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
        ccf64_t p = 1;
        ccu64_t d = 0;

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
        l->tok.bit   = cctoken_kLITFLO;
        l->tok.asf64 = u + d / p;
      } else
      { l->tok.bit = cctoken_kLITINT;
        l->tok.asu64 = u;
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
    { ++ l->max, l->tok.bit = cctoken_Kcolon;
    } break;
    case ',':
    { ++ l->max, l->tok.bit = cctoken_kCMA;
    } break;
    case '(':
    { ++ l->max, l->tok.bit = cctoken_kLPAREN;
    } break;
    case ')':
    { ++ l->max, l->tok.bit = cctoken_kRPAREN;
    } break;
    case '[':
    { ++ l->max, l->tok.bit = cctoken_kLSQUARE;
    } break;
    case ']':
    { ++ l->max, l->tok.bit = cctoken_kRSQUARE;
    } break;
    case '{':
    { ++ l->max, l->tok.bit = cctoken_Klcurly;
    } break;
    case '}':
    { ++ l->max, l->tok.bit = cctoken_Krcurly;
    } break;
    case '~':
    {
      l->max += 1, l->tok.bit = cctoken_Kbitwise_invert;
    } break;
    // .
    // ...
    case '.':
    { if(l->max[1] == '.' && l->max[2] == '.')
      { l->max += 3, l->tok.bit = cctoken_Kliteral_ellipsis;
      } else
      { l->max += 1, l->tok.bit = cctoken_kDOT;
      }
    } break;
    // ^
    // ^=
    case '^':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_Kbitwise_xor_eql;
      } else
      { l->max += 1, l->tok.bit = cctoken_Kbitwise_xor;
      }
    } break;
    // ||
    // |=
    // |
    case '|':
    {
      if(l->max[1]=='|')
      { l->max += 2, l->tok.bit = cctoken_Klogical_or;
      } else
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_Kbitwise_or_eql;
      } else
      { l->max += 1, l->tok.bit = cctoken_Kbitwise_or;
      }
    } break;
    // &&
    // &
    // &=
    case '&':
    {
      if(l->max[1]=='&')
      { l->max += 2, l->tok.bit = cctoken_Klogical_and;
      } else
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_Kbitwise_and_eql;
      } else
      { l->max += 1, l->tok.bit = cctoken_Kbitwise_and;
      }
    } break;
    // /=
    // /
    case '/':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_Kdiv_eql;
      } else
      { l->max += 1, l->tok.bit = cctoken_kDIV;
      }
    } break;
    // *=
    // *
    case '*':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_Kmul_eql;
      } else
      { l->max += 1, l->tok.bit = cctoken_kMUL;
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
      // { l->max += 2, l->tok.bit = cctoken_Kincrement;
      // } else
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_Kadd_eql;
      } else
      { l->max += 1, l->tok.bit = cctoken_kADD;
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
      // { l->max += 2, l->tok.bit = cctoken_Kdecrement;
      // } else
      if(l->max[1]=='>')
      { l->max += 2, l->tok.bit = cctoken_kARROW;
      } else
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_Ksub_eql;
      } else
      { l->max += 1, l->tok.bit = cctoken_kSUB;
      }
    } break;
    // ==
    // =
    case '=':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_kTEQ;
      } else
      { l->max += 1, l->tok.bit = cctoken_kASSIGN;
      }
    } break;
    // !=
    // !
    case '!':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_kFEQ;
      } else
      { l->max += 2, l->tok.bit = cctoken_Knegate;
      }
    } break;
    // >=
    // >
    case '>':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_kGTE;
      } else
      { l->max += 1, l->tok.bit = cctoken_kGTN;
      }
    } break;
    // <=
    // <
    case '<':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_kLTE;
      } else
      { l->max += 1, l->tok.bit = cctoken_kLTN;
      }
    } break;
    // %=
    // %
    case '%':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_Kmod_eql;
      } else
      { l->max += 1, l->tok.bit = cctoken_Kmod;
      }
    } break;

    case '?':
    { if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_kINVALID;
      } else
      { l->max += 2, l->tok.bit = cctoken_kINVALID;
      }
    } break;

    case '\0':
    { l->max += 1, l->tok.bit = cctoken_kEND;
    } break;

    // NOTE(RJ):
    // ; Handle trailing tokens!
    case  ' ': case '\t': case '\f': case '\v': case '\b':
    { l->max += 1, l->tok.bit = cctoken_Kspace;
    } break;
    case '\r':
    { if(l->max[1] == '\n')
      { l->max += 2, l->tok.bit = cctoken_Kendimpl;
      } else
      { l->max += 1, l->tok.bit = cctoken_Kendimpl;
      }
    } break;
    case '\n':
    { l->max += 1, l->tok.bit = cctoken_Kendimpl;
    } break;
    case ';':
    { l->max += 1, l->tok.bit = cctoken_Kendexpl;
    } break;
  }

  l->tok.term_expl = 0;
  l->tok.term_expl = 0;

ccdbenter("token-trailing");
  while(l->max<l->doc_max)
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
    }
    goto leave_token_trailing;
  }

leave_token_trailing:
ccdbleave("token-trailing");

leave:
ccdbleave("next_token_internal");
}

#endif