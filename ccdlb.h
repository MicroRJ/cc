#ifndef _CCDLB
#define _CCDLB

// Note:
// ccarrdel,ccstrdel = ccdel
// ccarrmin,ccstrmin = ccmin
// ccarrmax,ccstrmax = ccmax
// ccarrlen,ccstrlen = cclen
// ccarradd,ccstradd = ccadd
// ccarrres,ccstrres = ccres
// ccarrfor,ccstrfor = ccfor

// Note: if the length is negative then key should be interpreted as integer
typedef struct ccdlb_entry_t
{ ccdlb_entry_t *nex;
  int            len;
  char          *key;
  ccu32          val;
} ccdlb_entry_t;

// Todo: sorting ...
typedef struct ccdlb_t
{
#ifdef _DEBUG
  int debug_collisions;
#endif
  unsigned       can_rze: 1;
  unsigned       sze_fxd: 1;

  ccdlb_entry_t *entries;

  ccu32          sze_max;
  ccu32          sze_min;
} ccdlb_t;

// Todo:
#ifndef ccsub
# define ccsub(x,y) ((x)-(y))
#endif
#ifndef ccadd
# define ccadd(x,y) ((x)+(y))
#endif


// Note: C string utils ...
#ifndef ccstrlenS
# define ccstrlenS(cstr) cccast(ccu32,strlen(cstr))
#endif
#ifndef ccstrlenL
# define ccstrlenL(lstr) cccast(ccu32,sizeof(lstr)-1)
#endif


#ifndef ccdlbraw_
# define ccdlbraw_(ccm) ccsub(cccast(ccdlb_t*,ccm),1)
#endif
#ifndef ccdlbraw
# define ccdlbraw(ccm) ((ccm)?ccdlbraw_(ccm):(ccnil))
#endif

#ifndef ccdlbmax
# define ccdlbmax(ccm) ((ccm)?cccast(ccu32 *,ccm)[-2]:0)
#endif
#ifndef ccdlbmin
# define ccdlbmin(ccm) ((ccm)?cccast(ccu32 *,ccm)[-1]:0)
#endif
#ifndef ccdlbdel
# define ccdlbdel(ccm) ccfree(ccdlbraw(ccm))
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
# define ccarrlen ccarrmin
#endif

#ifndef ccarrend
# define ccarrend(arr) ((arr)?((arr)+ccarrlen(arr)):(arr))
#endif
#ifndef ccarrfor
# define ccarrfor(arr,itr) for(itr=arr;itr<ccarrend(arr);++itr)
#endif
#ifndef ccarrres
# define ccarrres(ccm,com) ((ccm)+ccdlb_arradd(cccast(void **,&(ccm)),sizeof(*(ccm)),com,ccnil))
#endif
#ifndef ccarraddi
# define ccarraddi(ccm,com) ccdlb_arradd(cccast(void **,&(ccm)),sizeof(*(ccm)),com,com)
#endif
#ifndef ccarradd
# define ccarradd(ccm,com) ((ccm)+ccarraddi(ccm,com))
#endif
#ifndef ccarrzro
# define ccarrzro(ccm) memset(ccm,ccnil,ccdlbmax(ccm))
#endif

#ifndef ccarrfix
# define ccarrfix(ccm) ((ccm)?ccdlbraw_(ccm)->sze_fxd=cctrue:(ccnil))
#endif

// Note: Table
//
#ifndef cctblsetL
# define cctblsetL(ccm,lit) ((ccm)+ccdlb_tblset(cccast(void **,ccaddr(ccm)),sizeof(*ccm),ccstrlenL(lit),lit))
#endif
#ifndef cctblsetS
# define cctblsetS(ccm,lit) ((ccm)+ccdlb_tblset(cccast(void **,ccaddr(ccm)),sizeof(*ccm),ccstrlenS(lit),lit))
#endif
#ifndef cctblsetP
# define cctblsetP(ccm,ptr) ((ccm)+ccdlb_tblset(cccast(void **,ccaddr(ccm)),sizeof(*ccm),-cccast(cci32,sizeof(ptr)),ptr))
#endif


// Note: String
typedef char *ccstr_t;

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

ccfunc ccu32
ccdlb_arradd_(ccdlb_t **dlb_, ccu32 rsze, ccu32 csze)
{
/* ccdlb_arradd_:
**  rsze: size to reserve
**  csze: size to commit, can be 0
**
** For instance: `ccdlb_arradd_(..,rsze=10,csze=5)`
** The buffer is ensured to be at-least 10 bytes, however, the min counter will
** only be incremented by 5.
**
** This comes in plenty handy, at least for the type of programming I do ...
*/


  ccdlb_t *dlb=*dlb_;

  int is_ini=!dlb;

  ccu32
    sze_max=ccnil,
    sze_min=ccnil;
  cci32
    sze_fxd=ccnil;

  if(!is_ini)
  { sze_max=dlb->sze_max;
    sze_min=dlb->sze_min;
    sze_fxd=dlb->sze_fxd;
  }


  // Note: ensure that we never commit past what we're about to reserve or what
  // we've reserved already ...
  ccassert(csze<=rsze+sze_max-sze_min);

  if(sze_max<sze_min+rsze)
  {
    ccassert(!sze_fxd);

    sze_max<<=1;
    if(sze_max<sze_min+rsze)
    { sze_max=sze_min+rsze;
    }

    dlb=*dlb_=(ccdlb_t*)ccrealloc(dlb,sizeof(*dlb)+sze_max);

    if(is_ini)
    {
      memset(dlb,ccnil,sizeof(*dlb));
    }
  }

  dlb->sze_max=sze_max;
  dlb->sze_min=sze_min+csze;
  return sze_min;
}

ccfunc ccinle ccu32
ccdlb_arradd(void **ccm, ccu32 isze, ccu32 cres, ccu32 ccom)
{
// Note: ccdlb_arradd
// This could be made into a macro, but it would look rather ugly, like me, ain't no-one likes me ...
  ccdlb_t *dlb=ccdlbraw(*ccm);
  ccu32 res=ccdlb_arradd_(&dlb,isze*cres,isze*ccom);
  *ccm=dlb+1;
  return res/isze;
}

ccfunc ccu32
ccdlb_stradd(char **ccm, ccu32 cres, ccu32 ccom, const char *cpy)
{ // Note: use char type instead of void for preemptive type-checking ...
  // Note: assuming that you'll reserve at-least one more byte for the null terminator ...
  ccassert(cres!=0);
  ccassert(ccom!=0);
  ccu32 res=ccdlb_arradd(cccast(void**,ccm),1,cres,ccom);
  char *cur=(char*)*ccm+res;
  memcpy(cur,cpy,cres-1);
  cur[cres-1]=0;
  return res;
}

ccfunc ccu32
ccdlb_tblset_(ccdlb_t **tbl_, ccu32 isze, int len_, void *key_)
{
// Note: ccdlb_tblset_
// This is an internal function and it operates on byte size rather than item count.
// The return value is the byte offset to the corresponding item and the isze param is the item size in bytes ...
// The table is presumed to be initialized aready and there should be enough items in the dynamic buffer ...
//
  ccnotnil(tbl_);
  ccnotnil(key_);
  ccnotnil(len_);

  ccdlb_t *tbl=*tbl_;

  ccnotnil(tbl);

  char *key=cccast(char*,key_);

  int   len;
  ccu64 hsh;

  if(len_>0)
  { len=len_;
    hsh=5381;
    for(int i=0;i<len;++i) hsh+=(hsh<<5)+key[i];
  } else
  { len=-len_;
    hsh=cccast(ccu64,key_);
  }

  ccdlb_entry_t *entry=tbl->entries+hsh%ccarrmax(tbl->entries);

  while(entry->key)
  {
    // Note: entry's key length is signed to differentiate a string hash from a pointer hash ...
    if(entry->len==len_)
    {
      if(len_>0)
      {
        if(memcmp(entry->key,key,len)==0)
          return entry->val;

      } else
      if(entry->key==key)
      {
        return entry->val;
      }
    }

    tbl->debug_collisions++;

    if(entry->nex)
      entry=entry->nex;
    else break;
  }

  if(entry->key)
  {
    entry->nex=ccmalloc_T(ccdlb_entry_t);
    entry=entry->nex;
    memset(entry,ccnil,sizeof(*entry));
  }

  // Note: must zero the item
  ccu32 val=ccdlb_arradd_(&tbl,isze,isze);
  memset(cccast(char*,tbl+1)+val,ccnil,isze);

  ccassert(entry->key==ccnil);
  ccassert(entry->len==ccnil);
  ccassert(entry->val==ccnil);

  if(len_>0)
  {
    // Todo:
    ccstrputN(entry->key,len,cccast(char*,key_));
  } else
      entry->key=key;

  entry->len=len_;
  entry->val=val;

  *tbl_=tbl;

  return val;
}

ccfunc ccu32
ccdlb_tblset(void **ccm, ccu32 isze, int len, void *key)
{
  ccdlb_t *dlb=ccdlbraw(*ccm);

  // Todo:
  int is_init=!dlb;
  if(is_init)
  {
    // Todo:
    ccdlb_arradd_(&dlb,isze*0xff,isze*0xff);
    memset(dlb,ccnil,sizeof(*dlb));

    // Todo:
    ccarrres(dlb->entries,0xff);
    ccarrzro(dlb->entries);
    ccarrfix(dlb->entries);

    ccassert(ccarrmax(dlb->entries)==0xff);
    ccassert(ccarrmin(dlb->entries)==0x00);
  }

  ccu32 res;
  res=ccdlb_tblset_(&dlb,isze,len,key);

  *ccm=dlb+1;

  return res/isze;
}



ccfunc ccu32
ccstr_vcatf(char **ccm, const char *fmt, va_list vli)
{
  int len=stbsp_vsnprintf(0,0,fmt,vli);
  ccu32 res=ccdlb_arradd(cccast(void**,ccm),1,len+1,len);
  len=stbsp_vsnprintf((char*)*ccm+res,len+1,fmt,vli);
  return res;
}

ccfunc ccu32
ccstr_catf(char **ccm, const char *fmt, ...)
{
  va_list vli;
  va_start(vli,fmt);

  ccu32 res;
  res=ccstr_vcatf(ccm,fmt,vli);

  va_end(vli);

  return res;
}


ccfunc void
ccdlb_test()
{
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

    key=ccformat("%c%i",'a'+i,i);
    res=cctblsetS(string_map,key);
    ccassert(res->s==ccnil);
    ccstrputS(res->s,key);

    key=ccformat("%c%i",'a'+i,i);
    res=cctblsetS(string_map,key);
    ccassert(strcmp(key,res->s)==0);

    key=ccformat("%c%i",'A'+i,i);
    res=cctblsetS(string_map,key);
    ccassert(res->s==ccnil);
    ccstrputS(res->s,key);

    key=ccformat("%c%i",'A'+i,i);
    res=cctblsetS(string_map,key);
    ccassert(strcmp(key,res->s)==0);
  }

  simple_t *pointer_map=ccnil;

  simple_t *item;
  ccarrfor(simple_arr,item)
  {
    char *key=item->s;

    simple_t *res;

    res=cctblsetS(string_map,key);
    ccassert(strcmp(key,res->s)==0);

    res=cctblsetP(pointer_map,item);
    ccstrputS(res->s,key);
  }

  ccarrfor(simple_arr,item)
  {
    char *key=item->s;

    simple_t *res;
    res=cctblsetP(pointer_map,item);


    ccassert(strcmp(key,res->s)==0);
  }

  cctracelog("array item count %i, reserved %i", ccarrmin(simple_arr), ccarrmax(simple_arr));
  cctracelog("string map collisions %i, items %i", ccdlbraw(string_map)->debug_collisions, ccarrlen(string_map));
  cctracelog("pointer map collisions %i, items %i", ccdlbraw(pointer_map)->debug_collisions, ccarrlen(string_map));
}

#endif