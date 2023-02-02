#ifndef _CCDLB
#define _CCDLB

// Note:
typedef struct ccblc_t
{
  unsigned int size_max;
  unsigned int size_min;
} ccblc_t;

#ifndef ccblc_raw
# define ccblc_raw(mem) ((mem)?(cccast(ccblc_t*,mem)-1):0)
#endif
#ifndef ccblc_max
# define ccblc_max(mem) ((mem)?cccast(unsigned int *,mem)[-2]:0)
#endif
#ifndef ccblc_min
# define ccblc_min(mem) ((mem)?cccast(unsigned int *,mem)[-1]:0)
#endif
#ifndef ccblc_del
# define ccblc_del(ccm) ccfree(ccblc_raw(ccm))
#endif
#ifndef ccarrnil
# define ccarrnil 0
#endif
#ifndef ccarrdel
# define ccarrdel ccblc_del
#endif
#ifndef ccarrlen
# define ccarrlen(arr) (ccblc_min(arr)/sizeof(*(arr)))
#endif
#ifndef ccarrfor
# define ccarrfor(arr,itr) for(itr=arr;itr<arr+ccarrlen(arr);++itr)
#endif
#ifndef ccarrres
# define ccarrres(ccm,com) ((ccm)+ccblc_arradd(cccast(void **,&(ccm)),sizeof(*(ccm)),com,ccnil))
#endif
#ifndef ccarradd
# define ccarradd(ccm,com) ((ccm)+ccblc_arradd(cccast(void **,&(ccm)),sizeof(*(ccm)),com,com))
#endif
typedef char *ccstr_t;
#ifndef ccstrnil
# define ccstrnil ccnil
#endif
#ifndef ccstrdel
# define ccstrdel ccblc_del
#endif
#ifndef ccstrlen
# define ccstrlen(str) (ccblc_min(str)/sizeof(*(str)))
#endif
#ifndef ccstradd
# define ccstradd(ccm,res,com) ((ccm)+ccblc_arradd(cccast(void **,&(ccm)),sizeof(*(ccm)),res,com))
#endif
#ifndef ccstrcatl
# define ccstrcatl(ccm,str,len) ((ccm)+ccblc_stradd(&(ccm),len+1,len+0,str))
#endif
#ifndef ccstrputl
# define ccstrputl(ccm,str,len) ((ccm)+ccblc_stradd(&(ccm),len+1,len+1,str))
#endif
#ifndef ccstrcat
# define ccstrcat(ccm,str) ccstrcatl(ccm,str,cccast(unsigned int,strlen(str)))
#endif
#ifndef ccstrput
# define ccstrput(ccm,str) ccstrputl(ccm,str,cccast(unsigned int,strlen(str)))
#endif
#ifndef ccstrcatf
# define ccstrcatf(ccm,fmt,...) ccstr_catf(&ccm,fmt,__VA_ARGS__)
#endif

ccfunc unsigned int
ccblc_add(ccblc_t **dlb_ptr, unsigned int res_size, unsigned int com_size)
{
  ccblc_t *dlb;
  dlb=*dlb_ptr;
  unsigned int size_max,size_min;
  size_max=dlb?dlb->size_max:0;
  size_min=dlb?dlb->size_min:0;

  // Note: Ensure commit is valid ...
  if(res_size+size_max-size_min<com_size) return 0xffffffff;

  if(size_max<size_min+res_size)
  { size_max<<=1;
    if(size_max<size_min+res_size)
    { size_max=size_min+res_size;
    }
    dlb=(ccblc_t*)ccrealloc(dlb,sizeof(*dlb)+size_max);
    *dlb_ptr=dlb;
  }
  dlb->size_max=size_max;
  dlb->size_min=size_min+com_size;
  return size_min;
}

ccfunc ccinle unsigned int
ccblc_arradd(void **ccm, unsigned int isze, unsigned int cres, unsigned int ccom)
{ ccblc_t *dlb=ccblc_raw(*ccm);
  unsigned int res=ccblc_add(&dlb,isze*cres,isze*ccom);
  *ccm=dlb+1;
  return res/isze;
}

// Note: use char type instead of void for preemptive type-checking ...
// Note: assuming that you'll reserve at-least one more byte for the null terminator ...
ccfunc unsigned int
ccblc_stradd(char **ccm, unsigned int cres, unsigned int ccom, const char *cpy)
{ ccassert(cres!=0);
  ccassert(ccom!=0);
  unsigned int res=ccblc_arradd(cccast(void**,ccm),1,cres,ccom);
  char *cur=(char*)*ccm+res;
  memcpy(cur,cpy,cres-1);
  cur[cres-1]=0;
  return res;
}

ccfunc unsigned int
ccstr_vcatf(char **ccm, const char *fmt, va_list vli)
{
  int len=stbsp_vsnprintf(0,0,fmt,vli);
  unsigned int res=ccblc_arradd(cccast(void**,ccm),1,len+1,len);
  len=stbsp_vsnprintf((char*)*ccm+res,len+1,fmt,vli);
  return res;
}

ccfunc unsigned int
ccstr_catf(char **ccm, const char *fmt, ...)
{
  va_list vli;
  va_start(vli,fmt);

  unsigned int res;
  res=ccstr_vcatf(ccm,fmt,vli);

  va_end(vli);

  return res;
}

#endif