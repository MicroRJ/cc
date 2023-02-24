// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCLEX
#define _CCLEX

// ** Hashing **

#define CCLEX_WITHIN(x,l,r) (((x)>=(l))&&((x)<=(r)))

// Todo: return the hash, check if the identifier is already in the table ...
static ccinle int
ccread_idenlen(const char *s)
{ int l;
  for(l=0; CCLEX_WITHIN(s[l],'a','z') ||
           CCLEX_WITHIN(s[l],'A','Z') ||
           CCLEX_WITHIN(s[l],'0','9') || s[l]=='_'; ++l);
  return l;
}

ccfunc void
ccread_token(ccread_t *l, cctoken_t *token)
{ *token = l->tok;
  memset(&l->tok,sizeof(l->tok),ccnil);
}

ccfunc void ccread_next_token_internal(ccread_t *l);

ccfunc cci32_t
ccread_next_token(ccread_t *l)
{
ccdbenter("next_token");
  do
  { ccread_next_token_internal(l);
  } while(l->tok.bit == cctoken_Kliteral_comment ||
          l->tok.bit == cctoken_Kspace   ||
          l->tok.bit == cctoken_Kendimpl ||
          l->tok.bit == cctoken_Kendexpl );

ccdbleave("next_token");
  return l->tok.bit != cctoken_kEND;
}

// Note:
ccfunc const char *
ccread_identifier(ccread_t *l, const char *str)
{
ccdbenter("identifier");
  int len=ccread_idenlen(str);

  l->tok.bit=cctoken_kLITIDENT;

  cctoken_k *k=cctblset(l->tok_tbl,len,str);
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
ccread_readstr(ccread_t *l, const char *str)
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
ccread_next_token_internal(ccread_t *l)
{
ccdbenter("next_token_internal");

  l->tok_min=l->tok_max;

  l->tok.loc=l->tok_min;

  if(l->tok_max >= l->doc_max)
  { l->tok.bit = cctoken_kEND;
    goto leave;
  }
  switch(* l->tok_max)
  { default:
    { ++ l->tok_max, l->tok.bit = cctoken_kINVALID;
    } break;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    {
      cci32_t b;
      if(l->tok_max[0] == '0')
      { switch (l->tok_max[1])
        { case 'h':
          case 'x':
          { b = 0x10;
            l->tok_max += 2;
          } break;
          case 'd':
          { b = 0x0A;
            l->tok_max += 2;
          } break;
          case 'o':
          { b = 0x08;
            l->tok_max += 2;
          } break;
          case 'b':
          { b = 0x02;
            l->tok_max += 2;
          } break;
          default:
          { b = 0x0A;
          } break;
        }
      } else
      { b = 0x0A;
      }
      ccu64_t u = 0;
      while(l->tok_max < l->doc_max)
      { if((*l->tok_max >= '0' && *l->tok_max <= '9'))
        { u = u * b + 0x00 + *l->tok_max++ - '0';
        } else
        if((*l->tok_max >= 'a' && *l->tok_max <= 'f'))
        { u = u * b + 0x0A + *l->tok_max++ - 'a';
        } else
        if((*l->tok_max >= 'A' && *l->tok_max <= 'F'))
        { u = u * b + 0x0A + *l->tok_max++ - 'A';
        } else
        { break;
        }
      }
      if(*l->tok_max == '.')
      { ++l->tok_max;

        ccf64_t p = 1;
        ccu64_t d = 0;

        for(; l->tok_max < l->doc_max; p *= b)
        { if((*l->tok_max >= '0' && *l->tok_max <= '9'))
          { d = d * b + 0x00 + *l->tok_max++ - '0';
          } else
          if((*l->tok_max >= 'a' && *l->tok_max <= 'f'))
          { d = d * b + 0x0A + *l->tok_max++ - 'a';
          } else
          if((*l->tok_max >= 'A' && *l->tok_max <= 'F'))
          { d = d * b + 0x0A + *l->tok_max++ - 'A';
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
    { l->tok_max = ccread_identifier(l, l->tok_max);
    } break;
    case '"':
    { l->tok_max = ccread_readstr(l, l->tok_max);
    } break;
    case ':':
    { ++ l->tok_max, l->tok.bit = cctoken_Kcolon;
    } break;
    case ',':
    { ++ l->tok_max, l->tok.bit = cctoken_kCMA;
    } break;
    case '(':
    { ++ l->tok_max, l->tok.bit = cctoken_kLPAREN;
    } break;
    case ')':
    { ++ l->tok_max, l->tok.bit = cctoken_kRPAREN;
    } break;
    case '[':
    { ++ l->tok_max, l->tok.bit = cctoken_kLSQUARE;
    } break;
    case ']':
    { ++ l->tok_max, l->tok.bit = cctoken_kRSQUARE;
    } break;
    case '{':
    { ++ l->tok_max, l->tok.bit = cctoken_Klcurly;
    } break;
    case '}':
    { ++ l->tok_max, l->tok.bit = cctoken_Krcurly;
    } break;
    case '~':
    {
      l->tok_max += 1, l->tok.bit = cctoken_Kbitwise_invert;
    } break;
    // .
    // ...
    case '.':
    { if(l->tok_max[1] == '.' && l->tok_max[2] == '.')
      { l->tok_max += 3, l->tok.bit = cctoken_Kliteral_ellipsis;
      } else
      { l->tok_max += 1, l->tok.bit = cctoken_kDOT;
      }
    } break;
    // ^
    // ^=
    case '^':
    {
      if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.bit = cctoken_kBWXOR_EQL;
      } else
      { l->tok_max += 1, l->tok.bit = cctoken_Kbitwise_xor;
      }
    } break;
    // ||
    // |=
    // |
    case '|':
    {
      if(l->tok_max[1]=='|')
      { l->tok_max += 2, l->tok.bit = cctoken_Klogical_or;
      } else
      if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.bit = cctoken_kBWOR_EQL;
      } else
      { l->tok_max += 1, l->tok.bit = cctoken_Kbitwise_or;
      }
    } break;
    // &&
    // &
    // &=
    case '&':
    {
      if(l->tok_max[1]=='&')
      { l->tok_max += 2, l->tok.bit = cctoken_Klogical_and;
      } else
      if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.bit = cctoken_kBWAND_EQL;
      } else
      { l->tok_max += 1, l->tok.bit = cctoken_Kbitwise_and;
      }
    } break;
    // /=
    // /
    case '/':
    {
      if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.bit = cctoken_kDIV_EQL;
      } else
      { l->tok_max += 1, l->tok.bit = cctoken_kDIV;
      }
    } break;
    // *=
    // *
    case '*':
    {
      if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.bit = cctoken_kMUL_EQL;
      } else
      { l->tok_max += 1, l->tok.bit = cctoken_kMUL;
      }
    } break;
    // +=,+
    case '+':
    { if(l->tok_max[1]=='=')
        l->tok_max += 2, l->tok.bit = cctoken_kADD_EQL;
      else
        l->tok_max += 1, l->tok.bit = cctoken_kADD;
    } break;
    // -=,-
    case '-':
    { if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.bit = cctoken_kSUB_EQL;
      } else
      { l->tok_max += 1, l->tok.bit = cctoken_kSUB;
      }
    } break;
    // ==
    // =
    case '=':
    { if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.bit = cctoken_kTEQ;
      } else
      { l->tok_max += 1, l->tok.bit = cctoken_kASSIGN;
      }
    } break;
    // !=
    // !
    case '!':
    { if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.bit = cctoken_kFEQ;
      } else
      { l->tok_max += 2, l->tok.bit = cctoken_kNEG;
      }
    } break;
    // >=
    // >
    case '>':
    {
      if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.bit = cctoken_kGTE;
      } else
      { l->tok_max += 1, l->tok.bit = cctoken_kGTN;
      }
    } break;
    // <=
    // <
    case '<':
    {
      if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.bit = cctoken_kLTE;
      } else
      { l->tok_max += 1, l->tok.bit = cctoken_kLTN;
      }
    } break;
    // %=
    // %
    case '%':
    {
      if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.bit = cctoken_kMOD_EQL;
      } else
      { l->tok_max += 1, l->tok.bit = cctoken_Kmod;
      }
    } break;

    case '?':
    { if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.bit = cctoken_kINVALID;
      } else
      { l->tok_max += 2, l->tok.bit = cctoken_kINVALID;
      }
    } break;

    case '\0':
    { l->tok_max += 1, l->tok.bit = cctoken_kEND;
    } break;

    // NOTE(RJ):
    // ; Handle trailing tokens!
    case  ' ': case '\t': case '\f': case '\v': case '\b':
    { l->tok_max += 1, l->tok.bit = cctoken_Kspace;
    } break;
    case '\r':
    { if(l->tok_max[1] == '\n')
      { l->tok_max += 2, l->tok.bit = cctoken_Kendimpl;
      } else
      { l->tok_max += 1, l->tok.bit = cctoken_Kendimpl;
      }
    } break;
    case '\n':
    { l->tok_max += 1, l->tok.bit = cctoken_Kendimpl;
    } break;
    case ';':
    { l->tok_max += 1, l->tok.bit = cctoken_Kendexpl;
    } break;
  }

  l->tok.term_expl = 0;
  l->tok.term_expl = 0;

ccdbenter("token-trailing");
  while(l->tok_max<l->doc_max)
  { switch(*l->tok_max)
    { case ' ': case '\t': case '\f': case '\v': case '\b':
      { ++ l->tok_max;
      } continue;
      case '\r':
      { if(l->tok_max[1] == '\n')
        { l->tok_max += 2;
        } else
        { l->tok_max += 1;
        }
        l->tok.term_impl ++;
      } continue;
      case '\n':
      { l->tok_max += 1;
        l->tok.term_impl ++;
      } continue;
      case ';':
      { l->tok_max += 1;
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