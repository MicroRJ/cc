#ifndef _CCCOLOR
#define _CCCOLOR

#define CCCOLOR_WITHIN(x,l,r) (((x)>=(l))&&((x)<=(r)))

// Todo: actually setup the color table properly ...
// Todo: this is absolute crap, will need a proper printf parser for this to work properly or modify stb's...
ccfunc void
ccprintf(const char *f, ...)
{
  HANDLE h=GetStdHandle(STD_OUTPUT_HANDLE);

	// CONSOLE_SCREEN_BUFFER_INFOEX e={sizeof(e)};
  // if(GetConsoleScreenBufferInfoEx(h,&e))
  // {
  // 	SetConsoleScreenBufferInfoEx(h,&e);
  // }

  const char *r=f;

  va_list vli;
  va_start(vli,f);

  WORD   s[0x10];
  WORD * p=s;
  WORD   v=0;
  char   c;

#define move( x )  v=x
#define push(   ) *p=v,p++
#define load(   )  p--,v=*p
#define next( l )  r+=l,c=*r
#define sval(   )  (char  *) va_arg(vli,char  *)
#define lval(   )  (cci64_t) va_arg(vli,cci64_t)
#define ival(   )  (cci32_t) va_arg(vli,cci32_t)
#define fval(   )  (ccf64_t) va_arg(vli,ccf64_t)
#define wval(   )  (cci16_t) va_arg(vli,cci32_t)
#define cval(   )  (char   ) va_arg(vli,char   )

  move(7);

  for(;c=*r;c)
  { SetConsoleTextAttribute(h,v);
    for(; (c!='\0')&&(c!='%') &&
         !(c=='<'&&r[1]=='!') &&
         !(c=='!'&&r[1]=='>'); next(1)) printf("%c",c);

    if(c=='<')
    { next(2);
      if(CCCOLOR_WITHIN(c,'0','F'))
        push(),move(c-'0'),next(1);
      else
      if(c=='%'&&r[1]=='i')
        push(),move(wval()),next(2);
      else
        ccassert(!"error");
    } else
    if(c=='!')
    { load(),next(2);
    } else
    if(c=='%')
    { next(1);
      if(c=='%')
        printf("%%"),next(1);
      else
      if(c=='i')
        printf("%i",ival()),next(1);
      else
      if(c=='f')
        printf("%f",fval()),next(1);
      else
      if(c=='s')
        printf("%s",sval()),next(1);
      else
      if(c=='c')
        printf("%c",cval()),next(1);
      else
      { if(c=='l'&&r[1]=='l'&&r[2]=='i')
          printf("%lli",lval()),next(3);
        else
        if(c=='l'&&r[1]=='l'&&r[2]=='u')
          printf("%llu",lval()),next(3);
        else
          ccassert(!"error");
      }
    } else
    if(c=='\0')
      break;
  }
  va_end(vli);

#undef move
#undef push
#undef load
#undef next
#undef sval
#undef lval
#undef ival
#undef fval
#undef wval
}
#endif