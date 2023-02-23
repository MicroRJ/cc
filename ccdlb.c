// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCDLB_C
#define _CCDLB_C

// Todo:
ccfunc void
ccdlbdel_(void **ccm)
{
  ccassert(ccm!=0);

  ccdlb_t *dlb=ccdlb(ccdref(ccm));
  if(dlb)
  {
    ccassert(dlb->allocator!=0);

    if(dlb->entries)
    {
      ccentry_t *i,*f,*e;
      ccarrfor(dlb->entries,e)
      { for(i=e->nex;i;)
        { f=i;
          i=i->nex;

          cccall();
          dlb->allocator(0,f);
        }
      }

      ccarrdel(dlb->entries);
    }

    cccall();
    dlb->allocator(0,dlb);
  }

  *ccm=0;
}

ccfunc cci64_t
ccdlb_arradd_(ccdlb_t **ccm, cci64_t rsze, cci64_t csze)
{
ccdbenter("arradd");
  ccdlb_t *dlb=*ccm;

  int rem_rze,rem_add;
  ccallocator_t *allocator;
  cci64_t sze_max, sze_min;

  if(dlb)
  { sze_max=dlb->sze_max;
    sze_min=dlb->sze_min;
    rem_rze=dlb->rem_rze;
    rem_add=dlb->rem_add;
    allocator=dlb->allocator;

    ccassert(!rem_add);
  } else
  { allocator=ccallocator;
    sze_max=0;
    sze_min=0;
    rem_rze=0;
    rem_add=0;
  }

  ccassert(csze<=rsze+sze_max-sze_min);

  if(sze_max<sze_min+rsze)
  {
    ccassert(!rem_rze);

    sze_max<<=1;
    if(sze_max<sze_min+rsze)
      sze_max=sze_min+rsze;

    cccall();
    *ccm=allocator(sizeof(*dlb)+sze_max,dlb);

    if(!dlb)
    { dlb=cccast(ccdlb_t*,*ccm);

      dlb->rem_rze=rem_rze;
      dlb->rem_add=rem_add;
      dlb->allocator=allocator;
      dlb->entries=0;
    } else
        dlb=cccast(ccdlb_t*,*ccm);

    dlb->sze_max=sze_max;
  }

  dlb->sze_min=sze_min+csze;

ccdbleave("arradd");
  return sze_min;
}

ccfunc ccinle cci64_t
ccdlb_arradd(void **ccm, cci64_t isze, cci64_t cres, cci64_t ccom)
{
  ccdlb_t *dlb=ccdlb(*ccm);
  cci64_t res=ccdlb_arradd_(&dlb,isze*cres,isze*ccom);
  *ccm=dlb+1;
  return res/isze;
}

ccfunc cci64_t
ccdlb_stradd(char **ccm, cci64_t cres, cci64_t ccom, const char *cpy)
{
ccdbenter("stradd");
  // Note: use char type instead of void for preemptive type-checking ...
  // Note: assuming that you'll reserve at-least one more byte for the null terminator ...
  ccassert(cres!=0);
  ccassert(ccom!=0);

  cci64_t res=ccdlb_arradd(cccast(void**,ccm),1,cres,ccom);
  char *cur=(char*)*ccm+res;
  memcpy(cur,cpy,cres-1);
  cur[cres-1]=0;

ccdbleave("stradd");
  return res;
}

// Note: I don't know whether this is a good hash or not ...
ccfunc ccinle ccu64_t
cchsh_abc(cci32_t len, const char *key)
{ ccu64_t hsh;
  if(len>0)
    for(hsh=5381;len;hsh+=(hsh<<5)+key[--len]);
  else
    hsh=cccast(ccu64_t,key);
  return hsh;
}

ccfunc void
ccdlb_tblini(ccdlb_t **dlb_, cci32_t isze)
{
  ccdlb_t *dlb=*dlb_;

  // Note: reserve one item, this will initialize the memory ...
  ccdlb_arradd_(&dlb,isze,0);
  ccarrzro(dlb+1);

  // Note: create some entries
  ccarradd(dlb->entries,0xff);
  ccarrzro(dlb->entries);
  ccarrfix(dlb->entries);

  // Note:
  *dlb_=dlb;
}

ccfunc ccentry_t *
ccdlb_tblcat(ccdlb_t **tbl, cci64_t isze, int len, const char *key, ccentry_t *ent)
{
  ccassert(tbl!=0);
  ccassert((*tbl)!=0);

  if(ent->key)
  { ent->nex=ccmalloc_T(ccentry_t);
    ent=ent->nex;
    memset(ent,ccnil,sizeof(*ent));
  }
  ccassert(ent->key==ccnil);
  ccassert(ent->len==ccnil);
  ccassert(ent->val==ccnil);

  // Note: must zero the item
  cci64_t val=ccdlb_arradd_(tbl,isze,isze);
  memset(cccast(char*,ccdref(tbl)+1)+val,ccnil,isze);

  // Todo:
  if(len>0)
    ccstrputN(ent->key,len,cccast(char*,key));
  else
    ent->key=cccast(char*,key);

  ent->len=len;
  ent->val=val;
  return ent;
}

ccfunc ccentry_t *
ccdbl_query(ccdlb_t *tbl, int len, const char *key)
{
  ccassert(key!=0);
  ccassert(len!=0);

  ccu64_t  hsh=cchsh_abc(len,key);
  cci64_t  idx=hsh%ccarrmax(tbl->entries);

  ccentry_t *ent=tbl->entries+idx;

  ccerrset(ccerr_kNON);
  while(ent->key)
  { if((ent->len==len)&&((ent->key==key)||(!memcmp(ent->key,key,len))))
      return ent;
    if(!ent->nex)
      break;
    ent=ent->nex;
  }
  ccerrset(ccerr_kNIT);
  return ent;
}

ccfunc cci64_t
ccdlb_tblget(void **ccm, cci32_t isze, cci32_t len, const char *key)
{
ccdbenter("tblget");

  ccdlb_t *tbl=ccdlb(ccdref(ccm));
  cckeyset(ccnil);

  // Todo: probably return an index that the user can still write to, but it won't affect other items...
  cci64_t val=ccnil;

  if(tbl)
  { ccentry_t *ent=ccdbl_query(tbl,len,key);
    if(ccerrnon())
    { cckeyset(ent->key);
      val=ent->val;
    }
  } else ccerrset(ccerr_kNIT);

ccdbleave("tblget");
  return val/isze;
}

ccfunc cci64_t
ccdlb_tblput(void **ccm, cci32_t isze, cci32_t len, const char *key)
{
ccdbenter("tblput");

  ccdlb_t *tbl=ccdlb(ccdref(ccm));

  if(!tbl)
    ccdlb_tblini(&tbl,isze);

  cckeyset(ccnil);

  ccentry_t *ent=ccdbl_query(tbl,len,key);

  if(ccerrnit())
  { ccerrset(ccerr_kNON);

    ent=ccdlb_tblcat(&tbl,isze,len,key,ent);
    cckeyset(ent->key);

    *ccm=tbl+1;
  } else
    ccerrset(ccerr_kAIT);

ccdbleave("tblput");
  return ent->val/isze;
}

ccfunc cci64_t
ccdlb_tblset(void **ccm, cci32_t isze, cci32_t len, const char *key)
{
ccdbenter("tblset");

  ccdlb_t *tbl=ccdlb(ccdref(ccm));

  if(!tbl) ccdlb_tblini(&tbl,isze);

  cckeyset(ccnil);

  // Todo: probably return an index that the user can still write to, but it won't affect other items...
  cci64_t val=ccnil;
  ccentry_t *ent=ccdbl_query(tbl,len,key);

  if(ccerrnit())
  { ccerrset(ccerr_kNON);
    ent=ccdlb_tblcat(&tbl,isze,len,key,ent);
    *ccm=tbl+1;
  }

  val=ent->val;

  cckeyset(ent->key);

ccdbleave("tblset");
  return val/isze;
}

ccfunc cci64_t
ccstr_vcatf(char **ccm, const char *fmt, va_list vli)
{ int len=stbsp_vsnprintf(0,0,fmt,vli);
  cci64_t res=ccdlb_arradd(cccast(void**,ccm),1,len+1,len);
  len=stbsp_vsnprintf((char*)*ccm+res,len+1,fmt,vli);
  return res;
}

ccfunc cci64_t
ccstr_catf(char **ccm, const char *fmt, ...)
{ va_list vli;
  va_start(vli,fmt);
  cci64_t res=ccstr_vcatf(ccm,fmt,vli);
  va_end(vli);
  return res;
}

ccfunc void
ccdlb_test()
{
#ifdef _DEBUG
#if 0
  void *mem=ccnil;
  mem=ccrealloc(mem,24);
  ccfree(mem);

  mem=ccmalloc(24);
  mem=ccrealloc(mem,48);
  ccfree(mem);
#endif

#if 0
  typedef struct block_t
  { char a[13];
  } block_t;
  block_t *b=ccmalloc_T(block_t);
  cci64_t  d=cccast(ccu64_t,b);
  cci64_t  x=d/sizeof(*b);
  block_t *r=b+-x;
  (void)r;
#endif

#if 0
  typedef struct blocka_t
  { char s[0x10];
  } blocka_t;
  typedef struct blockb_t
  { char s[0x10];
  } blockb_t;
  char mem[sizeof(blockb_t)];
  blocka_t *bal=(blocka_t *)mem;
  blockb_t *bbl=(blockb_t *)mem;
  memcpy(bal->s,"hello!",sizeof("!hello"));
  memcpy(bbl->s,"!hello",sizeof("hello!"));
#endif

  typedef struct simple_t
  { char *s;
  } simple_t;
  simple_t *simple_arr=ccnil;
  for(int i=0;i<='z'-'a';++i)
  { char *key,*res;
    simple_t *item=ccarradd(simple_arr,1);
    memset(item,ccnil,sizeof(*item));
    key=ccformat("%c%i",'a'+i,i);
    res=ccstrputS(item->s,key);
    ccassert(strcmp(res,key)==0);
  }
  for(int i=0;i<='z'-'a';++i)
  { char *key,*res;
    simple_t *item=ccarradd(simple_arr,1);
    memset(item,ccnil,sizeof(*item));
    key=ccformat("%c%i",'A'+i,i);
    res=ccstrputS(item->s,key);
    ccassert(strcmp(res,key)==0);
  }

  simple_t *string_map=ccnil;

  for(int i=0;i<='z'-'a';++i)
  { char *key;
    simple_t *res;

    // Put:
    key=ccformat("%c%i",'a'+i,i);
    res=cctblputS(string_map,key);
    ccassert(ccerrnon());
    ccstrputS(res->s,key);

    key=ccformat("%c%i",'A'+i,i);
    res=cctblputS(string_map,key);
    ccassert(ccerrnon());
    ccstrputS(res->s,key);

    // Set:
    key=ccformat("%c%i",'a'+i,i);
    res=cctblsetS(string_map,key);
    ccassert(ccerrnon());
    ccassert(strcmp(key,res->s)==0);

    key=ccformat("%c%i",'A'+i,i);
    res=cctblsetS(string_map,key);
    ccassert(ccerrnon());
    ccassert(strcmp(key,res->s)==0);

    // Get:
    key=ccformat("%c%i",'a'+i,i);
    res=cctblgetS(string_map,key);
    ccassert(ccerrnon());
    ccassert(strcmp(key,res->s)==0);

    key=ccformat("%c%i",'A'+i,i);
    res=cctblgetS(string_map,key);
    ccassert(ccerrnon());
    ccassert(strcmp(key,res->s)==0);
  }

  simple_t *pointer_map=ccnil;

  simple_t *item;
  ccarrfor(simple_arr,item)
  {
    char *key=item->s;

    simple_t *res;

    res=cctblgetS(string_map,key);
    ccassert(ccerrnon());
    ccassert(strcmp(key,res->s)==0);

    res=cctblputP(pointer_map,item);
    ccassert(ccerrnon());
    ccstrputS(res->s,key);
  }

  ccarrfor(simple_arr,item)
  { char *key=item->s;
    simple_t *res;
    res=cctblgetP(pointer_map,item);
    ccassert(ccerrnon());
    ccassert(strcmp(key,res->s)==0);
  }
#endif
}

#endif