#include "kttc.h"
// #include "gen.ktt.c"

typedef enum
{ kttcc_declspec_signed,
  kttcc_declspec_unsigned,
  kttcc_declspec_host,
  kttcc_declspec_float,
} kttcc_declspec;
typedef enum
{
  kttcc_typekind_nil=0,
  kttcc_typekind_mmx,
  kttcc_typekind_var,
  kttcc_typekind_fix,
  kttcc_typekind_vec,
  kttcc_typekind_ptr,
  kttcc_typekind_ref,
  kttcc_typekind_arr,
} kttcc_typekind;
typedef struct kttcc_type
{ kttcc_typekind typekind;
  kttcc_declspec declspec;
  int            bitlen_max;
  int            bitlen_min;
  int            bitlen;
  int            length;
  kttcc_type *   element;
} kttcc_type;

kttcc_type *gen_type(kttcc_typekind k)
{
  kttcc_type *type=(kttcc_type *)malloc(sizeof(*type));

  type->typekind=k;
  type->element=type;

  return type;
}

kttcc_type *gen_vectype(kttcc_type *t, int length)
{
  kttcc_type *type=gen_type(kttcc_typekind_vec);

  type->element=t;
  type->length=length;

  return type;
}

kttcc_type *gen_ptrtype(kttcc_type *t)
{
  kttcc_type *type=gen_type(kttcc_typekind_ptr);

  type->element=t;

  return type;
}

kttcc_type *gen_vartype(kttcc_declspec d,int i,int a)
{
  kttcc_type *type=gen_type(kttcc_typekind_var);

  type->declspec=d;
  type->bitlen_min=i;
  type->bitlen_max=a;

  return type;
}

kttcc_type *gen_fixtype(kttcc_declspec d,int b)
{
  kttcc_type *type=gen_type(kttcc_typekind_fix);

  type->declspec=d;
  type->bitlen=b;

  return type;
}

static const char  vn[]={'r','a','b','c','d','e','f','g','h','i'};
static const char  pf[]={'i','u','c','f'};
static const char  fn[]={'x','y','z','w'};
static const char *fm[]={"i","lli","c","f"};

const int gen_typename_ex(kttcc_type *t, char *buf, int len)
{
  if(t->typekind==kttcc_typekind_fix)
  {
    return ccformatex(buf,len,"%c%i",pf[t->declspec],t->bitlen);
  } else
  if(t->typekind==kttcc_typekind_mmx)
  {
    return ccformatex(buf,len,"%c%i_%i",pf[t->declspec],t->bitlen_max,t->bitlen_min);
  } else
  if(t->typekind==kttcc_typekind_vec)
  {
    int p=gen_typename_ex(t->element,buf,len);
    return ccformatex(buf+p,len-p,"x%i",t->length);
  } else
  {
    return 0;
  }
}

const char *gen_typename(kttcc_type *t)
{
  static char buf[0x20];
  gen_typename_ex(t,buf,0x20);
  return buf;
}

void emit_typename(char **out, kttcc_type *t)
{
  ccstr_catf(out,"%s",gen_typename(t));
}

void emit_modifier(char **out, kttcc_typekind modifier)
{
  if(modifier==kttcc_typekind_ptr) ccstr_catf(out,"*");
  if(modifier==kttcc_typekind_ref) ccstr_catf(out,"&");
  if(modifier==kttcc_typekind_arr) ccstr_catf(out,"[]");
}

void emit_vardecl_ex(char **out, kttcc_type *t, kttcc_typekind override_modifier, char name)
{
  ccstr_catf(out,"%s ",gen_typename(t));

  if((override_modifier==kttcc_typekind_ptr) ||
     (override_modifier==kttcc_typekind_ref))
  {
    emit_modifier(out,override_modifier);
  } else
  for(;;)
  { if((t->typekind==kttcc_typekind_ptr) ||
       (t->typekind==kttcc_typekind_ref))
    {
      emit_modifier(out,t->typekind);
    } else break;
  }

  ccstr_catf(out,"%c",name);

  if((override_modifier==kttcc_typekind_arr))
  {
    emit_modifier(out,override_modifier);
  } else
  for(;;)
  {
    if((t->typekind==kttcc_typekind_arr))
    {
      emit_modifier(out,t->typekind);
    } else break;
  }
}

void emit_vardecl(char **out, kttcc_type *t, char name)
{
  emit_vardecl_ex(out,t,kttcc_typekind_nil,name);
}

void emit_fundecl(char **out, kttcc_type *t, int l, kttcc_type *r, char *n)
{
  ccstr_catf(out,"static ");
  if(r)
  { emit_typename(out,r);
  } else
  { ccstr_catf(out,"void");
  }
  ccstr_catf(out,"\r\n%s",n);

  ccstr_catf(out,"(");
  for(int i=0;i<l;++i)
  { if(i) ccstr_catf(out,",");
    emit_vardecl(out,t,fn[i]);
  }
  ccstr_catf(out,")");

}


void genmake(char **out, kttcc_type *t)
{
  emit_fundecl(out,t->element,t->length,t,
    ccformat("%sm",gen_typename(t)));

  ccstr_catf(out,"\r\n");
  ccstr_catf(out,"{ ");

  emit_vardecl(out,t,'r');
  ccstr_catf(out,";");

  for(int i=0;i<t->length;++i)
  { ccstr_catf(out,"\r\n  r.%c=%c;",fn[i],fn[i]);
  }

  ccstr_catf(out,"\r\n  return r;\r\n");
  ccstr_catf(out,"}\r\n");
}

void emit_vecoprari(char **out, kttcc_type *t, const char *opr)
{
  ccstr_catf(out,"static %s operator %s (", gen_typename(t), opr);

  if(strlen(opr)==2 && opr[1]=='=')
  {
    emit_vardecl_ex(out,t,kttcc_typekind_ref,'a');
  } else
  {
    emit_vardecl(out,t,'a');
  }

  ccstr_catf(out,",");
  emit_vardecl(out,t,'b');

  ccstr_catf(out,")\r\n");

  ccstr_catf(out,"{ ");
  emit_vardecl(out,t,'r');
  ccstr_catf(out,";");

  for(int i=0;i<t->length;++i)
  { ccstr_catf(out,"\r\n  r.%c=a.%c%cb.%c;",fn[i],fn[i],opr[0],fn[i]);
  }
  ccstr_catf(out,"\r\n  return r;\r\n");
  ccstr_catf(out,"}\r\n");
}

#if 0
void gentostring(char **out, kttcc_type *t)
{
  ccstr_catf(out,"static const char *");
  ccstr_catf(out,"\r\n");
  emit_typename(out,t);
  ccstr_catf(out,"tos(");
  emit_vardecl(out,t,'a');
  ccstr_catf(out,")\r\n");
  ccstr_catf(out,"{ return ccformat(\"");
  for(int i=0;i<cc;++i)
  { ccstr_catf(out,".%c:%%i",fn[i]);
  }
  ccstr_catf(out,"\"");

  for(int i=0;i<cc;++i)
  { ccstr_catf(out,",a.%c",fn[i]);
  }
  ccstr_catf(out,");");

  ccstr_catf(out,"\r\n}\r\n");
}
void genoperdot(char **out, kttcc_type t,int bl,int cc)
{
  ccstr_catf(out,"static ");
  emit_typename(out,t);
  ccstr_catf(out,"\r\n");
  emit_typename(out,t);
  ccstr_catf(out,"dot(");
  emit_vardecl(out,t,bl,cc,kttcc_typekind_fix,'a');
  ccstr_catf(out,",");
  emit_vardecl(out,t,bl,cc,kttcc_typekind_fix,'b');
  ccstr_catf(out,")\r\n");
  ccstr_catf(out,"{ ");
  ccstr_catf(out,"return ");
  for(int i=0;i<cc;++i)
  { if(i) ccstr_catf(out,"+");
    ccstr_catf(out,"(a.%c*b.%c)",fn[i],fn[i]);
  }
  ccstr_catf(out,";\r\n}\r\n");
}
void genoperrel(char **out, kttcc_type t,int bl,int cc,const char *opr)
{
  ccstr_catf(out,"static i32\r\noperator %s (", opr);
  emit_vardecl(out,t,bl,cc,kttcc_typekind_fix,'a');
  ccstr_catf(out,",");
  emit_vardecl(out,t,bl,cc,kttcc_typekind_fix,'b');
  ccstr_catf(out,")\r\n");
  ccstr_catf(out,"{ ");

  ccstr_catf(out,"return ");

  for(int i=0;i<cc;++i)
  { if(i) ccstr_catf(out,"&&");
    ccstr_catf(out,"(a.%c%sb.%c)",fn[i],opr,fn[i]);
  }
  ccstr_catf(out,";");
  ccstr_catf(out,"\r\n}\r\n");
}

#endif
void emit_vectype(char **out, kttcc_type *t)
{
  if(t->typekind==kttcc_typekind_vec)
  { ccstr_catf(out,"typedef struct ");
    emit_typename(out,t);
    ccstr_catf(out,"\r\n");
    ccstr_catf(out,"{");
    for(int i=0;i<t->length;++i)
    { ccstr_catf(out,"\r\n  ");
      emit_vardecl(out,t->element,fn[i]);
      ccstr_catf(out,";");
    }
    ccstr_catf(out,"\r\n");
    ccstr_catf(out,"} ");
    emit_typename(out,t);
    ccstr_catf(out,";\r\n");
  }

  genmake(out,t);
  emit_vecoprari(out,t,"*");emit_vecoprari(out,t,"*=");
  emit_vecoprari(out,t,"/");emit_vecoprari(out,t,"/=");
  emit_vecoprari(out,t,"+");emit_vecoprari(out,t,"+=");
  emit_vecoprari(out,t,"-");emit_vecoprari(out,t,"-=");
#if 0
  genoperrel(out,t,bl,cc,"==");genoperrel(out,t,bl,cc,"!=");
  genoperrel(out,t,bl,cc,"<=");genoperrel(out,t,bl,cc,">=");
  if(t.declspec==kttcc_declspec_float)
  { genoperdot(out,t,bl,cc);
  }
  gentostring(out,t,bl,cc);
#endif
}

void emit_type(char **out, const kttcc_type *t)
{
  if(t->typekind==kttcc_typekind_var)
  {
    for(int i=t->bitlen_min;i<=t->bitlen_max;i<<=1)
    { kttcc_type *fix;
      fix=gen_fixtype(t->declspec,i);

      emit_vectype(out,gen_vectype(fix,2));
      emit_vectype(out,gen_vectype(fix,3));
      emit_vectype(out,gen_vectype(fix,4));
    }
  }

}

int main(int argc, char **argv)
{
  ++ argv;
  -- argc;

  ccreader_t reader;
  ccreader_init(& reader);
  ccreader_file(& reader, "test.ktt");

  cctree_t *tree = ccread_statement_list(& reader);

  char *strbuf = ktt__nullptr;
  cctree_t2s(&reader, &strbuf, tree);
  ccout(strbuf);
  ccout("\n");

  ccreader_uninit(& reader);

  { char *out = ktt__nullptr;

    ccstrcat(out,
      "#define f64 double\r\n"
      "#define f32 float\r\n"
      "#ifdef _MSC_VER\r\n"
      "# define i64 signed   __int64\r\n"
      "# define u64 unsigned __int64\r\n"
      "# define i32 signed   __int32\r\n"
      "# define u32 unsigned __int32\r\n"
      "# define i16 signed   __int16\r\n"
      "# define u16 unsigned __int16\r\n"
      "# define i8  signed   __int8\r\n"
      "# define u8  unsigned __int8\r\n"
      "#else\r\n"
      "# define i64 signed   long long\r\n"
      "# define u64 unsigned long long\r\n"
      "# define i32 signed   int\r\n"
      "# define u32 unsigned int\r\n"
      "# define i16 signed   short\r\n"
      "# define u16 unsigned short\r\n"
      "# define i8  signed   char\r\n"
      "# define u8  unsigned char\r\n"
      "#endif\r\n"
      "#define c8  char\r\n"
      "#define c16 wchar_t\r\n");

    static const kttcc_type ts[] =
    { {kttcc_typekind_var, kttcc_declspec_float,    64, 32 },
      {kttcc_typekind_var, kttcc_declspec_signed,   64, 8  },
      {kttcc_typekind_var, kttcc_declspec_unsigned, 64, 8  },
      {kttcc_typekind_var, kttcc_declspec_host,      8, 8  },
    };


    for(int i=0;i<ARRAYSIZE(ts);++i)
    { emit_type(&out,&ts[i]);
    }

    void *file=ccopenfile("gen.ktt.c");
    ccpushfile(file,0,ccstrlen(out),out);
    ccclosefile(file);
  }

}