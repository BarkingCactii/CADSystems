/*----------------------------------------------------------------------------
dextx.c

Builds a linked list of points representing the intersection between
all extension lines and the dimension line. This linked list is then
used to sort the points.

by Jeff Hill
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "adslib.h"
#include "ads.h"

#include "alistman.h"
#include "llistman.h"
#include "geom.h"
#include "dmain.h"

int build_dim_x(_DIM *dim)
{
   _D_ITEM     *dim_item;

   ads_real    ext_angle = 0.0;
   ads_real    curr_ext_angle;
   ads_point   inters_pt;
   ads_real    curr_ext_len;
   ads_real    adj_len;

   if ((dim_item = (_D_ITEM *)l_head(dim->item_ll)) == (_D_ITEM *)NULL)
      return(DIM_ERR);

   /* calculate angle of known line */
   ext_angle = dim->r.angle;
   if (ext_angle >= _180_DEG)
      ext_angle -= _180_DEG;

   /* convert to radians */
   ext_angle = degtorad(ext_angle);

   while (dim_item)
   {
      curr_ext_len = distance(dim->r.start, dim_item->e_base_pt);
      curr_ext_angle = fabs(angle(dim_item->e_base_pt, dim->r.start) - radtodeg(ext_angle));
      curr_ext_angle = degtorad(curr_ext_angle);

      adj_len = cos(curr_ext_angle) * curr_ext_len;

      ads_polar(dim_item->e_base_pt, ext_angle, adj_len, inters_pt);

      memcpy((void *)dim_item->e_d_inters_pt, (void *)inters_pt, sizeof(ads_point));

      dim_item->e_base_dim_loc_angle = dim->r.angle - angle(dim->r.end, inters_pt);
      dim_item->e_base_dim_loc_angle = adjust_angle(dim_item->e_base_dim_loc_angle);

      if (dim_item->e_base_dim_loc_angle > _180_DEG)
         dim_item->e_base_dim_loc_angle = dim_item->e_base_dim_loc_angle - _360_DEG;
      else if (dim_item->e_base_dim_loc_angle == _180_DEG)
         dim_item->e_base_dim_loc_angle = 0.0;

      dim_item = (_D_ITEM *)l_next(dim->item_ll);
   }

   return(DIM_OK);
}
