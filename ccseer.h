// Copyright(C) J. Dayan Rodriguez, 2022,2023 All rights reserved.
#ifndef _CCSEER_H
#define _CCSEER_H

typedef struct ccseer_t ccseer_t;
typedef struct ccseer_t
{
	cctree_t  **tdecl;
	cctree_t  **fdecl;
	cctree_t  **vdecl;

	// Note: this is deprecated now ...
	cctree_t  **symbols;
} ccseer_t;

#endif