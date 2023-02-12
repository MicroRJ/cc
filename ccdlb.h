#ifndef _CCDLB
#define _CCDLB

#ifndef ccdlb_
# define ccdlb_(ccm) (cccast(ccdlb_t*,ccm)-1)
#endif
#ifndef ccdlb
# define ccdlb(ccm) ((ccm)?ccdlb_(ccm):(cccast(ccdlb_t*,ccnil)))
#endif

#ifndef ccdlbmax
# define ccdlbmax(ccm) ((ccm)?cccast(size_t *,ccm)[-2]:0)
#endif
#ifndef ccdlbmin
# define ccdlbmin(ccm) ((ccm)?cccast(size_t *,ccm)[-1]:0)
#endif
#ifndef ccdlbdel
# define ccdlbdel(ccm) ccfree(ccdlb(ccm))
#endif

// Note: Array
#ifndef ccarrdel
# define ccarrdel ccdlbdel
#endif

#ifndef ccarrmax
# define ccarrmax(arr) (ccdlbmax(arr)/sizeof(*(arr))) // Note: null safe
#endif
#ifndef ccarrmin
# define ccarrmin(arr) (ccdlbmin(arr)/sizeof(*(arr))) // Note: null safe
#endif
#ifndef ccarrlen
# define ccarrlen(arr) cccast(ccu32_t,ccarrmin(arr))
#endif
#ifndef ccarrleni
# define ccarrleni(arr) cccast(cci32_t,ccarrmin(arr))
#endif

#ifndef ccarrend_max
# define ccarrend_max(arr) ((arr)+ccarrmax(arr))
#endif
#ifndef ccarrend_min
# define ccarrend_min(arr) ((arr)+ccarrmin(arr))
#endif
#ifndef ccarrend
# define ccarrend ccarrend_min
#endif

#ifndef ccarrresi
# define ccarrresi(ccm,com) ccdlb_arradd(cccast(void **,&(ccm)),sizeof(*(ccm)),com,ccnil)
#endif
#ifndef ccarrres
# define ccarrres(ccm,com) ((ccm)+ccarrresi(ccm,com))
#endif
#ifndef ccarraddi
# define ccarraddi(ccm,com) ccdlb_arradd(cccast(void **,&(ccm)),sizeof(*(ccm)),com,com)
#endif
#ifndef ccarradd
# define ccarradd(ccm,com) ((ccm)+ccarraddi(ccm,com))
#endif
#ifndef ccarrone
# define ccarrone(ccm) ccarradd(ccm,1)
#endif
#ifndef ccarrzro
# define ccarrzro(ccm) memset(ccm,ccnil,ccdlbmax(ccm))
#endif
#ifndef ccarrfix
# define ccarrfix(ccm) ((ccm)?ccdlb_(ccm)->rem_rze=cctrue:(ccnil))
#endif

#ifndef ccarrfor
# define ccarrfor(arr,itr) for(itr=arr;itr<ccarrend(arr);++itr)
#endif

// Note: Table

#ifndef cclithsh
# define cclithsh(lit) ccstrlenL(lit),lit
#endif
#ifndef ccnsthsh
# define ccntshsh(nts) ccstrlenS(nts),nts
#endif
#ifndef ccptrhsh
# define ccptrhsh(ptr) -cccast(cci32_t,sizeof(ptr)),cccast(char*,ptr)
#endif

// Note: these use the global error code _ccerr_ ... something I'm not very fond of ...
ccfunc size_t ccdlb_tblget(void **, cci32_t, cci32_t, const char *);
ccfunc size_t ccdlb_tblput(void **, cci32_t, cci32_t, const char *);
ccfunc size_t ccdlb_tblset(void **, cci32_t, cci32_t, const char *);

#ifndef cctblgetL
# define cctblgetL(ccm,lit) ((ccm)+ccdlb_tblget(cccast(void **,ccaddr(ccm)),sizeof(*(ccm)),cclithsh(lit)))
#endif
#ifndef cctblgetS
# define cctblgetS(ccm,nts) ((ccm)+ccdlb_tblget(cccast(void **,ccaddr(ccm)),sizeof(*(ccm)),ccntshsh(nts)))
#endif
#ifndef cctblgetP
# define cctblgetP(ccm,ptr) ((ccm)+ccdlb_tblget(cccast(void **,ccaddr(ccm)),sizeof(*(ccm)),ccptrhsh(ptr)))
#endif

#ifndef cctblset
# define cctblset(ccm,len,key) ((ccm)+ccdlb_tblset(cccast(void **,ccaddr(ccm)),sizeof(*ccm),len,key))
#endif
#ifndef cctblsetL
# define cctblsetL(ccm,lit) ((ccm)+ccdlb_tblset(cccast(void **,ccaddr(ccm)),sizeof(*ccm),cclithsh(lit)))
#endif
#ifndef cctblsetS
# define cctblsetS(ccm,nts) ((ccm)+ccdlb_tblset(cccast(void **,ccaddr(ccm)),sizeof(*ccm),ccntshsh(nts)))
#endif
#ifndef cctblsetP
# define cctblsetP(ccm,ptr) ((ccm)+ccdlb_tblset(cccast(void **,ccaddr(ccm)),sizeof(*ccm),ccptrhsh(ptr)))
#endif

#ifndef cctblputL
# define cctblputL(ccm,lit) ((ccm)+ccdlb_tblput(cccast(void **,ccaddr(ccm)),sizeof(*ccm),cclithsh(lit)))
#endif
#ifndef cctblputS
# define cctblputS(ccm,nts) ((ccm)+ccdlb_tblput(cccast(void **,ccaddr(ccm)),sizeof(*ccm),ccntshsh(nts)))
#endif
#ifndef cctblputP
# define cctblputP(ccm,ptr) ((ccm)+ccdlb_tblput(cccast(void **,ccaddr(ccm)),sizeof(*ccm),ccptrhsh(ptr)))
#endif

#ifndef ccstrdel
# define ccstrdel ccarrdel
#endif

#ifndef ccstrmax
# define ccstrmax ccarrmax
#endif
#ifndef ccstrmin
# define ccstrmin ccarrmin
#endif
#ifndef ccstrlen
# define ccstrlen ccarrlen
#endif

#ifndef ccstradd
# define ccstradd(ccm,res,com) ((ccm)+ccdlb_arradd(cccast(void **,&(ccm)),sizeof(*(ccm)),res,com))
#endif
// Note: length string
#ifndef ccstrcatN
# define ccstrcatN(ccm,len,str) ((ccm)+ccdlb_stradd(&(ccm),len+1,len+0,str))
#endif
#ifndef ccstrputN
# define ccstrputN(ccm,len,str) ((ccm)+ccdlb_stradd(&(ccm),len+1,len+1,str))
#endif
// Note: literal string
#ifndef ccstrcatL
# define ccstrcatL(ccm,str) ccstrcatN(ccm,ccstrlenL(str),str)
#endif
#ifndef ccstrputL
# define ccstrputL(ccm,str) ccstrputN(ccm,ccstrlenL(str),str)
#endif
// Note: null terminated string
#ifndef ccstrcatS
# define ccstrcatS(ccm,str) ccstrcatN(ccm,ccstrlenS(str),str)
#endif
#ifndef ccstrputS
# define ccstrputS(ccm,str) ccstrputN(ccm,ccstrlenS(str),str)
#endif

#ifndef ccstrcatnl
# define ccstrcatnl(ccm) ccstrputL(ccm,"\r\n")
#endif
#ifndef ccstrcatf
# define ccstrcatf(ccm,fmt,...) ccstr_catf(&ccm,fmt,__VA_ARGS__)
#endif

ccfunc size_t
ccdlb_arradd_(ccdlb_t **dlb_, size_t rsze, size_t csze)
{
ccenter("arradd");

/* ccdlb_arradd_:
**  rsze: size to reserve
**  csze: size to commit, can be 0
**
** For instance: `ccdlb_arradd_(..,rsze=10,csze=5)`
** The buffer is ensured to be at-least 10 bytes, however, the min counter will
** only be incremented by 5.
**
** This comes in handy, at least for the type of programming I do ...
*/
  ccdlb_t *dlb=*dlb_;

  int is_ini=!dlb;

  size_t
    sze_max=ccnil,
    sze_min=ccnil;
  cci32_t
    rem_rze=ccnil;
  ccalloctr_t
   *alloctr=ccalloctr;

  if(!is_ini)
  { sze_max=dlb->sze_max;
    sze_min=dlb->sze_min;
    rem_rze=dlb->rem_rze;
    alloctr=dlb->alloctr;
  }

  // Note: ensure that we never commit past what we're about to reserve or what
  // we've reserved already ...
  ccassert(csze<=rsze+sze_max-sze_min);

  if(sze_max<sze_min+rsze)
  {
    ccassert(!rem_rze);

    sze_max<<=1;
    if(sze_max<sze_min+rsze)
    	sze_max=sze_min+rsze;

    dlb=(ccdlb_t*)cccall(alloctr(sizeof(*dlb)+sze_max,dlb));
    *dlb_=dlb;

    if(is_ini)
    { dlb->rem_rze=ccfalse;
      dlb->rem_add=ccfalse;
      dlb->entries=ccfalse;
      dlb->alloctr=alloctr;
    }
  }

  dlb->sze_max=sze_max;
  dlb->sze_min=sze_min+csze;

ccleave("arradd");
  return sze_min;
}

ccfunc ccinle size_t
ccdlb_arradd(void **ccm, size_t isze, size_t cres, size_t ccom)
{
// Note: ccdlb_arradd
// This could be made into a macro, but it would look rather funky, like me and ain't no-one likes me ...
  ccdlb_t *dlb=ccdlb(*ccm);
  size_t res=ccdlb_arradd_(&dlb,isze*cres,isze*ccom);
  *ccm=dlb+1;
  return res/isze;
}

ccfunc size_t
ccdlb_stradd(char **ccm, size_t cres, size_t ccom, const char *cpy)
{
ccenter("stradd");
	// Note: use char type instead of void for preemptive type-checking ...
  // Note: assuming that you'll reserve at-least one more byte for the null terminator ...
  ccassert(cres!=0);
  ccassert(ccom!=0);
  size_t res=ccdlb_arradd(cccast(void**,ccm),1,cres,ccom);
  char *cur=(char*)*ccm+res;
  memcpy(cur,cpy,cres-1);
  cur[cres-1]=0;

ccleave("stradd");
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

  if(!dlb)
  {
    // Note: reserve some items, this will initialize the memory ...
    ccdlb_arradd_(&dlb,isze*0xff,0x00);
    ccarrzro(dlb+1);

    // Note: create some entries
    ccarrres(dlb->entries,0xff);
    ccarrzro(dlb->entries);
    ccarrfix(dlb->entries);

    // Note:
    *dlb_=dlb;
  }
}

ccfunc ccent_t *
ccdlb_tblcat(ccdlb_t **tbl, size_t isze, int len, const char *key, ccent_t *ent)
{
  ccnotnil(tbl);
  ccnotnil(*tbl);

  if(ent->key)
  { ent->nex=ccmalloc_T(ccent_t);
    ent=ent->nex;
    memset(ent,ccnil,sizeof(*ent));
    // ccarradd(ccdref(tbl)->entries,1);
  }

  ccassert(ent->key==ccnil);
  ccassert(ent->len==ccnil);
  ccassert(ent->val==ccnil);

  // Note: must zero the item
  size_t val=ccdlb_arradd_(tbl,isze,isze);
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

ccfunc ccent_t *
ccdbl_query(ccdlb_t *tbl, int len, const char *key)
{
	ccassert(key!=0);
	ccassert(len!=0);

  ccu64_t  hsh=cchsh_abc(len,key);
  size_t   idx=hsh%ccarrmax(tbl->entries);

  ccent_t *ent=tbl->entries+idx;

  ccerrset(ccerr_kNON);
  while(ent->key)
  { if((ent->len==len)&&((ent->key==key)||(!memcmp(ent->key,key,len))))
      return ent;
    if(!ent->nex)
      break;
    ccevent()->collisions++;
    ent=ent->nex;
  }
  ccerrset(ccerr_kNIT);
  return ent;
}

ccfunc size_t
ccdlb_tblget(void **ccm, cci32_t isze, int len, const char *key)
{
ccenter("tblget");

	ccdlb_t *tbl=ccdlb(ccdref(ccm));
  cckeyset(ccnil);

	// Todo: probably return an index that the user can still write to, but it won't affect other items...
  size_t val=ccnil;

  if(tbl)
  { ccent_t *ent=ccdbl_query(tbl,len,key);
	  if(ccerrnon())
	  { cckeyset(ent->key);
			val=ent->val;
	  }
  }

ccleave("tblget");
  return val/isze;
}

ccfunc size_t
ccdlb_tblput(void **ccm, cci32_t isze, int len, const char *key)
{
ccenter("tblput");

  ccdlb_t *tbl=ccdlb(ccdref(ccm));
  if(!tbl)
    ccdlb_tblini(&tbl,isze);
  cckeyset(ccnil);

	// Todo: probably return an index that the user can still write to, but it won't affect other items...
  size_t val=ccnil;

  ccent_t *ent=ccdbl_query(tbl,len,key);

  if(ccerrnit())
  { ccerrset(ccerr_kNON);

    ent=ccdlb_tblcat(&tbl,isze,len,key,ent);
    cckeyset(ent->key);

    val=ent->val;

    *ccm=tbl+1;
  } else
    ccerrset(ccerr_kAIT);

ccleave("tblput");
  return val/isze;
}

ccfunc size_t
ccdlb_tblset(void **ccm, cci32_t isze, int len, const char *key)
{
ccenter("tblset");

  ccdlb_t *tbl=ccdlb(ccdref(ccm));
  if(!tbl)
    ccdlb_tblini(&tbl,isze);

  cckeyset(ccnil);

	// Todo: probably return an index that the user can still write to, but it won't affect other items...
  size_t val=ccnil;
  ccent_t *ent=ccdbl_query(tbl,len,key);

  if(ccerrnit())
  { ccerrset(ccerr_kNON);
    ent=ccdlb_tblcat(&tbl,isze,len,key,ent);
    *ccm=tbl+1;
  }

  val=ent->val;

  cckeyset(ent->key);

ccleave("tblset");
  return val/isze;
}

ccfunc size_t
ccstr_vcatf(char **ccm, const char *fmt, va_list vli)
{ int len=stbsp_vsnprintf(0,0,fmt,vli);
  size_t res=ccdlb_arradd(cccast(void**,ccm),1,len+1,len);
  len=stbsp_vsnprintf((char*)*ccm+res,len+1,fmt,vli);
  return res;
}

ccfunc size_t
ccstr_catf(char **ccm, const char *fmt, ...)
{ va_list vli;
  va_start(vli,fmt);
  size_t res=ccstr_vcatf(ccm,fmt,vli);
  va_end(vli);
  return res;
}

ccfunc void
ccdlb_test()
{
#ifdef _DLB_TESTS
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