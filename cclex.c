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

ccfunc char *
cclex_hash(cclex_t *l, int len, const char *key, int cpy, int *bit, int *fnd)
{
  ccassert((l->tbl_max&0x1)==0);
  ccassert(bit!=0);

  unsigned int hash=cclex_hashfunc(len,key);
  ccentry_t *slot=l->tbl+(hash&(l->tbl_max-1));

  int eql=0;

  do
  { if(eql=(slot->len==len))
    { const char *m,*k;
      m=slot->key;
      k=key;
      while((k<key+len)&&(eql=(*k++==*m++)));
    }
  } while((!eql) && (slot->nex) && (slot = slot->nex));

  if(fnd) *fnd=eql;

  if(! eql)
  { // Note: this is either the first slot, or the last slot ... If no key, first slot, otherwise, last ...
  	if(slot->key) slot=slot->nex=(ccentry_t *)ccmalloc(sizeof(ccentry_t));

    slot->nex=0;
    slot->key=ccstrnil;
    slot->len=len;
    slot->bit=*bit;

    // Todo: replace this with a legit string arena ...
    if(cpy) ccstrputl((char*)slot->key,len,key);
    else slot->key=(char*)key;

    ++ l->tbl_min;
  } else
  {
	  if(bit) *bit=slot->bit;
  }
  return slot->key;
}

ccfunc void
cclex_hashonly(cclex_t *l, int len, const char *key, cctoken_Kkind bit)
{
	int fnd;
	const char *okey=cclex_hash(l,len,key,ccfalse,(int*)&bit,&fnd);
	ccassert(okey==key);
  if(fnd) cctraceerr("invalid key, already in hash table");
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
  cclex_hashonly(lexer,cclit("__asm"),cctoken_Kmsvc_attr_asm);
  cclex_hashonly(lexer,cclit("__based"),cctoken_Kmsvc_attr_based);
  cclex_hashonly(lexer,cclit("__cdecl"),cctoken_Kmsvc_attr_cdecl);
  cclex_hashonly(lexer,cclit("__clrcall"),cctoken_Kmsvc_attr_clrcall);
  cclex_hashonly(lexer,cclit("__fastcall"),cctoken_Kmsvc_attr_fastcall);
  cclex_hashonly(lexer,cclit("__inline"),cctoken_Kmsvc_attr_inline);
  cclex_hashonly(lexer,cclit("__stdcall"),cctoken_Kmsvc_attr_stdcall);
  cclex_hashonly(lexer,cclit("__thiscall"),cctoken_Kmsvc_attr_thiscall);
  cclex_hashonly(lexer,cclit("__vectorcal"),cctoken_Kmsvc_attr_vectorcal);
  /**
   * Group: alignment specifiers
   *
   * ** these are reserved keywords **
   **/
  cclex_hashonly(lexer,cclit("_Alignof"),cctoken_Kalign_of);
  cclex_hashonly(lexer,cclit("_Alignas"),cctoken_Kalign_as);
   /**
   * Group: type qualifiers
   *
   * ** these are reserved keywords **
   **/
  cclex_hashonly(lexer,cclit("const"),cctoken_Kconst);
  cclex_hashonly(lexer,cclit("restrict"),cctoken_Krestrict);
  cclex_hashonly(lexer,cclit("volatile"),cctoken_Kvolatile);
  /**
   * Group: function specifiers.
   *
   * ** these are reseverd keywords **
   **/
  cclex_hashonly(lexer,cclit("inline"),cctoken_Kinline);
  cclex_hashonly(lexer,cclit("_Noreturn"),cctoken_Kno_return);
  /**
   * Group: type specifiers.
   *
   * ** these are reseverd keywords **
   **/
  cclex_hashonly(lexer,cclit("signed"),cctoken_Ksigned);
  cclex_hashonly(lexer,cclit("unsigned"),cctoken_Kunsigned);
  cclex_hashonly(lexer,cclit("__int8"),cctoken_Kmsvc_int8);
  cclex_hashonly(lexer,cclit("__int16"),cctoken_Kmsvc_int16);
  cclex_hashonly(lexer,cclit("__int32"),cctoken_Kmsvc_int32);
  cclex_hashonly(lexer,cclit("__int64"),cctoken_Kmsvc_int64);
  cclex_hashonly(lexer,cclit("double"),cctoken_Kdouble);
  cclex_hashonly(lexer,cclit("float"),cctoken_Kfloat);
  cclex_hashonly(lexer,cclit("long"),cctoken_Klong);
  cclex_hashonly(lexer,cclit("int"),cctoken_Kint);
  cclex_hashonly(lexer,cclit("short"),cctoken_Kshort);
  cclex_hashonly(lexer,cclit("char"),cctoken_Kchar);
  cclex_hashonly(lexer,cclit("void"),cctoken_Kvoid);
  cclex_hashonly(lexer,cclit("_Bool"),cctoken_Kbool);
  cclex_hashonly(lexer,cclit("_Complex"),cctoken_Kcomplex);
  cclex_hashonly(lexer,cclit("_Atomic"),cctoken_Katomic);
  cclex_hashonly(lexer,cclit("enum"),cctoken_Kenum);
  cclex_hashonly(lexer,cclit("struct"),cctoken_Kstruct);
  /**
   * Group: type specifier & storage class.
   **/
  cclex_hashonly(lexer,cclit("typedef"),cctoken_Ktypedef);
  /*
   * Group: storage class.
   * ** these are reseverd keywords **
   **/
  cclex_hashonly(lexer,cclit("auto"),cctoken_Kauto);
  cclex_hashonly(lexer,cclit("extern"),cctoken_Kextern);
  cclex_hashonly(lexer,cclit("register"),cctoken_Kregister);
  cclex_hashonly(lexer,cclit("static"),cctoken_Kstatic);
  cclex_hashonly(lexer,cclit("_Thread_local"),cctoken_Kthread_local);
  cclex_hashonly(lexer,cclit("__declspec"),cctoken_Kmsvc_declspec);
  /**
   * Group: control statements.
   * ** these are reseverd keywords **
   **/
  cclex_hashonly(lexer,cclit("if"),cctoken_Kif);
  cclex_hashonly(lexer,cclit("switch"),cctoken_Kswitch);
  cclex_hashonly(lexer,cclit("else"),cctoken_Kelse);
  cclex_hashonly(lexer,cclit("case"),cctoken_Kcase);
  cclex_hashonly(lexer,cclit("default"),cctoken_Kdefault);
  cclex_hashonly(lexer,cclit("for"),cctoken_Kfor);
  cclex_hashonly(lexer,cclit("while"),cctoken_Kwhile);
  cclex_hashonly(lexer,cclit("do"),cctoken_Kdo);
  cclex_hashonly(lexer,cclit("goto"),cctoken_Kgoto);
  cclex_hashonly(lexer,cclit("return"),cctoken_Kreturn);
  cclex_hashonly(lexer,cclit("break"),cctoken_Kbreak);
  cclex_hashonly(lexer,cclit("continue"),cctoken_Kcontinue);
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

  l->tbl_max = 1024; // <-- for now this is fixed.
  l->tbl_min = 0;
  l->tbl     = (ccentry_t *) ccmalloc(sizeof(ccentry_t) * l->tbl_max);
  memset(l->tbl, 0, sizeof(ccentry_t) * l->tbl_max);

  cclex_hash_init(l);
}

ccfunc void
cclex_uninit(cclex_t *l)
{
	(void)l;
  // ccarrdel(l->buf);
}

#define CCLEX_WITHIN(x,l,r) (((x)>=(l))&&((x)<=(r)))

// Todo: if would be could if you'd hash at the same time ...
static int
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
  l->tok = {};
}

ccfunc void cclex_next_token_internal(cclex_t *l);

ccfunc ktt_i32
cclex_next_token(cclex_t *l)
{
  do
  { cclex_next_token_internal(l);
  } while(l->tok.bit == cctoken_Kliteral_comment ||
          l->tok.bit == cctoken_Kspace   ||
          l->tok.bit == cctoken_Kendimpl ||
          l->tok.bit == cctoken_Kendexpl );

  return l->tok.bit > 0;
}

// Note:
ccfunc const char *
cclex_identifier(cclex_t *l, const char *str)
{ int len=cclex_idenlen(str);
  l->tok.bit=cctoken_Kliteral_identifier;
  l->tok.str=cclex_hash(l,len,str,cctrue,(int*)&l->tok.bit,ccnil);
  return str+len;
}

ccfunc const char *
cclex_readstr(cclex_t *l, const char *str)
{
	// Todo: re-use this buffer ...
	// Todo: replace this with a legit string arena ... nothing too fancy ...
  l->tok.bit=cctoken_Kliteral_string_unterminated;
  l->tok.str=ccstrnil;

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

        l->tok.bit=cctoken_Kliteral_string;
        ccstradd((char*)l->tok.str,0,com);
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
  { l->tok.bit = cctoken_Kend;
    return;
  }
  switch(* l->max)
  { default:
    { ++ l->max, l->tok.bit = cctoken_Kinvalid;
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
        l->tok.bit = cctoken_Kliteral_float;
        l->tok.flo = u + d / p;
      } else
      { l->tok.bit = cctoken_Kliteral_integer;
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
    { ++ l->max, l->tok.bit = cctoken_Kcolon;
    } break;
    case ',':
    { ++ l->max, l->tok.bit = cctoken_Kcomma;
    } break;
    // case '#':
    // { ++ l->max, l->tok.bit = cctoken_Kpound;
    // } break;
    case '(':
    { ++ l->max, l->tok.bit = cctoken_Klparen;
    } break;
    case ')':
    { ++ l->max, l->tok.bit = cctoken_Krparen;
    } break;
    case '[':
    { ++ l->max, l->tok.bit = cctoken_Klsquare;
    } break;
    case ']':
    { ++ l->max, l->tok.bit = cctoken_Krsquare;
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
      { l->max += 1, l->tok.bit = cctoken_Kmso;
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
      { l->max += 1, l->tok.bit = cctoken_Kdiv;
      }
    } break;
    // *=
    // *
    case '*':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_Kmul_eql;
      } else
      { l->max += 1, l->tok.bit = cctoken_Kmul;
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
      { l->max += 1, l->tok.bit = cctoken_Kadd;
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
      { l->max += 2, l->tok.bit = cctoken_Kmsp;
      } else
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_Ksub_eql;
      } else
      { l->max += 1, l->tok.bit = cctoken_Ksub;
      }
    } break;
    // ==
    // =
    case '=':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_Kequals;
      } else
      { l->max += 1, l->tok.bit = cctoken_Kassign;
      }
    } break;
    // !=
    // !
    case '!':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_Knot_equals;
      } else
      { l->max += 2, l->tok.bit = cctoken_Knegate;
      }
    } break;
    // >=
    // >
    case '>':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_Kgreater_than_eql;
      } else
      { l->max += 1, l->tok.bit = cctoken_Kgreater_than;
      }
    } break;
    // <=
    // <
    case '<':
    {
      if(l->max[1]=='=')
      { l->max += 2, l->tok.bit = cctoken_Kless_than_eql;
      } else
      { l->max += 1, l->tok.bit = cctoken_Kless_than;
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
      { l->max += 2, l->tok.bit = cctoken_Kinvalid;
      } else
      { l->max += 2, l->tok.bit = cctoken_Kinvalid;
      }
    } break;

    case '\0':
    { l->max += 1, l->tok.bit = cctoken_Kend;
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