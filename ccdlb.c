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

// Note: returns the previous item count ...
ccfunc cci64_t
ccdlbadd(void **ccm, cci32_t isze, cci64_t rsze, cci64_t csze)
{
  ccdlb_t *dlb;
  ccallocator_t *allocator;
  int rem_rze,rem_add;
  cci64_t sze_max, sze_min;

  if(*ccm)
  { dlb=ccdlb_(*ccm);
    allocator=dlb->allocator;
    sze_max=dlb->sze_max;
    sze_min=dlb->sze_min;
    rem_rze=dlb->rem_rze;
    rem_add=dlb->rem_add;
    ccassert(!rem_add);
  } else
  { dlb=ccnull;
    allocator=ccallocator;
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
    dlb=cccast(ccdlb_t*,allocator(sizeof(*dlb)+sze_max*isze,dlb));

    if(!*ccm)
    { dlb->rem_rze=rem_rze;
      dlb->rem_add=rem_add;
      dlb->allocator=allocator;
      dlb->entries=0;
    }

    *ccm=dlb+1;

    dlb->sze_max=sze_max;
  }

  dlb->sze_min=sze_min+csze;
  return sze_min;
}

// Todo: does this use a good hashing function for strings?
ccfunc int
cctblhsh(void **ccm, cci32_t bit, int len, char *key, int create_always)
{
  ccassert(len!=0);
  ccassert(key!=0);

  ccdlb_t *dlb;

  if(!*ccm)
  {
    if(create_always)
    {
      ccdlbadd(ccm,bit,1,0);

      dlb=ccdlb_(*ccm);

      // Todo: how should we size up the hash-table?
      ccarradd(dlb->entries,0xff);
      ccarrzro(dlb->entries);
      ccarrfix(dlb->entries);
    }
      else return ccfalse;
  }
    else dlb=ccdlb_(*ccm);

  ccu64_t hsh=5381;
  ccu64_t idx,max;

  if(len>0)
    for(idx=0;idx<len;hsh=hsh<<5,hsh=hsh+key[idx++]);
  else
    hsh=cccast(ccu64_t,key);

  max=ccarrmax(dlb->entries);

  idx=hsh%max;

  ccentry=dlb->entries+idx;

  while(ccentry->key)
  { if(ccentry->len==len)
    { if(len>0)
      { if(!memcmp(ccentry->key,key,len))
          return cctrue;
      } else
      { if(ccentry->key==key)
          return cctrue;
      }
    }
    if(!ccentry->nex)
      break;
    ccentry=ccentry->nex;
  }

  if(create_always)
  {
    // Note: if this entry is occupied append one...
    if(ccentry->key)
      ccentry=ccentry->nex=ccmalloc_T(ccentry_t);

    cci64_t val=ccdlbadd(ccm,bit,1,1);

    // Todo: remove this from here!
    memset(cccast(char*,*ccm)+val*bit,ccnull,bit);

    // Todo:
    ccentry->nex=ccnull;
    ccentry->len=len;
    ccentry->key=key;
    ccentry->val=val;
  }

  return ccfalse;
}

ccfunc cci64_t
cctblgeti(void **ccm, cci32_t isze, cci32_t len, char *key)
{ ccassert(ccm!=0);
  ccerrset(ccerr_kNIT);
  if(cctblhsh(ccm,isze,len,key,ccfalse))
    ccerrset(ccerr_kNON);
  return ccentry? ccentry->val :ccnull;
}

ccfunc cci64_t
cctblputi(void **ccm, cci32_t isze, cci32_t len, char *key)
{ ccassert(ccm!=0);
  ccerrset(ccerr_kAIT);
  if(!cctblhsh(ccm,isze,len,key,cctrue))
    ccerrset(ccerr_kNON);
  return ccentry? ccentry->val :ccnull;
}

ccfunc cci64_t
cctblseti(void **ccm, cci32_t isze, cci32_t len, char *key)
{ ccassert(ccm!=0);
  ccerrset(ccerr_kNON);
  cctblhsh(ccm,isze,len,key,cctrue);
  return ccentry? ccentry->val :ccnull;
}

ccfunc cci64_t
ccstradd(char **ccm, cci64_t res, cci64_t com, const char *mem)
{
  // Note: assuming that you'll reserve at-least one more byte for the null terminator ...
  ccassert(res!=0);
  ccassert(com!=0);

  cci64_t result=ccdlbadd(cccast(void**,ccm),sizeof(char),res,com);

  char *cur=*ccm+res;

  memcpy(cur,mem,res-1);
  cur[res-1]=0;

  return result;
}

// Todo: remove from here?
ccfunc cci64_t
ccstr_vcatf(char **ccm, const char *fmt, va_list vli)
{ int len=stbsp_vsnprintf(0,0,fmt,vli);
  cci64_t res=ccdlbadd(cccast(void**,ccm),1,len+1,len);
  len=stbsp_vsnprintf((char*)*ccm+res,len+1,fmt,vli);
  return res;
}

// Todo: remove from here?
ccfunc cci64_t
ccstr_catf(char **ccm, const char *fmt, ...)
{ va_list vli;
  va_start(vli,fmt);
  cci64_t res=ccstr_vcatf(ccm,fmt,vli);
  va_end(vli);
  return res;
}


#endif