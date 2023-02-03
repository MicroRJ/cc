#include "kttc.h"


int main(int argc, char **argv)
{

  ++ argv;
  -- argc;

  ccreader_t reader;
  ccreader_init(& reader);
  ccreader_file(& reader, "test.svm.c");

  cctree_t *stats=ccread_translation_unit(& reader);

  ccsvm_exectree(stats);

  ccreader_uninit(& reader);



  // Todo:
  { char *out = ccstrnil;

    ccstrcat(out,
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

}