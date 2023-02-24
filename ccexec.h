// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCEXEC_VALUE
#define _CCEXEC_VALUE

typedef enum ccexec_value_k ccexec_value_k;
typedef enum ccexec_value_k
{
  ccexec_value_kINVALID  = 0,
  ccexec_value_kCONSTANT = 1,
  ccexec_value_kADDRESS  = 2,
} ccexec_value_k;

typedef void *ccaddress_t;

typedef struct ccexec_value_t ccexec_value_t;
typedef struct ccexec_value_t
{ ccexec_value_k    kind;

  union
  { ccaddress_t * address;
    void        * value;
    cci64_t       constI;
    ccu64_t       constU;
    ccf64_t       constR;
  };
} ccexec_value_t;

typedef struct ccexec_frame_t ccexec_frame_t;
typedef struct ccexec_frame_t
{ ccprocd_t      * procedure;
  ccblock_t      * current;
  cci32_t          irindex;
  ccexec_value_t * values;
  ccexec_value_t   result;
} ccexec_frame_t;

typedef struct ccexec_t ccexec_t;
typedef struct ccexec_t
{ ccemit_t * emit;

  void     * stack;
  cci64_t    stack_sze;
  cci64_t    stack_idx;
} ccexec_t;

#endif