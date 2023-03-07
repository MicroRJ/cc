// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCEMIT_C
#define _CCEMIT_C

// Note: this is just a test file to see how one would go about generating code
// for another programming language...

typedef enum ccemitC_class_k ccemitC_class_k;
typedef enum ccemitC_class_k
{ ccemitC_kSIGNED=0,
  ccemitC_kUNSIGNED=1,
  ccemitC_kHOST=2,
  ccemitC_kFLOAT=3,
  ccemitC_kPOINTER,
  ccemitC_kARRAY,
  ccemitC_kSTRUCT,
  ccemitC_kVECTOR,
  ccemitC_kFUNCTION,
  ccemitC_kOPERATOR,
  ccemitC_kTYPENAME,
} ccemitC_class_k;

typedef enum ccemitC_operator_k ccemitC_operator_k;
typedef enum ccemitC_operator_k
{ ccemitC_kMUL,
  ccemitC_kDIV,
  ccemitC_kADD,
  ccemitC_kSUB,
} ccemitC_operator_k;

typedef struct ccemitC_class_t ccemitC_class_t;
typedef struct ccemitC_class_t
{ ccemitC_class_k           kind;
  int                       size;
  struct ccemitC_class_t *  type;
  struct ccemitC_class_t ** list;
  const char              * name;
  ccemitC_operator_k        sort;
} ccemitC_class_t;


static const char *op[]={"*","/","+","-"};

static const char  pf[]={'i','u','c','f'};
static const char *fm[]={"i","lli","c","f"};
static const char *pn[]={"a","b","c","d"};
static const char *vn[]={"r","a","b","c","d","e"};
static const char *fn[]={"x","y","z","w"};

void ccemitC_decl(char **out, ccemitC_class_t *t, const char *name);

void ccemitC_typename(char **out, ccemitC_class_t *c)
{
  if(c->kind==ccemitC_kFUNCTION)
  { ccassert(c->type!=0);

    ccemitC_typename(out,c->type);
    ccstrcatL(*out,"(");
    for(int i=0;i<ccarrlen(c->list);++i)
    { if(i) ccstrcatL(*out,",");
      ccemitC_typename(out,c->list[i]);
    }
    ccstrcatL(*out,")");
  } else
  if(c->kind==ccemitC_kPOINTER)
  { ccemitC_typename(out,c->type);
    ccstrcatL(*out,"*");
  } else
  if(c->kind==ccemitC_kARRAY)
  { ccemitC_typename(out,c->type);
    ccstrcatF(*out,"[%i]",c->size/c->type->size);
  } else
  if(c->kind==ccemitC_kSTRUCT)
  { ccassert(!"error");
  } else
  if(c->kind==ccemitC_kVECTOR)
  { ccassert(c->type!=0);

    ccstrcatL(*out,"struct {");
    for(int i=0;i<c->size/c->type->size;++i)
    { ccemitC_decl(out,c->type,fn[i]);
      ccstrcatL(*out,";");
    }
    ccstrcatL(*out,"}");
  } else
  if(c->kind==ccemitC_kSIGNED||c->kind==ccemitC_kUNSIGNED||c->kind==ccemitC_kFLOAT)
  { ccstrcatF(*out,"%c%i",pf[c->kind],c->size*8);
  } else
  if(c->kind==ccemitC_kTYPENAME)
  { ccstrcatS(*out,c->name);
  } else
    ccassert(!"error");
}

void ccemitC_decl(char **out, ccemitC_class_t *c, const char *name)
{
  ccassert(name!=0);

  if(c->kind==ccemitC_kFUNCTION)
  { ccassert(c->type!=0);

    ccemitC_typename(out,c->type);

    ccstrcatF(*out," %s",name);

    ccstrcatL(*out,"(");
    for(int i=0;i<ccarrlen(c->list);++i)
    { if(i) ccstrcatL(*out,",");
      ccemitC_decl(out,c->list[i],pn[i]);
    }
    ccstrcatL(*out,")");

  } else
  if(c->kind==ccemitC_kVECTOR)
  { ccassert(c->type!=0);

    ccstrcatF(*out,"struct %s\r\n{",name);
    for(int i=0;i<c->size/c->type->size;++i)
    { ccstrcatL(*out,"\r\n  ");
      ccemitC_decl(out,c->type,fn[i]);
      ccstrcatL(*out,";");
    }
    ccstrcatL(*out,"\r\n}");
  } else
  if(c->kind==ccemitC_kSTRUCT)
  { ccassert(!"error");
  } else
  if(c->kind==ccemitC_kPOINTER)
  { ccstrcatL(*out,"*");
    ccemitC_decl(out,c->type,name);
  } else
  if(c->kind==ccemitC_kARRAY)
  { ccemitC_decl(out,c->type,name);
    ccstrcatF(*out,"[%i]",c->size/c->type->size);
  } else
  if(c->kind==ccemitC_kSIGNED||c->kind==ccemitC_kUNSIGNED||c->kind==ccemitC_kFLOAT)
  { ccstrcatF(*out,"%c%i %s",pf[c->kind],c->size*8, name);
  } else
  if(c->kind==ccemitC_kTYPENAME)
  { ccstrcatF(*out,"%s %s",c->name,name);
  } else
    ccassert(!"error");
}

void ccemitC_typedef(char **out, ccemitC_class_t *c, char *name)
{
  ccstrcatL(*out,"typedef ");
  ccemitC_decl(out,c,name);
  ccstrcatF(*out," %s;\r\n",name);
}

void ccemitC_vector_operator_predecl(char **out, ccemitC_class_t *v, cctoken_k sort, char *name)
{
  ccemitC_class_t **list=ccnull;
  *ccarradd(list,1)=v;
  *ccarradd(list,1)=v;

  ccemitC_class_t c=(ccemitC_class_t){ccemitC_kFUNCTION,2,v,list};
  ccemitC_decl(out,&c,name);
  ccstrcatL(*out,";\r\n");
}

void ccemitC_vector_operator(char **out, ccemitC_class_t *n, cctoken_k sort, char *name)
{
  ccemitC_class_t **list=ccnull;
  *ccarradd(list,1)=n;
  *ccarradd(list,1)=n;

  // operator * (f32x2 a, f32x2 b)
  ccemitC_class_t c=(ccemitC_class_t){ccemitC_kFUNCTION,2,n,list};
  ccemitC_decl(out,&c,name);

  ccstrcatL(*out,"\r\n{");
  ccstrcatL(*out,"\r\n");

  ccemitC_decl(out,n,vn[0]);
  ccstrcatL(*out,";\r\n");

  ccemitC_class_t *v=n->type;
  for(int i=0;i<v->size/v->type->size;++i)
  {
    ccstrcatF(*out,"%s.%s=%s.%s+%s.%s;\r\n",
      vn[0],fn[i], pn[0],fn[i],pn[1],fn[i]);
  }

  ccstrcatF(*out,"return %s;",vn[0]);

  ccstrcatL(*out,"\r\n}");
  ccstrcatL(*out,"\r\n");
}

void ccemitC()
{
  char *out=ccnull;

  ccemitC_class_t classic[]=
  { (ccemitC_class_t){ccemitC_kUNSIGNED,1},
    (ccemitC_class_t){ccemitC_kUNSIGNED,2},
    (ccemitC_class_t){ccemitC_kUNSIGNED,4},
    (ccemitC_class_t){ccemitC_kUNSIGNED,8},
    (ccemitC_class_t){ccemitC_kSIGNED,1},
    (ccemitC_class_t){ccemitC_kSIGNED,2},
    (ccemitC_class_t){ccemitC_kSIGNED,4},
    (ccemitC_class_t){ccemitC_kSIGNED,8},
    (ccemitC_class_t){ccemitC_kFLOAT,4},
    (ccemitC_class_t){ccemitC_kFLOAT,8},
  };

  ccemitC_class_t operator[]=
  { (ccemitC_class_t){ccemitC_kOPERATOR,0,0,0,"mul",ccemitC_kMUL},
    (ccemitC_class_t){ccemitC_kOPERATOR,0,0,0,"div",ccemitC_kDIV},
    (ccemitC_class_t){ccemitC_kOPERATOR,0,0,0,"add",ccemitC_kADD},
    (ccemitC_class_t){ccemitC_kOPERATOR,0,0,0,"sub",ccemitC_kSUB},
  };

  ccarray(ccemitC_class_t) vectors=ccnull;

  for(int i=0; i<ccCarrlenL(classic); ++i)
  {
    ccemitC_class_t *c=classic+i;

    for(int n=2; n<=4; ++n)
    {
      char name[0x10];
      ccformatex(name,sizeof(name),"%c%ix%i",pf[c->kind],c->size*8,n);

      ccemitC_class_t *v=ccmalloc_T(ccemitC_class_t);
      v->kind=ccemitC_kVECTOR;
      v->size=c->size*n;
      v->type=c;

      ccemitC_typedef(&out,v,name);

      ccemitC_class_t t=(ccemitC_class_t){ccemitC_kTYPENAME,0,v,0,name};
      *ccarradd(vectors,1)=t;

      for(int o=0; o<ccCarrlenL(operator); ++o)
      {
        ccemitC_class_t r=operator[o];
        ccemitC_vector_operator_predecl(&out,&t,r.sort,ccformat("%s%s",name,r.name));
      }
    }
  }

  ccemitC_class_t *vector;
  ccarrfor(vectors,vector)
  {
    for(int o=0; o<ccCarrlenL(operator); ++o)
    {
      ccemitC_class_t r=operator[o];
      ccemitC_vector_operator(&out,vector,r.sort,ccformat("%s%s",vector->name,r.name));
    }
  }

  void *file=ccopenfile("code\\generated\\ccvector.c","w");
  ccpushfile(file,0,ccstrlen(out),out);
  ccclosefile(file);
}

#endif