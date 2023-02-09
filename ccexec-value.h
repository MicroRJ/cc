#ifndef _CCEXEC_VALUE
#define _CCEXEC_VALUE

typedef enum ccexec_value_k
{
	ccexec_value_kINVALID=0,

	ccexec_value_kCONST,
	ccexec_value_kADDRS,
} ccexec_value_k;

typedef struct ccexec_value_t
{ const char      * debug_label;
	ccexec_value_k    kind;
	cctree_t        * type;
	union
	{ void * value;
		ccf64_t  asf64;
		cci64_t  asi64;
		ccu64_t  asu64;

		ccf32_t  asf32;

		ccu32_t  asu32;
		ccu16_t  asu16;
		ccu8_t   asu8;

		ccu32_t  asi32;
		ccu16_t  asi16;
		ccu8_t   asi8;
	};
} ccexec_value_t;

ccfunc ccinle ccexec_value_t
ccexec_value_I(ccexec_value_k kind, cctree_t *type, void *value, const char *debug_label)
{
	ccexec_value_t t;
	t.kind=kind;
	t.type=type;
	t.value=value;
	t.debug_label=debug_label;
	return t;
}

ccfunc ccinle ccexec_value_t
ccexec_value_i64(cctree_t *type, cci64_t val, const char *debug_label)
{ ccexec_value_t t;
	t.kind=ccexec_value_kCONST;
	t.debug_label=debug_label;
	t.type=type;
	t.asi64=val;
	return t;
}

ccfunc ccinle ccexec_value_t
ccexec_value_u64(cctree_t *type, ccu64_t val, const char *debug_label)
{ ccexec_value_t t;
	t.kind=ccexec_value_kCONST;
	t.debug_label=debug_label;
	t.type=type;
	t.asu64=val;
	return t;
}

ccfunc ccinle ccexec_value_t
ccexec_value_f64(cctree_t *type, ccf64_t val, const char *debug_label)
{ ccexec_value_t t;
	t.kind=ccexec_value_kCONST;
	t.debug_label=debug_label;
	t.type=type;
	t.asf64=val;
	return t;
}

#endif