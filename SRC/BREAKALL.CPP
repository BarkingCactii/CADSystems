/*----------------------------------------------------------------------------
breakall.cpp

Breaks all intersecting lines for a given selection set
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

typedef struct
{
   ads_point   start;
   ads_point   end;
   ads_name    entity;

} LINE_END_POINTS;

typedef struct
{
   ads_name    entity;
   ads_point   inters;

} BREAK_POINT;

typedef struct 
{
   ads_point   inters_pt;
   ads_real    dist;
} I_POINT;


#define DIM_LIST           1
#define MAX_INTERS_PTS     100
#define ENTITY_NAME        -1
#define ENTITY_START       0
#define ENTITY_LAYER       8
#define LINE_START         10
#define LINE_END           11

#define LINE_ENTITY  "LINE"

static I_POINT cross_list[MAX_INTERS_PTS];

static void swap_pts(ads_point p1, ads_point p2)
{
   ads_point   tmp;

   memcpy((void *)tmp, (void *)p1, sizeof(ads_point));
   memcpy((void *)p1, (void *)p2, sizeof(ads_point));
   memcpy((void *)p2, (void *)tmp, sizeof(ads_point));
}

static int line_entity(struct resbuf *linerb)
{
   struct resbuf *found;
   
   if ((found = a_find_restype(linerb, ENTITY_START)) == (struct resbuf *)NULL)
      return(FALSE);

   if (strcmp(found->resval.rstring, LINE_ENTITY))
      return(FALSE);
   
   return(TRUE);
}

static int process_line(ads_name entname, _L_HANDLE ll, _L_HANDLE rbl)
{
   struct resbuf     *entrb;
   void              *new_entrb;
   struct resbuf     *p1;
   struct resbuf     *p2;
   int               failed = FALSE;
   LINE_END_POINTS   *l_node;

   /* get resbuf list for entity */
   if ((entrb = ads_entget(entname)) == (struct resbuf *)NULL)
      return(FALSE);

   /* check if entity a line */
   if (line_entity(entrb) == TRUE)
   {
      if ((p1 = a_find_restype(entrb, LINE_START)) == (struct resbuf *)NULL)
         failed = TRUE;

      if ((p2 = a_find_restype(entrb, LINE_END)) == (struct resbuf *)NULL)
         failed = TRUE;

      if (l_add(ll, (void *)&l_node, sizeof(LINE_END_POINTS)) == LIST_ERR)
         failed = TRUE;

      if (!failed)
      {
         memcpy((void *)l_node->start, (void *)p1->resval.rpoint, sizeof(ads_point));
         memcpy((void *)l_node->end, (void *)p2->resval.rpoint, sizeof(ads_point));
         memcpy((void *)l_node->entity, (void *)entname, sizeof(ads_name));

         /* we have the lines points and its structure, so erase the
            lines from autocad */
         if (l_add(rbl, &new_entrb, sizeof(struct resbuf *)) == LIST_ERR)
            return(FALSE);
         memcpy((void *)new_entrb, (void *)&entrb, sizeof(struct resbuf *));
         ads_entdel(entname);
      }
   }

   return(TRUE);
}

static int make_line_list(ads_name ssname, _L_HANDLE ll, _L_HANDLE rbl)
{
   ads_name       entname;
   long           sslen;
   long           i;

   if (ads_sslength(ssname, &sslen) != RTNORM)
      return(FALSE);

   for (i = 0L; i < sslen; i++)
   {
      if (ads_ssname(ssname, i, entname) != RTNORM)
         return(FALSE);

      if (!process_line(entname, ll, rbl))
         return(FALSE);
   }

   return(TRUE);
}

static int calc_intersections(int current_line, struct resbuf *entrb, _L_HANDLE ll)
{
   struct resbuf     *p1;
   struct resbuf     *p2;
   int               idx = 0;
   LINE_END_POINTS   *l_node;
   LINE_END_POINTS   *l2_node;
   ads_point         inters_pt;
   int               j;

   if ((l_node = l_nth(ll, current_line)) == (void *)NULL)
      return(FALSE);

   if ((p1 = a_find_restype(entrb, LINE_START)) == (struct resbuf *)NULL)
      return(FALSE);
   memcpy((void *)cross_list[idx++].inters_pt, (void *)p1->resval.rpoint, sizeof(ads_point));

   if ((p2 = a_find_restype(entrb, LINE_END)) == (struct resbuf *)NULL)
      return(FALSE);

   for (j = 0; j < l_count(ll); j++)
   {
      if (current_line == j)
         continue;

      if ((l2_node = l_nth(ll, j)) == (void *)NULL)
         return(FALSE);

      /* if lines share common end points, then don't break */
      if (!memcmp(l_node->start, l2_node->start, sizeof(ads_point)) ||
          !memcmp(l_node->start, l2_node->end, sizeof(ads_point)) ||
          !memcmp(l_node->end, l2_node->start, sizeof(ads_point)) ||
          !memcmp(l_node->end, l2_node->end, sizeof(ads_point)))
          continue;

      if (parallel(l_node->start, l_node->end, l2_node->start, l2_node->end))
         continue;

      if (ads_inters(l_node->start, l_node->end, l2_node->start, l2_node->end, TRUE, inters_pt) == RTNORM)
         memcpy((void *)cross_list[idx++].inters_pt, (void *)inters_pt, sizeof(ads_point));
   }

   memcpy((void *)cross_list[idx++].inters_pt, (void *)p2->resval.rpoint, sizeof(ads_point));

   return(idx);
}

static int compare_distances(const I_POINT *item1, const I_POINT *item2)
{
   if (item1->dist > item2->dist)
      return(1);

   if (item1->dist < item2->dist)
      return(-1);

   return(0);
}

static void sort_intersections(int idx)
{
   int               k;
   int               j;
   ads_real          lowest;
   ads_real          tmp;

   for (k = 1; k < idx; k++)
      cross_list[k].dist = distance(cross_list[0].inters_pt, cross_list[k].inters_pt);

   qsort((void *)cross_list, idx - 1, sizeof(I_POINT), compare_distances);
}

static int display_lines(int idx, struct resbuf *entrb)
{
   int               k;
   struct resbuf     *p1;
   struct resbuf     *p2;

   if ((p1 = a_find_restype(entrb, LINE_START)) == (struct resbuf *)NULL)
      return(FALSE);

   if ((p2 = a_find_restype(entrb, LINE_END)) == (struct resbuf *)NULL)
      return(FALSE);

   for (k = 0; k < idx - 1; k++)
   {
      memcpy((void *)p1->resval.rpoint, (void *)cross_list[k].inters_pt, sizeof(ads_point));
      memcpy((void *)p2->resval.rpoint, (void *)cross_list[k + 1].inters_pt, sizeof(ads_point));
      ads_entmake(entrb);
   }
   ads_relrb(entrb);

   return(TRUE);
}

static int make_inters_list(_L_HANDLE ll, _L_HANDLE rbl)
{
   int               num_lines;
   int               i;
   void              *tmp;
   struct resbuf     *entrb;
   int               idx;

   num_lines = l_count(ll);

   for (i = 0; i < num_lines; i++)
   {
      if ((tmp = l_nth(rbl, i)) == (void *)NULL)
         return(FALSE);

      memcpy((void *)&entrb, (void *)tmp, sizeof(struct resbuf *));

      idx = calc_intersections(i, entrb, ll);

      sort_intersections(idx); 

      display_lines(idx, entrb);
   }

   return(TRUE);
}

int break_lines(ads_name ssname)
{
   _L_HANDLE      ll = (_L_HANDLE)NULL;
   _L_HANDLE      rbl = (_L_HANDLE)NULL;

   if ((ll = l_gethandle()) == LIST_FULL)
      return(FALSE);

   if ((rbl = l_gethandle()) == LIST_FULL)
      return(FALSE);

   /* make linked list of line end points */
   make_line_list(ssname, ll, rbl);

   /* make linked list of line intersection points */
   make_inters_list(ll, rbl);

   l_free(ll);
   l_free(rbl);

   return(TRUE);
}

