/*----------------------------------------------------------------------------
drepeat.c

Handles repeated dimensions

by Jeff Hill
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "adslib.h"
#include "ads.h"

#include "llistman.h"
#include "geom.h"
#include "dmain.h"
#include "dmkline.h"
#include "dcross.h"
#include "dtxtwid.h"
#include "dinterp.h"

#define MAX_TEXT_DIMS   100
   
typedef struct
{
   char        *distance;
   int         repeat;
   int         num;
   int         total;
} DIM_DISTANCE;

int calculate_repeat_dims(_DIM *dim)
{
   _D_DRAW        *draw_item;
   DIM_DISTANCE   dd[MAX_TEXT_DIMS];
   int            num_dims = 0;
   int            curr_dim = 0;
   int            i;
   int            j;
   char           curr_dist[MAX_TEXT_SZ];
   ads_real       offset;
   char           new_text[MAX_TEXT_SZ];

   if (!dim->multi_space)
      return(DIM_OK);

   if ((draw_item = (_D_DRAW *)l_head(dim->draw_ll)) == (void *)NULL)
      return(DIM_ERR);

   while (draw_item)
   {
      if (draw_item->type == TEXT)
         dd[num_dims++].distance = draw_item->d.t.text;

      if (num_dims >= MAX_TEXT_DIMS - 1)
         break;

      draw_item = (_D_DRAW *)l_next(dim->draw_ll);
   }

   /* parse the table */
   dd[0].repeat = FALSE;
   dd[0].num = 0;
   dd[0].total = 1;

   for (i = 1, strcpy(curr_dist,dd[0].distance); i < num_dims; i++)
   {
      if (!strcmp(dd[i].distance,curr_dist))
      {
         dd[i - 1].repeat = TRUE;
         dd[i].repeat = TRUE;
         dd[i].num = dd[i - 1].num + 1;
         dd[i].total = dd[i - 1].total + 1;

         for (j = 1; j < dd[i].total; j++)
            dd[i - j].total = dd[i].total;
      }
      else
      {
         dd[i].repeat = FALSE;
         dd[i].num = 0;
         dd[i].total = 1;
      }
      strcpy(curr_dist,dd[i].distance);
   }

   if ((draw_item = (_D_DRAW *)l_head(dim->draw_ll)) == (void *)NULL)
      return(DIM_ERR);


   while (draw_item)
   {
      if (draw_item->type == TEXT)
      {
         if (dd[curr_dim].repeat)
         {
            if (dd[curr_dim].num == 0)
            {
               /* make new text */
               memset(new_text, 0, sizeof(new_text));
               sprintf(new_text, "%d@", dd[curr_dim].total);
               strcat(new_text, draw_item->d.t.text);
               strcpy(draw_item->d.t.text, new_text);

               offset = (( (ads_real)dd[curr_dim].total - 1.0) / 2.0) * draw_item->d.t.coord_dist;
               ads_polar(draw_item->d.t.insert_pt, degtorad(dim->r.dim_angle), offset, draw_item->d.t.insert_pt);
               draw_item->d.t.repeater = TRUE;

            }
            else
               draw_item->d.t.text[0] = 0;
         }
         curr_dim++;
      }
      draw_item = (_D_DRAW *)l_next(dim->draw_ll);
   }

   return(DIM_OK);
}

