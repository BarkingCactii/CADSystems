/*----------------------------------------------------------------------------
dcross.c

Determines if a dimension line to be drawn crosses an existing dimension line
by Jeff Hill
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "adslib.h"
#include "ads.h"

#include "alistman.h"
#include "llistman.h"
#include "geom.h"
#include "dcross.h"
#include "dmain.h"
#include "dmkline.h"

#define DIM_LIST     1
#define SS_FENCE     "F"


static struct resbuf *make_flist(ads_point p1, ads_point p2)
{
   struct resbuf *rb;

	if ((rb = ads_newrb(RTPOINT)) == (struct resbuf *)NULL)
      return(rb);

   memcpy((void *)rb->resval.rpoint, (void *)p1, sizeof(ads_point));
   rb->rbnext = ads_newrb(RTPOINT);
   memcpy((void *)rb->rbnext->resval.rpoint, (void *)p2, sizeof(ads_point));

   return(rb);
}
  
int line_entity(struct resbuf *linerb)
{
   struct resbuf *found;
   
   if ((found = a_find_restype(linerb, ENTITY_START)) == (struct resbuf *)NULL)
      return(DIM_ERR);

   if (strcmp(found->resval.rstring, LINE_ENTITY))
      return(DIM_ERR);
   
   return(DIM_OK);
}

int line_on_layer(struct resbuf *linerb, char *layer_name)
{
   struct resbuf *found;
   
   if ((found = a_find_restype(linerb, ENTITY_LAYER)) == (struct resbuf *)NULL)
      return(DIM_ERR);

   if (strcmp(found->resval.rstring, layer_name))
      return(DIM_ERR);

   return(DIM_OK);
}

static int line_on_dim_layer(struct resbuf *linerb, _DIM *dim)
{
   if (line_on_layer(linerb, dim->l.dlayer) == DIM_OK)
      return(DIM_OK);

   return(DIM_ERR);
}

int line_on_ext_layer(struct resbuf *linerb, _DIM *dim)
{
   if (line_on_layer(linerb, dim->l.elayer) == DIM_OK)
      return(DIM_OK);

   return(DIM_ERR);
}

static int cross_draw_node(ads_point inters_pt, _D_DRAW *list, _DIM *dim)
{
   _D_DRAW     *node;
   ads_real    break_distance;

   if (l_add(dim->draw_ll, (void *)&node, sizeof(_D_DRAW)) == LIST_ERR)
      return(DIM_ERR);

   memcpy((void *)node, (void *)list, sizeof(_D_DRAW));

   break_distance = (dim->v._dimexo * dim->v._dimscale) / 2.0;
   ads_polar(inters_pt, degtorad(adjust_angle(dim->r.angle)), break_distance, list->d.l.end);
   ads_polar(inters_pt, degtorad(adjust_angle(dim->r.angle - _180_DEG)), break_distance, node->d.l.start);

/*   ads_printf("angle [%lf] opp angle [%lf] start [%lf][%lf]\n", adjust_angle(dim->r.angle), adjust_angle(dim->r.angle - _180_DEG), 
      node->d.l.start[X], node->d.l.start[Y]);
*/
   return(DIM_OK);
}

static int dim_entity(ads_name entname, _D_DRAW *list, _DIM *dim)
{
   struct resbuf *entrb;
   int    status = DIM_ERR;
   struct resbuf *p1;
   struct resbuf *p2;
   ads_point   inters_pt;

   /* get resbuf list for entity */
   if ((entrb = ads_entget(entname)) == (struct resbuf *)NULL)
      return(status);

   /* check if entity on line */
   if (line_entity(entrb) == DIM_OK)
   {
      /* chech if line on dimension layers */
      if (line_on_ext_layer(entrb, dim) == DIM_OK)
      {
         status = DIM_OK;
      }
      if (line_on_dim_layer(entrb, dim) == DIM_OK && strcmp(dim->l.dlayer, list->d.l.layer))
      {
         if ((p1 = a_find_restype(entrb, LINE_START)) == (struct resbuf *)NULL)
            return(DIM_ERR);

         if ((p2 = a_find_restype(entrb, LINE_END)) == (struct resbuf *)NULL)
            return(DIM_ERR);

         if (distance(list->d.l.start, list->d.l.end) <= dim->v._dimexo * dim->v._dimscale)
            return(DIM_ERR);

         if (distance(p1->resval.rpoint, p2->resval.rpoint) <= dim->v._dimexo * dim->v._dimscale)
            return(DIM_ERR);

         if (ads_inters(list->d.l.start, list->d.l.end, p1->resval.rpoint, p2->resval.rpoint, TRUE, inters_pt) != RTNORM)
            return(DIM_ERR);

         /* in this situation, we break up the new line rather then entrb */
         cross_draw_node(inters_pt, list, dim);
      }
   }

   ads_relrb(entrb);
   return(status);
}

static int add_dim_node(_L_HANDLE lhand, ads_name entname, _D_DRAW *params)
{
   struct resbuf *entrb;
   struct resbuf *p1;
   struct resbuf *p2;
   _DIM_NODE      *node;
   ads_point      inters_pt;

   /* get resbuf list for entity */
   if ((entrb = ads_entget(entname)) == (struct resbuf *)NULL)
      return(DIM_ERR);

   if ((p1 = a_find_restype(entrb, LINE_START)) == (struct resbuf *)NULL)
      return(DIM_ERR);

   if ((p2 = a_find_restype(entrb, LINE_END)) == (struct resbuf *)NULL)
      return(DIM_ERR);

   /* ignore same line or lines with same endpoints */
   if ((memcmp(p1, params->d.l.start, sizeof(ads_point)) == 0 ||
        memcmp(p2, params->d.l.start, sizeof(ads_point)) == 0) &&
       (memcmp(p1, params->d.l.end, sizeof(ads_point)) == 0 ||
        memcmp(p2, params->d.l.end, sizeof(ads_point)) == 0))
   {
      ads_printf("similar line\n");
      return(DIM_ERR);
   }
   if (l_add(lhand, (void *)&node, sizeof(_DIM_NODE)) == LIST_ERR)
      return(DIM_ERR);

   memcpy((void *)node->line_entity, (void *)entname, sizeof(ads_name));
   memcpy((void *)node->p1, (void *)p1->resval.rpoint, sizeof(ads_point));
   memcpy((void *)node->p2, (void *)p2->resval.rpoint, sizeof(ads_point));

   node->angle = angle(node->p1, node->p2);

   if (parallel(params->d.l.start, params->d.l.end, node->p1, node->p2))
   {
      node->crossing_type = DIM_PARALLEL;
   }
   else
   {
      node->crossing_type = DIM_CROSSING;
      if (ads_inters(params->d.l.start, params->d.l.end, node->p1, node->p2, TRUE, node->inters_pt) != RTNORM)
         return(DIM_ERR);
   }

/*   ads_inters(params->d.l.start, params->d.l.end, node->p1, node->p2, TRUE, node->inters_pt);*/
/*   ads_command(RTSTR, "circle", RTPOINT, inters_pt, RTREAL, 0.1, RTNONE, RTNONE); */

   ads_relrb(entrb);
   return(DIM_OK);
}

int dim_x(_D_DRAW *list, _DIM *dim)
{
   struct resbuf *fencelist;
   ads_name       ssname;
   ads_name       entname;
   long           sslen;
   int            i;
   _DIM_NODE      *node;

   ads_point      c1;
   ads_point      c2;

   /* if extension line offset is zero, don't bother breaking lines */
   if (dim->v._dimexo == 0.0)
      return(DIM_OK);

   if (list->type != DIM_LINE && list->type != EXT_LINE) 
      return(DIM_OK);

   if ((fencelist = make_flist(list->d.l.start, list->d.l.end)) == (struct resbuf *)NULL)
      return(DIM_ERR);

   if (ads_ssget(SS_FENCE, (void *)fencelist, NULL, NULL, ssname) != RTNORM)
   {
      /* no entities cross the dimension */
      ads_relrb(fencelist);
      return(DIM_OK);
   }

   ads_relrb(fencelist);

   if (ads_sslength(ssname, &sslen) != RTNORM)
      return(DIM_ERR);

   for (i = 0L; i < sslen; i++)
   {
      if (ads_ssname(ssname, i, entname) != RTNORM)
         continue;

      if (dim_entity(entname, list, dim) == DIM_ERR)
         continue;

      /* if we get here, then we know a dimension will cross */
      add_dim_node(dim->cross_ll, entname, list);
   }

   ads_ssfree(ssname);

   /* issue the break commands */
   if ((node = (_DIM_NODE *)l_head(dim->cross_ll)) == (void *)NULL)
      return(DIM_ERR);

   while (node)
   {
      ads_polar(node->inters_pt, adjust_angle(degtorad(node->angle)), dim->v._dimexo * dim->v._dimscale, c1);
      ads_polar(node->inters_pt, adjust_angle(degtorad(node->angle - _180_DEG)), dim->v._dimexo * dim->v._dimscale, c2);

      ads_command(RTSTR, "break", RTENAME, node->line_entity, RTPOINT, c1, RTPOINT, c2, RTNONE);

      node = (_DIM_NODE *)l_next(dim->cross_ll);
   }

   return(DIM_OK);
}
