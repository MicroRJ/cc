/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved. **/
/*****************************************************************/
#ifndef _CCTRACE
#define _CCTRACE

ccfunc void
ccout(const char *string)
{
#if defined(_DEBUG) && defined(_WIN32)
  OutputDebugStringA(string);
#endif
  printf(string);
}

#ifndef cctrace
# define cctrace(tag,fmt,...) cctrace_(__COUNTER__,__FILE__,__LINE__,__FUNC__,tag,fmt,__VA_ARGS__)
#endif

#ifndef cctracelog
# define cctracelog(fmt,...) cctrace("log",fmt,__VA_ARGS__)
#endif

#ifndef cctracewar
# define cctracewar(fmt,...) cctrace("war",fmt,__VA_ARGS__)
#endif

#ifndef cctraceerr
# define cctraceerr(fmt,...) cctrace("err",fmt,__VA_ARGS__)
#endif

ccfunc void
cctrace_(int guid, const char  *file, int line, const char *func, const char *tag, const char *fmt, ...);

ccfunc int
ccformatvex(char *buf, int len, const char * fmt, va_list vli)
{
  return stbsp_vsnprintf(buf,len,fmt,vli);
}

ccfunc char *
ccformatv(const char * fmt, va_list vli)
{
	// Todo:
	static __declspec(thread) char buf[0xff];

  ccformatvex(buf,0xff,fmt,vli);

  return buf;
}

ccfunc int
ccformatex(char *buf, int len, const char * fmt, ...)
{
	va_list vli;
  va_start(vli,fmt);
  int res=ccformatvex(buf,len,fmt,vli);
  va_end(vli);

  return res;
}

ccfunc char *
ccformat(const char * fmt, ...)
{
	va_list vli;
  va_start(vli,fmt);
  char *res=ccformatv(fmt,vli);
  va_end(vli);

  return res;
}

ccfunc void
cctrace_(int guid, const char  *file, int line, const char *func, const char *tag, const char *fmt, ...)
{
  (void)guid;

  va_list vli;
  va_start(vli, fmt);

  char buf[0xff];
  ccformatvex(buf,0xff,fmt,vli);

  ccout(ccformat("%s[%i] %s() %s: %s\n", file,line,func,tag,buf));

  va_end(vli);
}

#endif