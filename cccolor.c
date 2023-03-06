// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCCOLOR
#define _CCCOLOR

#define CCFG_BLACK        "<!0"
#define CCFG_BLUE         "<!1"
#define CCFG_GREEN        "<!2"
#define CCFG_CYAN         "<!3"
#define CCFG_RED          "<!4"
#define CCFG_MAGENTA      "<!5"
#define CCFG_BROWN        "<!6"
#define CCFG_LIGHTGRAY    "<!7"
#define CCFG_GRAY         "<!8"
#define CCFG_LIGHTBLUE    "<!9"
#define CCFG_LIGHTGREEN   "<!0"
#define CCFG_LIGHTCYAN    "<!1"
#define CCFG_LIGHTRED     "<!2"
#define CCFG_LIGHTMAGENTA "<!3"
#define CCFG_YELLOW       "<!4"
#define CCFG_WHITE        "<!F"
#define CCEND             "!>"

// Todo: actually setup the color table properly ...
// CONSOLE_SCREEN_BUFFER_INFOEX e={sizeof(e)};
// if(GetConsoleScreenBufferInfoEx(h,&e))
// { SetConsoleScreenBufferInfoEx(h,&e);
// }

// Todo: this is absolute crap, will need a proper printf parser for this to work properly or modify stb's...
ccfunc void
ccprintf(const char *r, ...)
{
  va_list vli;
  va_start(vli,r);

#define cccolornext( l )  r+=l,c=*r
#define cccolorpval(   )  (void  *) va_arg(vli,void  *)
#define cccolorsval(   )  (char  *) va_arg(vli,char  *)
#define cccolorlval(   )  (cci64_t) va_arg(vli,cci64_t)
#define cccolorival(   )  (cci32_t) va_arg(vli,cci32_t)
#define cccolorfval(   )  (ccf64_t) va_arg(vli,ccf64_t)
#define cccolorwval(   )  (cci16_t) va_arg(vli,cci32_t)
#define cccolorcval(   )  (char   ) va_arg(vli,char   )

  // --- Todo: remove this from here!
  HANDLE h=GetStdHandle(STD_OUTPUT_HANDLE);


  char c;
  for(;c=*r;c)
  {
    for(; (c!='\0')&&(c!='%') &&
         !(c=='<'&&r[1]=='!') &&
         !(c=='!'&&r[1]=='>'); cccolornext(1)) printf("%c",c);

    if(c=='<')
    { cccolornext(2);
      if(CCWITHIN(c,'0','9'))
        cccolorpush(),cccolormove(0x00+c-'0'),cccolornext(1);
      else
      if(CCWITHIN(c,'A','F'))
        cccolorpush(),cccolormove(0x0A+c-'A'),cccolornext(1);
      else
      if(CCWITHIN(c,'a','f'))
        cccolorpush(),cccolormove(0x0A+c-'a'),cccolornext(1);
      else
      if(c=='%'&&r[1]=='i')
        cccolorpush(),cccolormove(cccolorwval()),cccolornext(2);
      else
        ccassert(!"error");
    } else
    if(c=='!')
    { cccolorload(),cccolornext(2);
    } else
    if(c=='%')
    { cccolornext(1);
      if(c=='%')
        printf("%%"),cccolornext(1);
      else
      if(c=='i')
        printf("%i",cccolorival()),cccolornext(1);
      else
      if(c=='f')
        printf("%f",cccolorfval()),cccolornext(1);
      else
      if(c=='s')
        printf("%s",cccolorsval()),cccolornext(1);
      else
      if(c=='c')
        printf("%c",cccolorcval()),cccolornext(1);
      else
      if(c=='p')
        printf("%p",cccolorpval()),cccolornext(1);
      else
      { if(c=='l'&&r[1]=='l'&&r[2]=='i')
          printf("%lli",cccolorlval()),cccolornext(3);
        else
        if(c=='l'&&r[1]=='l'&&r[2]=='u')
          printf("%llu",cccolorlval()),cccolornext(3);
        else
          ccassert(!"error");
      }
    } else
    if(c=='\0')
      break;

    SetConsoleTextAttribute(h,cccolorstate.v);
  }
  va_end(vli);
}
#endif