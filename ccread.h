/*****************************************************************/
/** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved. **/
/*****************************************************************/
#ifndef _CCREAD
#define _CCREAD

// Implementation based on the:
// https://learn.microsoft.com/en-us/cpp/c-language/c-language-reference
//

typedef enum cctoken_k
{
  cctoken_Kinvalid = - 1, // <-- for the hash table this means that it wasn't there before.
  /**
   * Group: syntactic operators.
   **/
  cctoken_Kend     =   0,

  // ** this is up to the lexer to determine and the parser to interpret **
  cctoken_Kendimpl, // '\r\n'
  cctoken_Kendexpl, // ';'

  cctoken_kLPAREN,  // '('
  cctoken_kRPAREN,  // ')'
  cctoken_Klcurly,  // '{'
  cctoken_Krcurly,  // '}'
  cctoken_Klsquare, // '['
  cctoken_Krsquare, // ']'

  cctoken_Kcomma, // ','
  cctoken_Kcolon, // ':'
  cctoken_Kspace, // ' '

  /**
   * Group: literals.
   **/
  cctoken_Kliteral_ellipsis,
  cctoken_Kliteral_comment,
  cctoken_Kliteral_character,
  cctoken_kLITSTRING,
  cctoken_Kliteral_string_format,
  cctoken_Kliteral_string_unterminated,
  cctoken_kLITIDENT,
  cctoken_kLITINTEGER,
  cctoken_kLITFLOAT,
  /**
   * Group: msvc attributes.
   *
   *  ** these are reserved keywords **
   **/
  cctoken_Kmsvc_attr_asm,       // maps to:  __asm
  cctoken_Kmsvc_attr_based,     // maps to:  __based
  cctoken_Kmsvc_attr_cdecl,     // maps to:  __cdecl
  cctoken_Kmsvc_attr_clrcall,   // maps to:  __clrcall
  cctoken_Kmsvc_attr_fastcall,  // maps to:  __fastcall
  cctoken_Kmsvc_attr_inline,    // maps to:  __inline
  cctoken_Kmsvc_attr_stdcall,   // maps to:  __stdcall
  cctoken_Kmsvc_attr_thiscall,  // maps to:  __thiscall
  cctoken_Kmsvc_attr_vectorcal, // maps to:  __vectorcal
  /**
   * Group: alignment specifiers
   *
   * ** these are reserved keywords **
   **/
  kttc__algn_spec_0,
  cctoken_Kalign_of, // maps to: _Alignof
  cctoken_Kalign_as, // maps to: _Alignas
  kttc__algn_spec_1,
  /**
   * Group: type qualifiers
   *
   * ** these are reserved keywords **
   **/
  cctype_qual_0,
  cctoken_Kconst,    // maps to: const
  cctoken_Krestrict, // maps to: restrict
  cctoken_Kvolatile, // maps to: volatile
  cctype_qual_1,
  /**
   * Group: function specifiers.
   *
   * ** these are reserved keywords **
   **/
  kttc__func_spec_0,
  cctoken_Kinline,
  cctoken_Kno_return,
  kttc__func_spec_1,
  /**
   * Group: type specifiers.
   *
   * ** these are reserved keywords **
   **/
  cctype_spec_0,
  cctoken_Ksigned, // group start
  cctoken_Kunsigned,
  cctoken_Kmsvc_int8,  // __int8
  cctoken_Kmsvc_int16, // __int16
  cctoken_Kmsvc_int32, // __int32
  cctoken_Kmsvc_int64, // __int64
  cctoken_Kdouble,
  cctoken_Kfloat,
  cctoken_Klong,
  cctoken_Kint,
  cctoken_Kshort,
  cctoken_Kchar,
  cctoken_Kvoid,
  cctoken_Kbool,     // _Bool
  cctoken_Kcomplex,  // _Complex
  cctoken_Katomic,   // _Atomic
  cctoken_Kenum,
  cctoken_kSTRUCT,
  /**
   * Group: type specifier & storage class.

   * This is a bit wanly but it makes so that I don't have edge cases.
   **/
  kttc__scls_spec_0,

  cctoken_Ktypedef,

  cctype_spec_1,
  /*
   * Group: storage class.

   * ** these are reseverd keywords **
   **/
  cctoken_Kauto,
  cctoken_Kextern,
  cctoken_Kregister,
  cctoken_Kstatic,
  cctoken_Kthread_local, // _Thread_local
  cctoken_Kmsvc_declspec, // __declspec

  kttc__scls_spec_1,

  /**
   * Group: control statements.

   * ** these are reseverd keywords **
   **/
  cctoken_Kif,
  cctoken_Kswitch,

  cctoken_Kelse,
  cctoken_Kcase,
  cctoken_Kdefault,

  cctoken_Kfor,
  cctoken_Kwhile,
  cctoken_Kdo,

  cctoken_Kgoto,
  cctoken_Kreturn,
  cctoken_Kbreak,
  cctoken_Kcontinue,
  /**
   * Group: operators.
   *
   * ** listed from highest precedence groups to lowest **
   **/
  // Group: unary
  //
  // ** these are not generated by the lexer **
  cctoken_Kptr_dereference,
  cctoken_Kptr_address_of,
  cctoken_Kbitwise_invert,

  cctoken_Kpositive, // +
  cctoken_Knegative, // -
  //
  cctoken_Knegate, // !
  cctoken_Kmso, // .
  cctoken_Kmsp, // ->
  //
  // Group: multiplicative
  cctoken_kMUL,
  cctoken_kDIV,
  cctoken_Kmod,

  // Group: additive
  cctoken_kADD,
  cctoken_kSUB,

  // Group: shift
  cctoken_Kbitwise_shl,
  cctoken_Kbitwise_shr,

  // Group: relational
  cctoken_kGTN,
  cctoken_kLTN,

  // Group: equality
  cctoken_Kequals,
  cctoken_Knot_equals,

  // Group: bit-wise and
  cctoken_Kbitwise_and,

  // Group: bit-wise xor
  cctoken_Kbitwise_xor,

  // Group: bit-wise or
  cctoken_Kbitwise_or,

  // Group: logical and
  cctoken_Klogical_and,

  // Group: logical or
  cctoken_Klogical_or,

  // Group: conditional
  cctoken_Kconditional,

  // Group: assignment
  cctoken_kASSIGN,
  cctoken_Kmul_eql,
  cctoken_Kdiv_eql,
  cctoken_Kmod_eql,
  cctoken_Kadd_eql,
  cctoken_Ksub_eql,
  cctoken_Kbitwise_shl_eql,
  cctoken_Kbitwise_shr_eql,
  cctoken_kGTE,
  cctoken_kLTE,
  cctoken_Kbitwise_and_eql,
  cctoken_Kbitwise_xor_eql,
  cctoken_Kbitwise_or_eql,

  cctoken_Kpre_decrement,
  cctoken_Kpre_increment,

  cctoken_Kpos_decrement,
  cctoken_Kpos_increment,

} cctoken_k;

typedef struct ccclassic_t
{ union
  {
    cci64_t   asi64; ccu64_t   asu64;
    ccu32_t   asi32; ccu32_t   asu32;
    ccu16_t   asi16; ccu16_t   asu16;
    ccu8_t    asi8;  ccu8_t    asu8;

    ccf64_t   asf64; ccf32_t   asf32;
    void    * asptr;
  };
} ccclassic_t;

typedef struct ccloc_t ccloc_t;
typedef struct ccloc_t
{ const char *file;
  const char *func;
  const char *clss;
  int row, col;
} ccloc_t;

typedef struct ccentry_t ccentry_t;
typedef struct ccentry_t
{ ccentry_t   * nex;
  char        * key;
  int           len;
  int           bit;
} ccentry_t;

// Tokens produced by the lexer are very simple, so the parser may modify them after lexical analysis,
// the lexer however, will check for keywords and format strings.
typedef struct cctoken_t
{
  cctoken_k bit;

  union
  { unsigned long long int sig;
    unsigned long long int uns;
    double                 flo;
    ccstr_t                str;
  };

#ifdef _DEBUG
  const char *doc;
#endif

  ccloc_t loc;

  unsigned term_impl: 1;
  unsigned term_expl: 1;
} cctoken_t;

// Note: merge the lexer and reader and make the symbol table global?
typedef struct cclex_t
{ const char *doc_max, *doc_min;
  const char *max,     *min; // <-- points to the beginning and end of the token after its been parsed.

// Todo: remove this ....
  ccu32_t    tbl_max;
  ccu32_t    tbl_min;
  ccentry_t *tbl;
  ccu32_t    tbl_dbg;

  cctoken_t  tok;
} cclex_t;

typedef struct ccread_t
{
  cclex_t    lex;
  cctoken_t *buf;
  cctoken_t *max;
  cctoken_t *min;
  cctoken_t *bed;
} ccread_t;

// TODO(RJ): make this legit!
#ifndef ccsynerr
#ifdef _DEBUG
# define ccsynerr(tok,cod,fmt, ...) cctraceerr(fmt, __VA_ARGS__), ccbreak()
#else
# define ccsynerr(tok,cod,fmt, ...) cctraceerr(fmt, __VA_ARGS__)
#endif
#endif
#ifndef ccsynwar
# define ccsynwar(tok,cod,fmt, ...) cctracewar(fmt, __VA_ARGS__)
#endif

#endif