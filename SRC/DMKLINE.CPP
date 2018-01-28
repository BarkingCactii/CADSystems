/*----------------------------------------------------------------------------
dmkline.c

Builds a linked list of extension and dimension lines to be drawn by autocad

by Jeff Hill
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "adslib.h"
#include "ads.h"

#include "llistman.h"
#include "geom.h"
#include "dmain.h"
#include "dextx.h"
#include "dmkline.h"
#include "mettoimp.h"

static int add_block_node(_DIM *dim, _D_ITEM *i1, int tag)
{
   _D_DRAW     *draw_item;

   if (dim->mark_type == TICK_T)
   {
      if (l_add(dim->draw_ll, (void *)&draw_item, sizeof(_D_DRAW)) == LIST_ERR)
         return(DIM_ERR);

      draw_item->type = BLOCK;
      memcpy((void *)draw_item->d.b.insert_pt, (void *)i1->e_d_inters_pt, sizeof(ads_point));
      draw_item->d.b.name = dim->dim_marks;
      draw_item->d.b.angle = dim->r.dim_angle;
      draw_item->d.b.x_scale = dim->v._dimscale * dim->v._dimtsz;
      draw_item->d.b.y_scale = dim->v._dimscale * dim->v._dimtsz;
      draw_item->d.b.layer = dim->l.dlayer;
      draw_item->d.b.tag = tag;
   }

   if (dim->mark_type == ARROW_T)
   {
      if (l_add(dim->draw_ll, (void *)&draw_item, sizeof(_D_DRAW)) == LIST_ERR)
         return(DIM_ERR);

      draw_item->type = BLOCK;
      memcpy((void *)draw_item->d.b.insert_pt, (void *)i1->e_d_inters_pt, sizeof(ads_point));
      draw_item->d.b.name = dim->dim_marks;
      draw_item->d.b.angle = dim->r.dim_angle;
      draw_item->d.b.x_scale = dim->v._dimscale * dim->v._dimasz;
      draw_item->d.b.y_scale = dim->v._dimscale * dim->v._dimasz;
      draw_item->d.b.layer = dim->l.dlayer;
      draw_item->d.b.tag = tag;

      if (memcmp((void *)dim->r.dim_start, (void *)i1->e_d_inters_pt, sizeof(ads_point)) == (int)NULL)
      {
         /* first extension line */
         draw_item->d.b.angle = adjust_angle(dim->r.dim_angle - _180_DEG);
         return(DIM_OK);
      }

      if (memcmp((void *)dim->r.dim_end, (void *)i1->e_d_inters_pt, sizeof(ads_point)) == (int)NULL)
      {
         return(DIM_OK);
      }

      if (l_add(dim->draw_ll, (void *)&draw_item, sizeof(_D_DRAW)) == LIST_ERR)
         return(DIM_ERR);

      draw_item->type = BLOCK;
      memcpy((void *)draw_item->d.b.insert_pt, (void *)i1->e_d_inters_pt, sizeof(ads_point));
      draw_item->d.b.name = dim->dim_marks;
      draw_item->d.b.angle = adjust_angle(dim->r.dim_angle - _180_DEG);
      draw_item->d.b.x_scale = dim->v._dimscale * dim->v._dimasz;
      draw_item->d.b.y_scale = dim->v._dimscale * dim->v._dimasz;
      draw_item->d.b.layer = dim->l.dlayer;
      draw_item->d.b.tag = tag;

   }

   return(DIM_OK);
}

static int add_line_node(_DIM *dim, int type, _D_ITEM *i1, _D_ITEM *i2, int tag)
{
   _D_DRAW     *draw_item;
   ads_real    dist;

   if (l_add(dim->draw_ll, (void *)&draw_item, sizeof(_D_DRAW)) == LIST_ERR)
      return(DIM_ERR);

   draw_item->type = type;
   switch (type)
   {
      case(DIM_LINE):
         memcpy((void *)draw_item->d.l.start, (void *)dim->r.dim_start, sizeof(ads_point));
         memcpy((void *)draw_item->d.l.end, (void *)dim->r.dim_end, sizeof(ads_point));
         draw_item->d.l.layer = dim->l.dlayer;
      break;
      case(EXT_LINE):
         memcpy((void *)draw_item->d.l.start, (void *)i1->e_d_inters_pt, sizeof(ads_point));

         /* extend the extension line according to DIMEXE */
         ads_polar(draw_item->d.l.start, degtorad(dim->r.angle), dim->v._dimexe * dim->v._dimscale, draw_item->d.l.start);

         memcpy((void *)draw_item->d.l.end, (void *)i1->e_base_pt, sizeof(ads_point));
         draw_item->d.l.layer = dim->l.elayer;
         draw_item->d.l.tag = tag;

         draw_item->d.l.hide_line = FALSE;
         if (!dim->draw_end_lines)
         {
            if (!tag || dim->last_line)
               draw_item->d.l.hide_line = TRUE;
         }

         if (!dim->draw_ext_lines)
            draw_item->d.l.hide_line = TRUE;

         /* add mark nodes */
         add_block_node(dim, i1, tag);
      break;
      case(TEXT):


         dist = distance(i1->e_d_inters_pt, i2->e_d_inters_pt);
         if (dim->a.format)
         {
            if (metric_to_impstr(draw_item->d.t.text, dist, dim->a.format) == IMPCV_ERR)
               return(DIM_ERR);
         } else
            sprintf(draw_item->d.t.text, "%.0lf", dist);

         memcpy((void *)draw_item->d.t.insert_pt, (void *)i1->e_d_inters_pt, sizeof(ads_point));

         draw_item->d.t.layer = dim->l.tlayer;
         draw_item->d.t.angle = dim->r.dim_angle;
         draw_item->d.t.height = dim->v._dimtxt * dim->v._dimscale;
         draw_item->d.t.coord_dist = dist;
         draw_item->d.t.repeater = FALSE;
         draw_item->d.t.tag = tag;

         /* change rotation if text is upside down and transform to
            opposite point */
         if (draw_item->d.t.angle > dim->a.rot_lower_limit && 
             draw_item->d.t.angle < dim->a.rot_upper_limit)
         {
            draw_item->d.t.flipped = TRUE;
            draw_item->d.t.angle += _180_DEG;
            memcpy((void *)draw_item->d.t.insert_pt, (void *)i2->e_d_inters_pt, sizeof(ads_point));

            ads_polar(draw_item->d.t.insert_pt, degtorad(dim->r.angle - _180_DEG), dim->v._dimtvp, draw_item->d.t.insert_pt);
            /* now work out the centre point */
            ads_polar(draw_item->d.t.insert_pt, degtorad(dim->r.dim_angle - _180_DEG), dist / 2.0, draw_item->d.t.insert_pt);
         }
         else
         {
            draw_item->d.t.flipped = FALSE;
            ads_polar(draw_item->d.t.insert_pt, degtorad(dim->r.angle), dim->v._dimtvp, draw_item->d.t.insert_pt);
            /* now work out the centre point */
            ads_polar(draw_item->d.t.insert_pt, degtorad(dim->r.dim_angle), dist / 2.0, draw_item->d.t.insert_pt);
         }

         /* flipping not yet implemented */
      break;
   }
   
   return(DIM_OK);
}

static int process_ref(_DIM *dim)
{
   _D_ITEM   *inters_item;

   /* more processing on reference line */
   if ((inters_item = (_D_ITEM *)l_head(dim->item_ll)) == (void *)NULL)
      return(DIM_ERR);

   memcpy((void *)dim->r.dim_start, (void *)inters_item->e_d_inters_pt, sizeof(ads_point));

   if ((inters_item = (_D_ITEM *)l_last(dim->item_ll)) == (void *)NULL)
      return(DIM_ERR);

   memcpy((void *)dim->r.dim_end, (void *)inters_item->e_d_inters_pt, sizeof(ads_point));

   dim->r.dim_angle = angle(dim->r.dim_start, dim->r.dim_end);

   return(DIM_OK);
}

static void add_extension_lines(_DIM *dim)
{
   _D_ITEM     *dim_node;
   int         end_ext_node = 0;
   int         i;

   end_ext_node = l_count(dim->item_ll);

   for (i = 0; i < end_ext_node; i++)
   {
      if (!dim->draw_end_lines)
      {
         if (i == end_ext_node - 1)
            dim->last_line = TRUE;
         else
            dim->last_line = FALSE;
      }

      if ((dim_node = (_D_ITEM *)l_nth(dim->item_ll, i)) != (void *)NULL)
         add_line_node(dim, EXT_LINE, dim_node, (_D_ITEM *)NULL, i);
   }
   dim->last_line = FALSE;
}

int build_dim_lines(_DIM *dim)
{
   _D_ITEM     *dim_node;
   _D_ITEM     *to_node;
   _D_DRAW     *line_node;
   int         i;
   int         num_elems;

   if ((dim->draw_ll = l_gethandle()) == LIST_FULL)
      return(DIM_ERR);

   if (process_ref(dim) == DIM_ERR)
      return(DIM_ERR);

   add_line_node(dim, DIM_LINE, (_D_ITEM *)NULL, (_D_ITEM *)NULL, -1);

   add_extension_lines(dim);

   num_elems = l_count(dim->item_ll);

   for (i = 0; i < num_elems - 1; i++)
   {
      dim_node = l_nth(dim->item_ll, i);
      to_node = l_nth(dim->item_ll, i + 1);
      add_line_node(dim, TEXT, dim_node, to_node, i);
   }

   return(DIM_OK);
}

