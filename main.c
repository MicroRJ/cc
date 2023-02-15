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

ccfunc ccexec_value_t buildrunfile(const char *filename)
{


ccenter("build-run-file");

	ccexec_value_t result;

  ccread_t read;
  ccemit_t emit;
  ccexec_t exec;

  ccread_init(&read);
	ccemit_init(&emit);
  ccexec_init(&exec);

ccenter("read");
  ccread_include(&read,filename);
  cctree_t *tree;
  tree=ccread_translation_unit(&read);
ccleave("read");

	// Todo: this is global for now!
	ccseek_translation_unit(tree);

ccenter("emit");
	ccemit_translation_unit(&emit,tree);
ccleave("emit");

ccenter("exec");
  result=ccexec_translation_unit(&exec,&emit);
ccleave("exec");

  ccexec_uninit(&exec);
  ccread_uninit(&read);

  cctracelog("file %s: %i",filename,result.asi32);

ccleave("build-run-file");
  return result;
}

int main(int argc, char **argv)
{
	ccini();

ccenter("main");
  ++ argv;
  -- argc;

#if 0
  testdbgsys();
#else

  buildrunfile("code\\decl.cc");
  buildrunfile("code\\lval.cc");


#if 0
  ccexec_value_t retr=buildrunfile("code\\fib.cc");
ccenter("compare");
	int c=fib(ARG);
ccleave("compare");
  cctracelog("c:%i - cc:%i",c,retr.asi32);
#endif

#endif


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