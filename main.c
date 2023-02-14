#define ARG 22

#include "cc.c"

int fib(int x)
{ if(x>=2)
  { int l=fib(x-2);
    int r=fib(x-1);
    return l+r;
  }
  return x;
}

ccfunc void testdbgsys()
{
	ccenter("test-debug-system");
		for(int i=0; i<8; ++i)
		{
	ccenter("test-debug-track-master");
	ccenter("test-debug-track");
			void *mem=ccmalloc(24);
			mem=ccrealloc(mem,48);
			ccfree(mem);
	ccleave("test-debug-track");
	ccleave("test-debug-track-master");
		}
	ccleave("test-debug-system");
}

int main(int argc, char **argv)
{
	ccini();

ccenter("main");
  ++ argv;
  -- argc;

ccenter("read");
  ccread_t read;
  ccread_init(&read);
  ccread_include(& read, "code\\fib.cc");
  cctree_t *tree;
  tree=ccread_translation_unit(&read);
ccleave("read");

ccenter("emit");
  ccemit_t emit;
	ccemit_init(&emit);
	ccemit_translation_unit(&emit,tree);
ccleave("emit");

ccenter("exec");

#ifndef _HARD_DEBUG
	ccclocktime_t tick=ccclocktick();
#endif
  ccexec_value_t retr;
  ccexec_t exec;
  ccexec_init(&exec);
  retr=ccexec_translation_unit(&exec,&emit);
  ccexec_uninit(&exec);

#ifndef _HARD_DEBUG
  printf("done in %f\n", ccclocksecs(ccclocktick()-tick));
#endif

ccleave("exec");

  ccread_uninit(&read);

ccenter("compare");
	int c=fib(ARG);
  cctracelog("c:%i - cc:%i",c,retr.asi32);
ccleave("compare");

#if 0
ccenter("emit-c");
  // Todo:
  { char *out = ccnil;

    ccstrcatS(out,
      "#define f64 double\r\n"
      "#define f32 float\r\n"
      "#ifdef _MSC_VER\r\n"
      "# define i64 signed   __int64\r\n"
      "# define u64 unsigned __int64\r\n"
      "# define i32 signed   __int32\r\n"
      "# define u32 unsigned __int32\r\n"
      "# define i16 signed   __int16\r\n"
      "# define u16 unsigned __int16\r\n"
      "# define i8  signed   __int8\r\n"
      "# define u8  unsigned __int8\r\n"
      "#else\r\n"
      "# define i64 signed   long long\r\n"
      "# define u64 unsigned long long\r\n"
      "# define i32 signed   int\r\n"
      "# define u32 unsigned int\r\n"
      "# define i16 signed   short\r\n"
      "# define u16 unsigned short\r\n"
      "# define i8  signed   char\r\n"
      "# define u8  unsigned char\r\n"
      "#endif\r\n"
      "#define c8  char\r\n"
      "#define c16 wchar_t\r\n");

    static const kttcc_type ts[] =
    { {kttcc_typekind_var, kttcc_declspec_float,    64, 32 },
      {kttcc_typekind_var, kttcc_declspec_signed,   64, 8  },
      {kttcc_typekind_var, kttcc_declspec_unsigned, 64, 8  },
      {kttcc_typekind_var, kttcc_declspec_host,      8, 8  },
    };


    for(int i=0;i<ARRAYSIZE(ts);++i)
    { emit_type(&out,&ts[i]);
    }

    void *file=ccopenfile("gen.ktt.c");
    ccpushfile(file,0,ccstrlen(out),out);
    ccclosefile(file);
  }
ccleave("emit-c");
#endif
ccleave("main");
ccdebugend();
}