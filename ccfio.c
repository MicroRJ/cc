#ifndef _CCFIO
#define _CCFIO

ccfunc const char *
ccfilename(const char *name)
{ const char *result=name;
	for(result=name;*name;++name)
	{ if((*name=='\\')||(*name=='/'))
		{ result=name+1;
		}
	}
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

ccfunc int
ccrealfile(void *file)
{ return (HANDLE)file!=INVALID_HANDLE_VALUE;
}

ccfunc unsigned long int
ccfilesize(void *file)
{ return GetFileSize((HANDLE)file,0x00);
}

ccfunc void
ccclosefile(void *file)
{ CloseHandle((HANDLE)file);
}

ccfunc void *
ccopenfile(const char *name)
{ void *file=(void*)CreateFileA(name,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_WRITE|FILE_SHARE_READ,0x00,OPEN_ALWAYS,0x00,0x00);
  return ccrealfile(file)?file:0;
}

ccfunc void *
ccpullfile(void *file, unsigned long int offset, unsigned long int *lplength)
{ if(!ccrealfile(file)) return 0;
  unsigned long int file_size;
  file_size=ccfilesize(file);
  unsigned long int length=lplength?*lplength:0;
  if(file_size<offset+length) return 0;
  if(!length) length=file_size;
  void *file_data=ccmalloc(length);
  if(!lplength) lplength=&length;
  if(!ReadFile((HANDLE)file,file_data,length,lplength,0x00)) return 0;
  return file_data;
}

ccfunc unsigned long int
ccpushfile(void *file, unsigned long int offset, unsigned long int length, void *file_data)
{ if(!ccrealfile(file)) return 0;
  unsigned long int file_size;
  file_size=ccfilesize(file);
  if(file_size<offset) return 0;
  if(!WriteFile((HANDLE)file,file_data,length,0x00,0x00)) return 0;
  return length;
}
#endif

#endif