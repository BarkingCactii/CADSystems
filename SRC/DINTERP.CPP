/*----------------------------------------------------------------------------
dinterp.c

Interprets a linked list of graphic commands to Autocad.
Don't even try to understand its operation because you never will.

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
#include "drepeat.h"
#include "alloc.h"
#include "alistman.h"
#include "dtxtx.h"

#define  LEFT_PLACE     1
#define  CENTRE_PLACE   2
#define  RIGHT_PLACE    3
#define  SQUEEZE_PLACE  4

#define  REF1           1
#define  REF2           2

static void set_layer(char *layer)
{
   ads_command(RTSTR, "layer", RTSTR, "s", RTSTR, layer, RTSTR, "", RTNONE);
}

static void process_line(_D_line *l)
{
   if (l->hide_line)
      return;

   set_layer(l->layer);
   ads_command(RTSTR, "line", RTPOINT, l->start, RTPOINT, l->end, RTSTR, "", RTNONE);
}

static void process_text(_D_text *n)
{
   if (strlen(n->text) == (int)NULL)
      return;

   set_layer(n->layer);
   ads_command(RTSTR, "text", RTSTR, "c", RTPOINT, n->insert_pt, RTREAL, n->height, RTREAL, n->angle, RTSTR, n->text, RTNONE);
}

static void process_text2(_D_text *t)
{
   if (strlen(t->text) == (int)NULL)
      return;

   ads_entdel(t->tname);
   set_layer(t->layer);
   ads_command(RTSTR, "text", RTSTR, "c", RTPOINT, t->n_insert_pt, RTREAL, t->height, RTREAL, t->angle, RTSTR, t->text, RTNONE);
}

static void process_block(_D_block *b)
{
   set_layer(b->layer);
   ads_command(RTSTR, "insert", RTSTR, b->name, RTPOINT, b->insert_pt, 
      RTREAL, b->x_scale, RTSTR, "", RTREAL, b->angle, RTNONE);
}

static int add_tent_node(_D_DRAW *n, _DIM *dim)
{
   ads_name    tname;
   ads_name    ssname;
   _D_text     *tent;
   int         err;

   tent = &n->d.t;

   if ((err = ads_ssget((char *)"L", NULL, NULL, NULL, ssname)) != RTNORM)
      return(DIM_ERR);

   if ((err = ads_ssname(ssname, 0, tname)) != RTNORM)
      return(DIM_ERR);
               
   memcpy((void *)tent->tname, (void *)tname, sizeof(ads_name));
   get_text_bounds(tname, tent->insert_pt, tent->angle, &tent->width);
   tent->avail_dist = tent->coord_dist - tent->width;
   tent->placement = CENTRE_PLACE;

   ads_ssfree(ssname);

   return(DIM_OK);
}

static int dim_draw_node(_D_DRAW *n, _DIM *dim)
{
   switch (n->type)
   {
      case(DIM_LINE):
      case(EXT_LINE):
         process_line(&n->d.l);
      break;
      case(TEXT):
         process_text(&n->d.t);
         add_tent_node(n, dim);
      break;
      case(BLOCK):
         process_block(&n->d.b);
      break;
   }

   return(DIM_OK);
}

 
static struct resbuf *style;

static void save_environment()
{
   if (alloc_mem((void *)&style, sizeof(struct resbuf)) == MEM_ERR)
      return;

   ads_getvar("textstyle", style);

}

static void restore_environment()
{
   ads_setvar("textstyle", style);
   ads_relrb(style);
}


static int change_block_attr(_DIM *dim, int tag, char *layer, int ref)
{
   _D_DRAW  *d;
   _D_block *b;
   int   curr_ref = REF1;

   if ((d = (_D_DRAW *)l_head(dim->draw_ll)) == (void *)NULL)
      return(DIM_ERR);

   while (d)
   {
      if (d->type == BLOCK)
      {
         b = &d->d.b;
         if (b->tag == tag)
         {
            if (ref == curr_ref)
            {
               b->angle += _180_DEG;
               b->layer = layer;
            }
            curr_ref++;
         }
      }
      d = (_D_DRAW *)l_next(dim->draw_ll);
   }

   return(DIM_OK);
}

static int recalculate_block(_DIM *dim, int placement, int tag)
{
   _D_DRAW  *d;
   _D_block *b;
   int      delete_flag = FALSE;
   _D_block save_b;

   if (dim->mark_type == TICK_T)
      return(DIM_OK);

   if (placement == CENTRE_PLACE || placement == SQUEEZE_PLACE)
      return(DIM_OK);

   if ((d = (_D_DRAW *)l_head(dim->draw_ll)) == (void *)NULL)
      return(DIM_ERR);

   while (d)
   {
      if (d->type == BLOCK)
      {
         b = &d->d.b;
         if (b->tag == tag)
         {
            if (placement == LEFT_PLACE)
            {
               if (!tag)
                  change_block_attr(dim, tag, b->layer, REF1);
               else
                  change_block_attr(dim, tag, b->layer, REF2);
               break;
            }
            if (placement == RIGHT_PLACE)
            {
               if (!tag)
                  change_block_attr(dim, tag, b->layer, REF1);
               change_block_attr(dim, tag + 1, b->layer, REF1);
               break;
            }
         }
      }
      l_make_current(dim->draw_ll, (void *)d);
      d = (_D_DRAW *)l_next(dim->draw_ll);
   }

   return(DIM_OK);
}

static int calc_text_pos2(_DIM *dim, int start, int curr, int total)
{
   _D_DRAW  *draw;
   _D_text  *t;
   _D_text  *prev = (_D_text *)NULL;
   _D_text  *pprev = (_D_text *)NULL;
   _D_text  *next = (_D_text *)NULL;
   ads_real new_offset;

   if ((draw = (_D_DRAW *)l_nth(dim->draw_ll, curr)) == (void *)NULL)
      return(DIM_ERR);

   t = &draw->d.t;
   if (curr > start)
   {
      draw = (_D_DRAW *)l_nth(dim->draw_ll, curr - 1);
      prev = &draw->d.t;
   }

   memcpy((void *)t->n_insert_pt, (void *)t->insert_pt, sizeof(ads_point));

   if (t->repeater)
      return(DIM_OK);

   if (t->avail_dist >= 0.0)
      return(DIM_OK);

   /* available distance is negative, therefore the text is larger than
      the dimension it refers to */

   new_offset = t->width + (t->avail_dist / 2);
/*   recalculate_block(dim, t, (curr == total - 1) ? TRUE : FALSE); */

   /* last dimension text, so always right place */
   if (curr == total - 1)
   {
      t->placement = RIGHT_PLACE;
      t->avail_dist = t->coord_dist;
      if (t->flipped)
         ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle + _180_DEG)), new_offset, t->n_insert_pt);
      else
         ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle)), new_offset, t->n_insert_pt);

      return(DIM_OK);
   }

   /* first dimension text, so always left place */
   if (curr == start)
   {
      t->placement = LEFT_PLACE;
      t->avail_dist = t->coord_dist;
      if (t->flipped)
         ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle)), new_offset, t->n_insert_pt);
      else
         ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle + _180_DEG)), new_offset, t->n_insert_pt);

      return(DIM_OK);
   }


   if (curr < total - 1)
   {
      draw = (_D_DRAW *)l_nth(dim->draw_ll, curr + 1);
      next = &draw->d.t;
   }

   if (prev->placement == SQUEEZE_PLACE || prev->placement == LEFT_PLACE)
   {
      /* try to locate to right */
      if (next->avail_dist / 2.0 >= t->width)
      {
         t->placement = RIGHT_PLACE;
         t->avail_dist = t->coord_dist;
         next->avail_dist -= t->width;
         if (t->flipped)
            ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle + _180_DEG)), new_offset, t->n_insert_pt);
         else
            ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle)), new_offset, t->n_insert_pt);
         return(DIM_OK);
      }
      /* won't fit anywhere, so squeeze */
      t->placement = SQUEEZE_PLACE;
      return(DIM_OK);
   }

   if (prev->placement == CENTRE_PLACE)
   {
      /* try to locate to left */
      if (prev->avail_dist / 2.0 >= t->width)
      {
         t->placement = LEFT_PLACE;
         t->avail_dist = t->coord_dist;
         prev->avail_dist -= t->width;
         if (t->flipped)
            ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle)), new_offset, t->n_insert_pt);
         else
            ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle + _180_DEG)), new_offset, t->n_insert_pt);
         return(DIM_OK);
      }
      /* try to locate to right */
      if (next->avail_dist / 2.0 >= t->width)
      {
         t->placement = RIGHT_PLACE;
         t->avail_dist = t->coord_dist;
         next->avail_dist -= t->width;
         if (t->flipped)
            ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle + _180_DEG)), new_offset, t->n_insert_pt);
         else
            ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle)), new_offset, t->n_insert_pt);
         return(DIM_OK);
      }
      /* won't fit anywhere, so squeeze */
      t->placement = SQUEEZE_PLACE;
      return(DIM_OK);
   }

   if (prev->placement == RIGHT_PLACE)
   {
      prev->placement = SQUEEZE_PLACE;
      prev->avail_dist = prev->coord_dist - prev->width;
      memcpy((void *)prev->n_insert_pt, (void *)prev->insert_pt, sizeof(ads_point));

      /* try to locate to right */
      if (next->avail_dist / 2.0 >= t->width)
      {
         t->placement = RIGHT_PLACE;
         t->avail_dist = t->coord_dist;
         next->avail_dist -= t->width;
         if (t->flipped)
            ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle + _180_DEG)), new_offset, t->n_insert_pt);
         else
            ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle)), new_offset, t->n_insert_pt);
         return(DIM_OK);
      }
      /* won't fit anywhere, so squeeze */
      t->placement = SQUEEZE_PLACE;
      return(DIM_OK);

   }

   ads_printf("!! DIMENSIONING TEXT PLACEMENT ERROR !!\n");
   return(DIM_OK);
}

static int unsqueeze_text(_DIM *dim, int start)
{
   _D_DRAW     *draw_item;
   int         i;
   _D_text     *t = (_D_text *)NULL;
   ads_point   start_pt;
   ads_point   end_pt;
   int         num_squeezes;
   int         start_squeeze;
   int         idx;
   ads_real    offset;
   ads_real    unit_distance;
   ads_real    tot_distance;

   i = start;
   if ((draw_item = (_D_DRAW *)l_nth(dim->draw_ll, i)) != (void *)NULL)
   {
      t = &draw_item->d.t;

      /* calculate start point */
      if (t->flipped)
         ads_polar(t->n_insert_pt, degtorad(adjust_angle(t->angle - _180_DEG)), t->width / 2.0, start_pt);
      else
         ads_polar(t->n_insert_pt, degtorad(adjust_angle(t->angle)), t->width / 2.0, start_pt);
   }

   start_squeeze = start + 1;
   num_squeezes = 0;

   i++;
   if ((draw_item = (_D_DRAW *)l_nth(dim->draw_ll, i)) == (void *)NULL)
      return(i);
   if (draw_item->type != TEXT)
      return(i);

   t = &draw_item->d.t;

   while (t->placement == SQUEEZE_PLACE)
   {
      num_squeezes++;
      if ((draw_item = (_D_DRAW *)l_next(dim->draw_ll)) == (void *)NULL)
         return(i);
      if (draw_item->type != TEXT)
         return(i);

      t = &draw_item->d.t;
   }

   /* calculate end point */
   if (t->flipped)
      ads_polar(t->n_insert_pt, degtorad(adjust_angle(t->angle)), t->width / 2.0, end_pt);
   else
      ads_polar(t->n_insert_pt, degtorad(adjust_angle(t->angle) - _180_DEG), t->width / 2.0, end_pt);

   tot_distance = distance(start_pt, end_pt);
   unit_distance = tot_distance / num_squeezes;

   for (idx = 0, i = start_squeeze; i < start_squeeze + num_squeezes; i++, idx++)
   {
      if ((draw_item = (_D_DRAW *)l_nth(dim->draw_ll, i)) == (void *)NULL)
         return(i);
      if (draw_item->type != TEXT)
         return(i);
      t = &draw_item->d.t;

      if (t->placement != SQUEEZE_PLACE)
         return(i);

      /* now adjust the point */
      offset = unit_distance * idx;
      /* up to mid point */
      offset += unit_distance / 2.0;
   
      if (t->flipped)
         ads_polar(start_pt, degtorad(adjust_angle(t->angle - _180_DEG)), offset, t->n_insert_pt);
      else
         ads_polar(start_pt, degtorad(adjust_angle(t->angle)), offset, t->n_insert_pt);
   }
   return(i);
}

static int recalculate_squeezed_text(_DIM *dim, int start, int end)
{
   _D_DRAW  *draw_item;
   _D_text  *t = (_D_text *)NULL;
   _D_text  *next = (_D_text *)NULL;
   int      i;

   for (i = start; i < end; i++)
   {
      if ((draw_item = (_D_DRAW *)l_nth(dim->draw_ll, i)) != (void *)NULL)
      {
         if (draw_item->type != TEXT)
            return(DIM_ERR);

         t = &draw_item->d.t;

         if ((draw_item = (_D_DRAW *)l_nth(dim->draw_ll, i + 1)) == (void *)NULL)
            return(DIM_ERR);
         if (draw_item->type != TEXT)
            return(DIM_ERR);

         next = &draw_item->d.t;

         if (t->placement == LEFT_PLACE && next->placement == SQUEEZE_PLACE)
            i = unsqueeze_text(dim, i);
      }
   }
}

static void recalculate_text(_DIM *dim)
{
   _D_DRAW     *draw_item;
   int         i;
   int         total;
   int         start_txt = 0;
   int         end_txt = 0;

   total = l_count(dim->draw_ll);
   for (i = 0, start_txt = 0, end_txt = total; i < total; i++)
   {
      if ((draw_item = (_D_DRAW *)l_nth(dim->draw_ll, i)) != (void *)NULL)
      {
         if (draw_item->type == TEXT)
         {
            if (!start_txt)
               /* mark start */
               start_txt = i;
         }
         else
         {
            if (start_txt && !end_txt)
            {
               /* mark end and exit */
               end_txt = i;
               break;
            }
         }
      }
   }

   for (i = start_txt; i < end_txt; i++)
      calc_text_pos2(dim, start_txt, i, end_txt);

   recalculate_squeezed_text(dim, start_txt, end_txt); 
}


int dim_interpret(_DIM *dim)
{
   _D_DRAW     *draw_item;
   int         i;
   int         total;

   /* calculate and break crossed lines */
   if (dim->break_lines)
   {
      for (i = 0; i < l_count(dim->draw_ll); i++)
         if ((draw_item = (_D_DRAW *)l_nth(dim->draw_ll, i)) != (void *)NULL)
         {
            if ((dim->cross_ll = l_gethandle()) == LIST_FULL)
               break;
            dim_x(draw_item, dim);
            l_free(dim->cross_ll);
         }
   }

   /* calculate repeated dimensions n @ ddd */
   calculate_repeat_dims(dim);

   /* place entities on screen except for blocks */
   if ((draw_item = (_D_DRAW *)l_head(dim->draw_ll)) == (void *)NULL)
      return(DIM_ERR);
   while (draw_item)
   {
      if (draw_item->type != BLOCK)
         dim_draw_node(draw_item, dim);
      draw_item = (_D_DRAW *)l_next(dim->draw_ll);
   }

   /* re-calculate text positioning */
   recalculate_text(dim);

   /* redraw the text in its final position */
   if ((draw_item = (_D_DRAW *)l_head(dim->draw_ll)) == (void *)NULL)
      return(DIM_ERR);
   while (draw_item)
   {
      if (draw_item->type == TEXT)
      {
         /* only redraw text if insert point has changed */
         if (memcmp((void *)draw_item->d.t.insert_pt, (void *)draw_item->d.t.n_insert_pt, sizeof(ads_point))) 
            process_text2(&draw_item->d.t);
      }
      draw_item = (_D_DRAW *)l_next(dim->draw_ll);
   }

   /* re-calculate blocks */
   total = l_count(dim->draw_ll);
   for (i = 0; i < total; i++)
   {
      if ((draw_item = (_D_DRAW *)l_nth(dim->draw_ll, i)) == (void *)NULL)
         break;
      if (draw_item->type == TEXT)
         recalculate_block(dim, draw_item->d.t.placement, draw_item->d.t.tag);
   }

   /* place blocks on screen */
   if ((draw_item = (_D_DRAW *)l_head(dim->draw_ll)) == (void *)NULL)
      return(DIM_ERR);
   while (draw_item)
   {
      if (draw_item->type == BLOCK)
         dim_draw_node(draw_item, dim);
      draw_item = (_D_DRAW *)l_next(dim->draw_ll);
   }

   /* and finally, check all dimension entities for intersecting lines,
      and break them if they do */
   calculate_text_x(dim);

   return(DIM_OK);
}
