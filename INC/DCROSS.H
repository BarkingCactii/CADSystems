#ifndef _DCROSS_H
#define _DCROSS_H

#include "dmain.h"
#include "dmkline.h"

#define DIM_CROSSING    1
#define DIM_PARALLEL    2

#define ENTITY_NAME  -1
#define ENTITY_START 0
#define ENTITY_LAYER 8
#define LINE_START   10
#define LINE_END     11

#define LINE_ENTITY  "LINE"

typedef struct
{
   ads_name    line_entity;
   ads_point   p1;
   ads_point   p2;
   ads_real    angle;
   int         crossing_type;
   ads_point   inters_pt;

} _DIM_NODE;

int line_on_layer(struct resbuf *, char *);
int line_on_ext_layer(struct resbuf *, _DIM *);
int line_entity(struct resbuf *);
int dim_x(_D_DRAW *, _DIM *);

#endif
