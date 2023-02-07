#ifndef _CCEXEC_VALUE
#define _CCEXEC_VALUE

typedef struct ccexec_value_t
{ const char *debug_label;

	cctype_t     *type;

  ccclassic_t   clsc;
  void         *addr;

  unsigned      is_const: 		  1;
  unsigned      is_edict_value: 1;
} ccexec_value_t;


#endif