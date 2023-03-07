// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCSYS_C
#define _CCSYS_C

#ifdef _WIN32
#ifndef CINTERFACE
# define CINTERFACE
#endif
#ifndef NOMINMAX
# define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
// #ifndef _NO_CRT_STDIO_INLINE
// # define _NO_CRT_STDIO_INLINE
// #endif
#include <windows.h>
#include <winuser.h>

#pragma comment(lib,"User32")
#endif

ccfunc const char *
ccfilename(const char *name)
{ const char *result=name;
  for(result=name;*name;++name)
    if((*name=='\\')||(*name=='/'))
      result=name+1;
  return result;
}

#ifdef _WIN32

ccfunc ccinle ccclocktick_t
ccclocktick()
{ LARGE_INTEGER l;
  QueryPerformanceCounter(&l);
  return l.QuadPart;
}
ccfunc ccinle ccf64_t
ccclocksecs(ccclocktick_t t)
{ LARGE_INTEGER l;
  QueryPerformanceFrequency(&l);
  return (((ccf64_t)t)/l.QuadPart);
}

// Note: also check for the file attributes
ccfunc int
ccrealfile(void *file)
{ return (HANDLE)file!=INVALID_HANDLE_VALUE;
}

ccfunc ccu32_t
ccfilesize(void *file)
{ return GetFileSize((HANDLE)file,0x00);
}

ccfunc void
ccclosefile(void *file)
{ CloseHandle((HANDLE)file);
}

// Todo: proper file disposition
ccfunc void *
ccopenfile(const char *name, const char *flags)
{

  DWORD waccess=0;

  if(flags)
  {
		for(;*flags;flags++)
		{
			if(*flags=='r')
				waccess|=GENERIC_READ;
			else
			if(*flags=='w')
				waccess|=GENERIC_WRITE;
		}
  }

  ccassert(waccess!=0);

  void *file=CreateFileA(name,waccess,FILE_SHARE_WRITE|FILE_SHARE_READ,0x00,OPEN_ALWAYS,0x00,0x00);

  if(!ccrealfile(file))
  {
    cctraceerr("'%s': invalid file",name);
    return 0;
  }

  return file;
}


ccfunc const char *
ccfilename_from_handle(void *file)
{
  static char buff[256];
  memset(buff,0,sizeof(buff));

  GetFinalPathNameByHandle(file,buff,sizeof(buff),FILE_NAME_OPENED);

  return ccfilename(buff);
}

ccfunc void *
ccpullfile(void *file, ccu32_t offset, ccu32_t *lplength)
{
  if(!ccrealfile(file))
  {
    cctraceerr("invalid file",0);
    return 0;
  }

  ccu32_t file_size;
  file_size=ccfilesize(file);
  ccu32_t length=lplength?*lplength:0;
  if(file_size<offset+length) return 0;
  if(!length) length=file_size;
  void *file_data=ccmalloc(length);
  if(!lplength) lplength=&length;
  if(!ReadFile((HANDLE)file,file_data,length,lplength,0x00)) return 0;
  return file_data;
}

ccfunc ccu32_t
ccpushfile(void *file, ccu32_t offset, ccu32_t length, void *file_data)
{
  if(!ccrealfile(file))
  {
    cctraceerr("invalid file",0);
    return 0;
  }

  ccu32_t file_size=ccfilesize(file);

  if(file_size<offset) return 0;

  if(!WriteFile((HANDLE)file,file_data,length,0x00,0x00))
  {
    cctraceerr("'%s': write failed", ccfilename_from_handle(file));
    return 0;
  }

  return length;
}
#endif

#endif