#ifndef _DMAIN_H
#define _DMAIN_H

#include "llistman.h"

#define DIM_ERR         0
#define DIM_OK          1

#define MAX_SUFFIX_SZ   80
#define MAX_PREFIX_SZ   80
#define MAX_LAYER_SZ    30
#define MAX_COLOR_SZ    10

#define ARROW_T         1              /* mark types */
#define TICK_T          2

#define METRIC_OUTPUT   0

typedef struct
{
   ads_point   e_base_pt;              /* extension base point */
   char        suffix[MAX_SUFFIX_SZ];  /* text to suffix dimension text - future */
   char        prefix[MAX_PREFIX_SZ];  /* text to prefix dimension text - future */

   ads_point   e_d_inters_pt;          /* intersection point between extension
                                          and dimension line */
   ads_real    e_base_dim_loc_angle;   /* angle of line from extension base
                                          point to diminsion location point */
} _D_ITEM;

typedef struct
{
   int         format;                 /* output text in which format */

   ads_real    rot_lower_limit;        /* lowest angle before text is rotated */
   ads_real    rot_upper_limit;        /* highest angle before text is rotated */

   ads_real    flip_lower_limit;       /* lowest angle before text is flipped to other side of dimension */
   ads_real    flip_upper_limit;       /* lowest angle before text is flipped to other side of dimension */
} _D_TEXT_ATTRIB;

typedef struct
{
   char        dlayer[MAX_LAYER_SZ];   /* layer to place dimensions on */
   char        dlayerc[MAX_COLOR_SZ];  /* color for dimension layer */
   char        elayer[MAX_LAYER_SZ];   /* layer to place extensions */
   char        elayerc[MAX_COLOR_SZ];  /* color for extensions layer */
   char        tlayer[MAX_LAYER_SZ];   /* layer to place text on */
   char        tlayerc[MAX_COLOR_SZ];  /* color for text layer */
} _D_LAYERS;

typedef struct
{
   ads_point   start;                  /* reference line start point */
   ads_point   end;                    /* reference line end point */
   ads_real    angle;                  /* reference line angle */

   ads_point   dim_start;              /* dimension line start point */
   ads_point   dim_end;                /* dimension line end point */
   ads_real    dim_angle;              /* angle of dimension line */

} _D_REF;

typedef struct
{
   ads_real    _dimasz;                /* arrow size */
   short       _dimclrd;               /* dimension and marks line color */
   short       _dimclre;               /* extension line color */
   short       _dimclrt;               /* dimension text color */
   ads_real    _dimexo;                /* extension line offset */
   ads_real    _dimtsz;                /* tick size */
   ads_real    _dimtvp;                /* text vertical position */
   ads_real    _dimtxt;                /* text size */
   ads_real    _dimscale;              /* dimension feature scale factor */
   ads_real    _dimexe;                /* extension line extension */

} _D_ACADVARS;

typedef struct 
{
   _L_HANDLE      item_ll;             /* handle to _D_ITEM linked list */
   _L_HANDLE      draw_ll;             /* handle to _D_DRAW linked list */
   _L_HANDLE      cross_ll;            /* handle to _D_DRAW crossed lines */

   _D_LAYERS      l;                   /* layers */

   _D_REF         r;                   /* first line reference info */

   _D_ACADVARS    v;                   /* autocad variables used for dimensioning */

   ads_point      dim_loc;             /* dimension location */

   int            multi_space;         /* display 'n @ xxx' format */
   int            mark_type;           /* type of dimension mark */
   char           dim_marks[30];       /* block name of dimension marks */

   int            draw_ext_lines;      /* draw extension lines */
   int            draw_end_lines;      /* draw end lines */
   int            break_lines;         /* break up intersecting extensions */
   int            last_line;           /* TRUE if last line being processed */

   _D_TEXT_ATTRIB a;                   /* text related attributes */

} _DIM;

int dimension(_DIM *);

#endif


