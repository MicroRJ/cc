// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCLEX
#define _CCLEX

#ifndef ccrune_letter
# define ccrune_letter(r) (CCWITHIN(r,'a','z')||CCWITHIN(r,'A','Z')||CCWITHIN(r,'0','9')||(r)=='_')
#endif

// Note: #ccread_register expects a null terminated identifier string
ccfunc ccinle void
ccread_register(ccread_t *reader, char *s, cctoken_k k)
{
  ccu64_t h=5381;

  int c,n;
  for(n=0;c=s[n];++n)
  { ccassert(ccrune_letter(c));
    if(c!=0)
      h=h<<5,h=h+c;
    else
      break;
  }

  cctoken_entry_t *entry=cctblputP(reader->tok_tbl,h);
  entry->kind=k;
  entry->name=s;
}

// Note: #ccread_token_identifier has to match #ccread_register's hashing function
ccfunc char *
ccread_token_identifier(ccread_t *reader, char *s)
{
  ccu64_t h=5381;

  int n,c;
  for(n=0;c=s[n];++n)
  { if(ccrune_letter(c))
      h=h<<5,h=h+c;
    else
      break;
  }

  cctoken_entry_t *e=cctblsetP(reader->tok_tbl,h);

  int   kind;
  char *name;
  if(e->name)
  { kind=e->kind;
    name=e->name;
  } else
  { kind=e->kind=cctoken_kLITIDENT;
    name=e->name=ccmalloc(n+1);

    memcpy(name,s,n);
    name[n]=0;
  }

  reader->tok.kind=kind;
  reader->tok.name=name;

  return s+n;
}

// Todo: speed!
ccfunc char *
ccread_string(ccread_t *l, char *str)
{
  // Todo: re-use this buffer ...
  // Todo: replace this with a legit string arena ... nothing too fancy ...
  l->tok.kind=cctoken_kLITSTR_INVALID;
  l->tok.name=ccnull;

  char end=*str++;

  char *cur;
  unsigned int res,com;
  for(res=0x20,com=0;cur=ccstraddN(l->tok.name,res,0);res+=0x20)
  {
    for(;com<res;++com)
    {
      if(*str==end)
      { * cur++=0;
          str++;
          com++; // Note: account for the null terminator

        l->tok.kind=cctoken_kLITSTR;
        ccstraddN(l->tok.name,0,com);
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
  return str;
}


ccfunc ccinle char *
ccread_blank(char *r, char *e)
{ while(r<e)
  {
    switch(*r)
    { case  ' ':
      case '\t':
      case '\f':
      case '\v':
      case '\b':
      case '\r':
      case '\n':  r++;
        continue;
    }
    break;
  }
  return r;
}

// Todo: speed!
ccfunc void
ccread_token(ccread_t *l)
{
  l->tok_min=l->tok_max;

  // Todo:
  l->tok.loca=(char*)l->tok_min;

  if(l->tok_max >= l->doc_max)
  { l->tok.kind = cctoken_kEND;
    goto leave;
  }
  switch(* l->tok_max)
  { default:
    { ++ l->tok_max, l->tok.kind = cctoken_kINVALID;
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

        // Todo: should we instead store the two parts?
        l->tok.kind=cctoken_kLITFLO;
        l->tok.real=u+d/p;
      } else
      { l->tok.kind=cctoken_kLITINT;
        l->tok.name=(char*)u;
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
    {
      l->tok_max = ccread_token_identifier(l, l->tok_max);
    } break;
    case '"':
    { l->tok_max = ccread_string(l, l->tok_max);
    } break;
    case ':':
    { ++ l->tok_max, l->tok.kind = cctoken_kCOLON;
    } break;
    case ',':
    { ++ l->tok_max, l->tok.kind = cctoken_kCMA;
    } break;
    case '(':
    { ++ l->tok_max, l->tok.kind = cctoken_kLPAREN;
    } break;
    case ')':
    { ++ l->tok_max, l->tok.kind = cctoken_kRPAREN;
    } break;
    case '[':
    { ++ l->tok_max, l->tok.kind = cctoken_kLSQUARE;
    } break;
    case ']':
    { ++ l->tok_max, l->tok.kind = cctoken_kRSQUARE;
    } break;
    case '{':
    { ++ l->tok_max, l->tok.kind = cctoken_kLCURLY;
    } break;
    case '}':
    { ++ l->tok_max, l->tok.kind = cctoken_kRCURLY;
    } break;
    case '~':
    {
      l->tok_max += 1, l->tok.kind = cctoken_kBWINV;
    } break;
    // .
    // ...
    case '.':
    { if(l->tok_max[1] == '.' && l->tok_max[2] == '.')
      { l->tok_max += 3, l->tok.kind = cctoken_Kliteral_ellipsis;
      } else
      { l->tok_max += 1, l->tok.kind = cctoken_kDOT;
      }
    } break;
    // ^
    // ^=
    case '^':
    {
      if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.kind = cctoken_kBWXOR_EQL;
      } else
      { l->tok_max += 1, l->tok.kind = cctoken_kBWXOR;
      }
    } break;
    // ||
    // |=
    // |
    case '|':
    {
      if(l->tok_max[1]=='|')
      { l->tok_max += 2, l->tok.kind = cctoken_kLGOR;
      } else
      if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.kind = cctoken_kBWOR_EQL;
      } else
      { l->tok_max += 1, l->tok.kind = cctoken_kBWOR;
      }
    } break;
    // &&
    // &
    // &=
    case '&':
    {
      if(l->tok_max[1]=='&')
      { l->tok_max += 2, l->tok.kind = cctoken_kLGAND;
      } else
      if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.kind = cctoken_kBWAND_EQL;
      } else
      { l->tok_max += 1, l->tok.kind = cctoken_kBWAND;
      }
    } break;
    // /=
    // /
    case '/':
    {
      if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.kind = cctoken_kDIV_EQL;
      } else
      { l->tok_max += 1, l->tok.kind = cctoken_kDIV;
      }
    } break;
    // *=
    // *
    case '*':
    {
      if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.kind = cctoken_kMUL_EQL;
      } else
      { l->tok_max += 1, l->tok.kind = cctoken_kMUL;
      }
    } break;
    // +=,+
    case '+':
    { if(l->tok_max[1]=='=')
        l->tok_max += 2, l->tok.kind = cctoken_kADD_EQL;
      else
        l->tok_max += 1, l->tok.kind = cctoken_kADD;
    } break;
    // -=,-
    case '-':
    { if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.kind = cctoken_kSUB_EQL;
      } else
      { l->tok_max += 1, l->tok.kind = cctoken_kSUB;
      }
    } break;
    // ==
    // =
    case '=':
    { if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.kind = cctoken_kTEQ;
      } else
      { l->tok_max += 1, l->tok.kind = cctoken_kASSIGN;
      }
    } break;
    // !=
    // !
    case '!':
    { if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.kind = cctoken_kFEQ;
      } else
      { l->tok_max += 2, l->tok.kind = cctoken_kLGNEG;
      }
    } break;
    // >=
    // >
    case '>':
    {
      if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.kind = cctoken_kGTE;
      } else
      { l->tok_max += 1, l->tok.kind = cctoken_kGTN;
      }
    } break;
    // <=
    // <
    case '<':
    {
      if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.kind = cctoken_kLTE;
      } else
      { l->tok_max += 1, l->tok.kind = cctoken_kLTN;
      }
    } break;
    // %=
    // %
    case '%':
    {
      if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.kind = cctoken_kMOD_EQL;
      } else
      { l->tok_max += 1, l->tok.kind = cctoken_kMOD;
      }
    } break;

    case '?':
    { if(l->tok_max[1]=='=')
      { l->tok_max += 2, l->tok.kind = cctoken_kINVALID;
      } else
      { l->tok_max += 2, l->tok.kind = cctoken_kINVALID;
      }
    } break;
    case '\0':
    { l->tok_max += 1, l->tok.kind = cctoken_kEND;
    } break;
  }

  l->tok.term_expl = 0;
  l->tok.term_expl = 0;

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
    goto leave;
  }

leave:;

}

// Todo: make these keywords global!
ccfunc void
ccread_register_defaults(ccread_t *reader)
{
  ccread_register(reader,"__asm",cctoken_Kmsvc_attr_asm);
  ccread_register(reader,"__based",cctoken_Kmsvc_attr_based);
  ccread_register(reader,"__cdecl",cctoken_Kmsvc_attr_cdecl);
  ccread_register(reader,"__clrcall",cctoken_Kmsvc_attr_clrcall);
  ccread_register(reader,"__fastcall",cctoken_Kmsvc_attr_fastcall);
  ccread_register(reader,"__inline",cctoken_Kmsvc_attr_inline);
  ccread_register(reader,"__stdcall",cctoken_Kmsvc_attr_stdcall);
  ccread_register(reader,"__thiscall",cctoken_Kmsvc_attr_thiscall);
  ccread_register(reader,"__vectorcal",cctoken_Kmsvc_attr_vectorcal);

  ccread_register(reader,"_Alignof",cctoken_Kalign_of);
  ccread_register(reader,"_Alignas",cctoken_Kalign_as);

  ccread_register(reader,"const",cctoken_Kconst);
  ccread_register(reader,"restrict",cctoken_Krestrict);
  ccread_register(reader,"volatile",cctoken_Kvolatile);

  ccread_register(reader,"inline",cctoken_Kinline);
  ccread_register(reader,"_Noreturn",cctoken_Kno_return);

  ccread_register(reader,"signed",cctoken_kSTDC_SIGNED);
  ccread_register(reader,"unsigned",cctoken_kSTDC_UNSIGNED);
  ccread_register(reader,"__int8",cctoken_kMSVC_INT8);
  ccread_register(reader,"__int16",cctoken_kMSVC_INT16);
  ccread_register(reader,"__int32",cctoken_kMSVC_INT32);
  ccread_register(reader,"__int64",cctoken_kMSVC_INT64);
  ccread_register(reader,"double",cctoken_kSTDC_DOUBLE);
  ccread_register(reader,"float",cctoken_kSTDC_FLOAT);
  ccread_register(reader,"long",cctoken_kSTDC_LONG);
  ccread_register(reader,"int",cctoken_kSTDC_INT);
  ccread_register(reader,"short",cctoken_kSTDC_SHORT);
  ccread_register(reader,"char",cctoken_kSTDC_CHAR);
  ccread_register(reader,"void",cctoken_kVOID);
  ccread_register(reader,"_Bool",cctoken_kSTDC_BOOL);

  // ccread_register(reader,"_Complex",cctoken_Kcomplex);
  // ccread_register(reader,"_Atomic",cctoken_Katomic);

  ccread_register(reader,"enum",cctoken_kENUM);
  ccread_register(reader,"struct",cctoken_kSTRUCT);

  ccread_register(reader,"typedef",cctoken_Ktypedef);

  ccread_register(reader,"sizeof",cctoken_kSIZEOF);

  ccread_register(reader,"auto",cctoken_Kauto);
  ccread_register(reader,"extern",cctoken_Kextern);
  ccread_register(reader,"register",cctoken_Kregister);
  ccread_register(reader,"static",cctoken_Kstatic);
  ccread_register(reader,"_Thread_local",cctoken_Kthread_local);
  ccread_register(reader,"__declspec",cctoken_Kmsvc_declspec);

  ccread_register(reader,"if",cctoken_kIF);
  ccread_register(reader,"switch",cctoken_Kswitch);
  ccread_register(reader,"else",cctoken_kELSE);
  ccread_register(reader,"case",cctoken_Kcase);
  ccread_register(reader,"default",cctoken_Kdefault);
  ccread_register(reader,"for",cctoken_Kfor);
  ccread_register(reader,"while",cctoken_kWHILE);
  ccread_register(reader,"do",cctoken_Kdo);
  ccread_register(reader,"goto",cctoken_kGOTO);
  ccread_register(reader,"return",cctoken_kRETURN);
  ccread_register(reader,"break",cctoken_Kbreak);
  ccread_register(reader,"continue",cctoken_Kcontinue);

  ccassert(ccerrnon());
}


#endif