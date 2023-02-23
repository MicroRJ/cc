// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCREAD_H
#define _CCREAD_H

// Implementation based on the specs at:
// https://learn.microsoft.com/en-us/cpp/c-language/c-language-reference

typedef enum cctoken_k
{
  cctoken_kINVALID=0,
  /**
   * Group: syntactic operators.
   **/
  cctoken_kEND,

  cctoken_Kendimpl, // '\r\n'
  cctoken_Kendexpl, // ';'

  cctoken_kLPAREN,  // '('
  cctoken_kRPAREN,  // ')'
  cctoken_Klcurly,  // '{'
  cctoken_Krcurly,  // '}'
  cctoken_kLSQUARE, // '['
  cctoken_kRSQUARE, // ']'

  cctoken_kCMA,   // ','
  cctoken_Kcolon, // ':'
  cctoken_Kspace, // ' '
  /**
   * Group: literals.
   **/
  cctoken_Kliteral_ellipsis,
  cctoken_Kliteral_comment,
  cctoken_Kliteral_character,
  cctoken_kLITSTR,
  cctoken_Kliteral_string_format,
  cctoken_kLITSTR_INVALID,
  cctoken_kLITIDENT,
  cctoken_kLITINT,
  cctoken_kLITFLO,
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

  // Note: type specifiers ...
  cctoken_kVOID,
  cctoken_kSTDC_INT,
  cctoken_kSTDC_LONG,
  cctoken_kSTDC_SHORT,
  cctoken_kSTDC_DOUBLE,
  cctoken_kSTDC_FLOAT,
  cctoken_kSTDC_CHAR,
  cctoken_kSTDC_BOOL,     // _Bool
  cctoken_kSTDC_SIGNED,   // group start
  cctoken_kSTDC_UNSIGNED,
  cctoken_kMSVC_INT8,     // __int8
  cctoken_kMSVC_INT16,    // __int16
  cctoken_kMSVC_INT32,    // __int32
  cctoken_kMSVC_INT64,    // __int64
  cctoken_kENUM,
  cctoken_kSTRUCT,

  // Todo:
  // cctoken_Kcomplex,       // _Complex
  // cctoken_Katomic,        // _Atomic

  // Note: storage classes
  cctoken_Ktypedef,
  cctoken_Kauto,
  cctoken_Kextern,
  cctoken_Kregister,
  cctoken_Kstatic,
  cctoken_Kthread_local, // _Thread_local
  cctoken_Kmsvc_declspec, // __declspec

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
  cctoken_kDEREFERENCE,
  cctoken_kADDRESSOF,
  cctoken_Kbitwise_invert,

  cctoken_Kpositive, // +
  cctoken_Knegative, // -
  //
  cctoken_Knegate, // !
  cctoken_kDOT, // .
  cctoken_kARROW, // ->
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
  cctoken_kTEQ,
  cctoken_kFEQ,

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

  cctoken_kPDE,
  cctoken_kPIN,

  cctoken_Kpos_decrement,
  cctoken_Kpos_increment,


  // Todo: CC tokens ...
  // cctoken_kCCASSERT,
  // cctoken_kCCBREAK,
  // cctoken_kCCERROR,
} cctoken_k;

#define ccclassic(T) (ccclassic_t){cccast(cci64_t,T)}

typedef struct ccclassic_t
{ union
  { cci64_t   asi64; ccu64_t   asu64;
    ccu32_t   asi32; ccu32_t   asu32;
    ccu16_t   asi16; ccu16_t   asu16;
    ccu8_t    asi8;  ccu8_t    asu8;
    ccf64_t   asf64; ccf32_t   asf32;
    char     *value;
  };
} ccclassic_t;

typedef struct ccloca_t ccloca_t;
typedef struct ccloca_t
{ int idx;
  int row, col;
} ccloca_t;

typedef struct cctoken_t
{
  cctoken_k bit;

  const char *loc;

  unsigned  term_impl: 1;
  unsigned  term_expl: 1;
  union
  { cci64_t asi64;
    ccu64_t asu64;
    ccf64_t asf64;
    ccstr_t str;
  };
} cctoken_t;

// Note: merge the lexer and reader and make the symbol table global?
typedef struct cclex_t
{
  const char *doc_max, *doc_min;
  const char *max, *min;

  cctoken_k *tbl;

#if 0
  ccu32_t    tbl_max;
  ccu32_t    tbl_min;
  ccentry_t *tbl;
  ccu32_t    tbl_dbg;
#endif

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
#define ccsynerr(tok,cod,fmt, ...) 0
#define ccsynwar(tok,cod,fmt, ...) 0


// Todo: temporary
ccfunc const char *
cctoken_to_string(cctoken_k kind)
{ switch(kind)
  {
    case cctoken_kINVALID:       return "invalid";

    case cctoken_kVOID:          return "void";
    case cctoken_kSTDC_INT:      return "int";
    case cctoken_kSTDC_LONG:     return "long";
    case cctoken_kSTDC_SHORT:    return "short";
    case cctoken_kSTDC_DOUBLE:   return "double";
    case cctoken_kSTDC_FLOAT:    return "float";
    case cctoken_kSTDC_CHAR:     return "char";
    case cctoken_kSTDC_BOOL:     return "_Bool";
    case cctoken_kSTDC_SIGNED:   return "signed";
    case cctoken_kSTDC_UNSIGNED: return "unsigned";
    case cctoken_kMSVC_INT8:     return "__int8";
    case cctoken_kMSVC_INT16:    return "__int16";
    case cctoken_kMSVC_INT32:    return "__int32";
    case cctoken_kMSVC_INT64:    return "__int64";
  }

  ccassert(!"error");
  return "error";
}



#endif