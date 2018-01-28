/*
 * m_stats.h : This header contains the stats for each memeber
 */

#ifndef MEMBER_STATS_INCLUDE

#include "adsinc.h"
#include "standard.h"

typedef enum {
	M_TYPE_Z,
	M_TYPE_C,
	M_TYPE_TFC,
	M_TYPE_PFC,
	M_TYPE_L,
	M_TYPE_T,
	M_TYPE_TFT,
	M_TYPE_UB,
	M_TYPE_UC,
	M_TYPE_WB,
	M_TYPE_WC,
	M_TYPE_TFB,
	M_TYPE_RHS,
	M_TYPE_SHS,
	M_TYPE_CHS,
	M_TYPE_SQUARE,
	M_TYPE_FLAT,
	M_TYPE_ROUND,
	M_TYPE_RAILS,
	M_TYPE_PLATE,
	M_TYPE_SPLATE,
	M_TYPE_BOX,
	M_TYPE_PWG,
	M_TYPE_MAX	/* Max number */
} MEMBER_TYPE;

#define MAX_MEMBER_DESC_STR 40

typedef struct {
	MEMBER_TYPE mtype;	/* Type of member */

	char desc[MAX_MEMBER_DESC_STR];	/* members description */

	ads_real depth;
	ads_real width;		/* Top */
	ads_real width_b;	/* Bottom width */

	ads_real flange;
	ads_real web;		/* Top */
	ads_real web_b;		/* Bottom web */

	ads_real top;
	ads_real bottom;
	ads_real lip;

	ads_real thick;

	ads_real radius_1;
	ads_real radius_2;

	ads_real taper;

	ads_real grav_x;
	ads_real grav_y;

	ads_real fl_off_1;
	ads_real fl_off_2;

	ads_real gauge_x;
	ads_real gague_y;

	ads_real mass;
} MEMBER_STATS;

#define MEMBER_STATS_INCLUDE

#endif 	/* MEMBER_STATS_INCLUDE */

