/*----------------------------------------------------------------------------
dsort.c

Sorts a linked list of _D_ITEMS into order of points closest to the points
furthest from the dimension point indicated by the user. The linked list
of _D_ITEMS is references by the list handle in type _DIM

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
#include "dsort.h"
#include "dextx.h"

static int sort_dim_pts(_DIM *dim)
{
   ads_real    lo_angle = 0.0;
   _D_ITEM     *x_pts;
   _D_ITEM     *x_pts_lo;
   int         num_elems = 0;
   int         min;
   int         i;
   int         j;
   
   num_elems = l_count(dim->item_ll);

   for (i = 0; i < num_elems - 1; i++)
   {
      if ((x_pts = (_D_ITEM *)l_nth(dim->item_ll, i)) == (void *)NULL)
         return(DIM_ERR);

      min = i;
      lo_angle = x_pts->e_base_dim_loc_angle;

      for (j = i + 1; j < num_elems; j++)
      {
         if ((x_pts_lo = (_D_ITEM *)l_nth(dim->item_ll, j)) == (void *)NULL)
            return(DIM_ERR);

         if (x_pts_lo->e_base_dim_loc_angle < lo_angle)
         {
            min = j;
            lo_angle = x_pts_lo->e_base_dim_loc_angle;
         }
      }

/*      ads_printf("Swapping %d with %d on handle %d from dim->item_ll %d\n", i, min, (int)dim->item_ll, (int)dim->item_ll); */
      if (i != min)
         if (l_swap(dim->item_ll, i, min) == LIST_ERR)
            return(DIM_ERR);

   }

   return(DIM_OK);
}

int sort_dimensions(_DIM *dim)
{
   /* build list of intersection points betwwen extension lines 
      and the dimension line */

   if (build_dim_x(dim) == DIM_ERR)
      return(DIM_ERR);

   if (sort_dim_pts(dim) == DIM_ERR)
      return(DIM_ERR);

   return(DIM_OK);
}
