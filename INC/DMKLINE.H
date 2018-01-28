#ifndef _DDRAW_H
#define _DDRAW_H

#include "dmain.h"
#include "llistman.h"

/*
   'types' to draw
*/

#define     DIM_LINE    1
#define     EXT_LINE    2
#define     TEXT        3
#define     BLOCK       4

#define     MAX_TEXT_SZ 30

typedef struct
{
   int               tag;
                              /* line information */
   ads_point         start;
   ads_point         end;
   char              *layer;
   int               hide_line;
} _D_line;

typedef struct
{
   int               tag;
                              /* text information */
   char              text[MAX_TEXT_SZ];
   ads_real          angle;
   ads_point         insert_pt;
   char              *layer;
   ads_real          height;
   ads_real          coord_dist;

   int               repeater;

   /* second pass variables */
   ads_name          tname;         /* text entity */
   ads_real          width;         /* width of text */
   ads_real          avail_dist;    /* distance available (total) not occupied by text */
   ads_point         n_insert_pt;   /* point where text is now to be placed */
   int               placement;     /* method by which text is eventually placed */
   int               flipped;       /* text flipped around */
} _D_text;

typedef struct
{
   int               tag;
                              /* block information */
   char              *name;
   ads_point         insert_pt;
   ads_real          angle;
   ads_real          x_scale;
   ads_real          y_scale;
   char              *layer;

} _D_block;

typedef union 
{
   _D_line           l;
   _D_text           t;
   _D_block          b;

} _D_draw_dets;

typedef struct
{
   int               type;
   _D_draw_dets      d;

} _D_DRAW;

int build_dim_lines(_DIM *);

#endif
