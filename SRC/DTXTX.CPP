/*----------------------------------------------------------------------------
dtxtx.c

Removes any extension lines that cross dimension text

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
#include "dmkline.h"
#include "dcross.h"
#include "alistman.h"

#define UCS_ORIGIN      10
#define TEXT_ANGLE      50

#define TEXT_ENTITY     "TEXT"

static ads_point        p1;
static ads_point        p2;
static ads_point        p3;
static ads_point        p4;

#define  TEXT_POSITION  10
#define  TEXT_DEGREES   50

static int line_breaks_text(_DIM *dim, ads_point line_from, ads_point line_to, ads_point break_from, ads_point break_to)
{
   ads_point   inters_pt;
   int         break_from_filled = FALSE;
   int         break_to_filled = FALSE;
/*   ads_polar(inters_pt, degtorad(adjust_angle(dim->r.angle)), break_distance, list->d.l.end);
   ads_polar(inters_pt, degtorad(adjust_angle(dim->r.angle - _180_DEG)), break_distance, node->d.l.start);
*/

   if (!parallel(line_from, line_to, p1, p2))
   {
      if (ads_inters(line_from, line_to, p1, p2, TRUE, inters_pt) == RTNORM)
      {
         if (break_from_filled)
         {
            memcpy((void *)break_to, (void *)inters_pt, sizeof(ads_point));
            return(DIM_OK);
         }
         else
         {
            memcpy((void *)break_from, (void *)inters_pt, sizeof(ads_point));
            break_from_filled = TRUE;
         }
      }
   }

   if (!parallel(line_from, line_to, p2, p3))
   {
      if (ads_inters(line_from, line_to, p2, p3, TRUE, inters_pt) == RTNORM)
      {
         if (break_from_filled)
         {
            memcpy((void *)break_to, (void *)inters_pt, sizeof(ads_point));
            return(DIM_OK);
         }
         else
         {
            memcpy((void *)break_from, (void *)inters_pt, sizeof(ads_point));
            break_from_filled = TRUE;
         }
      }
   }

   if (!parallel(line_from, line_to, p3, p4))
   {
      if (ads_inters(line_from, line_to, p3, p4, TRUE, inters_pt) == RTNORM)
      {
         if (break_from_filled)
         {
            memcpy((void *)break_to, (void *)inters_pt, sizeof(ads_point));
            return(DIM_OK);
         }
         else
         {
            memcpy((void *)break_from, (void *)inters_pt, sizeof(ads_point));
            break_from_filled = TRUE;
         }
      }
   }

   if (!parallel(line_from, line_to, p4, p1))
   {
      if (ads_inters(line_from, line_to, p4, p1, TRUE, inters_pt) == RTNORM)
      {
         if (break_from_filled)
         {
            memcpy((void *)break_to, (void *)inters_pt, sizeof(ads_point));
            return(DIM_OK);
         }
         else
         {
            memcpy((void *)break_from, (void *)inters_pt, sizeof(ads_point));
            break_from_filled = TRUE;
         }
      }
   }

   if (break_from_filled || break_to_filled)
      return(DIM_OK);

   return(DIM_ERR);
}

static struct resbuf *make_crossing_text_poly(_DIM *dim)
{
   struct      resbuf *start;
   struct      resbuf *rb;
   ads_real    break_distance;

   break_distance = (dim->v._dimexo * dim->v._dimscale) / 2.0;

   /* massage the points */
   if ((start = rb = ads_newrb(RTPOINT)) == (struct resbuf *)NULL)
      return((struct resbuf *)NULL);
   ads_polar(p1, degtorad(angle(p3,p1)), break_distance, p1);
   memcpy((void *)rb->resval.rpoint, (void *)p1, sizeof(ads_point));

   if ((rb->rbnext = ads_newrb(RTPOINT)) == (struct resbuf *)NULL)
      return((struct resbuf *)NULL);
   rb = rb->rbnext;
   ads_polar(p2, degtorad(angle(p4,p2)), break_distance, p2);
   memcpy((void *)rb->resval.rpoint, (void *)p2, sizeof(ads_point));

   if ((rb->rbnext = ads_newrb(RTPOINT)) == (struct resbuf *)NULL)
      return((struct resbuf *)NULL);
   rb = rb->rbnext;
   ads_polar(p3, degtorad(angle(p1,p3)), break_distance, p3);
   memcpy((void *)rb->resval.rpoint, (void *)p3, sizeof(ads_point));

   if ((rb->rbnext = ads_newrb(RTPOINT)) == (struct resbuf *)NULL)
      return((struct resbuf *)NULL);
   rb = rb->rbnext;
   ads_polar(p4, degtorad(angle(p2,p4)), break_distance, p4);
   memcpy((void *)rb->resval.rpoint, (void *)p4, sizeof(ads_point));

   return(start);
}

static int text_entity(struct resbuf *linerb)
{
   struct resbuf *found;
   
   if ((found = a_find_restype(linerb, ENTITY_START)) == (struct resbuf *)NULL)
      return(DIM_ERR);

   if (strcmp(found->resval.rstring, TEXT_ENTITY))
      return(DIM_ERR);
   
   return(DIM_OK);
}

static struct resbuf *make_crossing_poly(_DIM *dim)
{
   _D_ITEM           *item;
   struct            resbuf *start;
   struct            resbuf *rb;
   ads_point         corner;

   if ((item = (_D_ITEM *)l_head(dim->item_ll)) == (_D_ITEM *)NULL)
      return((struct resbuf *)NULL);

   if ((start = rb = ads_newrb(RTPOINT)) == (struct resbuf *)NULL)
      return((struct resbuf *)NULL);
   memcpy((void *)rb->resval.rpoint, (void *)item->e_base_pt, sizeof(ads_point));

   if ((rb->rbnext = ads_newrb(RTPOINT)) == (struct resbuf *)NULL)
      return((struct resbuf *)NULL);
   rb = rb->rbnext;
   ads_polar(item->e_d_inters_pt, degtorad(dim->r.angle), dim->v._dimtvp * 1.1, corner);
   memcpy((void *)rb->resval.rpoint, (void *)corner, sizeof(ads_point));

   if ((item = (_D_ITEM *)l_last(dim->item_ll)) == (_D_ITEM *)NULL)
      return((struct resbuf *)NULL);

   if ((rb->rbnext = ads_newrb(RTPOINT)) == (struct resbuf *)NULL)
      return((struct resbuf *)NULL);
   rb = rb->rbnext;
   ads_polar(item->e_d_inters_pt, degtorad(dim->r.angle), dim->v._dimtvp * 1.1, corner);
   memcpy((void *)rb->resval.rpoint, (void *)corner, sizeof(ads_point));

   if ((rb->rbnext = ads_newrb(RTPOINT)) == (struct resbuf *)NULL)
      return((struct resbuf *)NULL);
   rb = rb->rbnext;
   memcpy((void *)rb->resval.rpoint, (void *)item->e_base_pt, sizeof(ads_point));

   return(start);
}

static int text_on_dim_layer(struct resbuf *linerb, _DIM *dim)
{
   if (line_on_layer(linerb, dim->l.tlayer) == DIM_OK)
      return(DIM_OK);

   return(DIM_ERR);
}

static int calculate_line_x_text(_DIM *dim, struct resbuf *polylist)
{
   ads_name    entname;
   ads_name    ssname;
   long        sslen;
   long        i;
   int         err;
   struct      resbuf *entrb;
   struct      resbuf *p1;
   struct      resbuf *p2;
   ads_point   break_from;
   ads_point   break_to;

   if ((err = ads_ssget((char *)"CP", polylist, NULL, NULL, ssname)) != RTNORM)
      return(DIM_ERR);

   if (ads_sslength(ssname, &sslen) != RTNORM)
   {
      ads_ssfree(ssname);
      return(DIM_ERR);
   }

   for (i = 0L; i < sslen; i++)
   {
      if (ads_ssname(ssname, i, entname) != RTNORM)
         continue;

      if ((entrb = ads_entget(entname)) == (struct resbuf *)NULL)
         continue;

      if (line_entity(entrb) == DIM_OK)
      {
         /* chech if line on dimension layers */
         if (line_on_ext_layer(entrb, dim) == DIM_OK)
         {
            if ((p1 = a_find_restype(entrb, LINE_START)) == (struct resbuf *)NULL)
            {
               ads_ssfree(ssname);
               ads_relrb(entrb);
               return(DIM_ERR);
            }

            if ((p2 = a_find_restype(entrb, LINE_END)) == (struct resbuf *)NULL)
            {
               ads_ssfree(ssname);
               ads_relrb(entrb);
               return(DIM_ERR);
            }

            break_from[X] = break_from[Y] = 0.0;
            break_to[X] = break_to[Y] = 0.0;
            if (line_breaks_text(dim, p1->resval.rpoint, p2->resval.rpoint, break_from, break_to))
            {
               if (break_from[X] && break_from[Y] && break_to[X] && break_to[Y])
                  ads_command(RTSTR, "break", RTENAME, entname, RTPOINT, break_from, RTPOINT, break_to, RTNONE);
            }
            else
            {
               /* delete the line, as it is a segment lying within the poly */
               ads_entdel(entname);
            }
         }
      }

      ads_relrb(entrb);
   }

   ads_ssfree(ssname);

   return(DIM_OK);
}

int calculate_text_x(_DIM *dim)
{
   struct resbuf *polylist;
   struct resbuf *polytextlist;
   ads_name    entname;
   ads_name    ssname;
   long        sslen;
   long        i;
   int         err;
   struct      resbuf *entrb;
   struct      resbuf *p;
   struct      resbuf *q;
   ads_real    rotate;
   ads_point   origin;
   ads_point   lowleft;
   ads_point   upright;

   if (!dim->break_lines)
      return(DIM_OK);

   if ((polylist = make_crossing_poly(dim)) == (struct resbuf *)NULL)
      return(DIM_ERR);

   if ((err = ads_ssget((char *)"CP", polylist, NULL, NULL, ssname)) != RTNORM)
      return(DIM_ERR);

   ads_relrb(polylist);

   if (ads_sslength(ssname, &sslen) != RTNORM)
   {
      ads_ssfree(ssname);
      return(DIM_ERR);
   }

   for (i = 0L; i < sslen; i++)
   {
      if (ads_ssname(ssname, i, entname) != RTNORM)
         continue;

      if ((entrb = ads_entget(entname)) == (struct resbuf *)NULL)
         continue;

      if (text_entity(entrb) == DIM_OK)
      {
         /* chech if line on dimension layers */
         if (text_on_dim_layer(entrb, dim) == DIM_OK)
         {
            if ((p = a_find_restype(entrb, UCS_ORIGIN)) == (struct resbuf *)NULL)
            {
               ads_ssfree(ssname);
               ads_relrb(entrb);
               return(DIM_ERR);
            }

            origin[X] = p->resval.rpoint[X];
            origin[Y] = p->resval.rpoint[Y];

            if ((q = a_find_restype(entrb, TEXT_ANGLE)) == (struct resbuf *)NULL)
            {
               ads_ssfree(ssname);
               ads_relrb(entrb);
               return(DIM_ERR);
            }

            rotate = q->resval.rreal;

            if (ads_textbox(entrb, lowleft, upright) != RTNORM)
            {
               ads_ssfree(ssname);
               ads_relrb(entrb);
               return(DIM_ERR);
            }

            p1[X] = origin[X] + ((lowleft[X] * cos(rotate)) - (lowleft[Y] * sin(rotate)));
            p1[Y] = origin[Y] + ((lowleft[X] * sin(rotate)) + (lowleft[Y] * cos(rotate)));

            p2[X] = origin[X] + ((upright[X] * cos(rotate)) - (lowleft[Y] * sin(rotate)));
            p2[Y] = origin[Y] + ((upright[X] * sin(rotate)) + (lowleft[Y] * cos(rotate)));

            p3[X] = origin[X] + ((upright[X] * cos(rotate)) - (upright[Y] * sin(rotate)));
            p3[Y] = origin[Y] + ((upright[X] * sin(rotate)) + (upright[Y] * cos(rotate)));

            p4[X] = origin[X] + ((lowleft[X] * cos(rotate)) - (upright[Y] * sin(rotate)));
            p4[Y] = origin[Y] + ((lowleft[X] * sin(rotate)) + (upright[Y] * cos(rotate)));

            if ((polytextlist = make_crossing_text_poly(dim)) == (struct resbuf *)NULL)
            {
               ads_ssfree(ssname);
               ads_relrb(entrb);
               return(DIM_ERR);
            }

            calculate_line_x_text(dim, polytextlist);
            ads_relrb(polytextlist);

         }
      }
      ads_relrb(entrb);
   }

   ads_ssfree(ssname);

   return(DIM_OK);
}


