/*----------------------------------------------------------------------------
dscale.cpp

Copies a circle and its enclosed entities to a designated scale

by Jeff Hill
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "adslib.h"
#include "adsdlg.h"
#include "alistman.h"
#include "geom.h"

#define ENTITY_START 0
#define LINE_START   10
#define LINE_END     11
#define SCALE        10.0

int scircle(ads_name circle, ads_name ss)
{
   struct      resbuf *circle_g;
   ads_point   cent;
   ads_real    radius;
   ads_real    ang;
   struct      resbuf *tmp;
   ads_point   pnt1;
   ads_point   pnt2;

   if ((circle_g = ads_entget(circle)) == (struct resbuf *)NULL)
      return(FALSE);
   
   if ((tmp = a_find_restype(circle_g, 10)) == (struct resbuf *)NULL)
   {
      ads_relrb(circle_g);
      return(FALSE);
   }
   cent[X] = tmp->resval.rpoint[X];
   cent[Y] = tmp->resval.rpoint[Y];

   if ((tmp = a_find_restype(circle_g, 40)) == (struct resbuf *)NULL)
   {
      ads_relrb(circle_g);
      return(FALSE);
   }

   radius = tmp->resval.rreal;
   ang = 0.0;

   ads_command(RTSTR, "select", RTNONE);
   while (ang < 180.0) 
   {
     ads_polar(cent, degtorad(ang), radius, pnt1);
     ads_polar(cent, degtorad(ang - 180.0), radius, pnt2);
     ads_command(RTSTR, "C", RTPOINT, pnt1, RTPOINT, pnt2, RTNONE);
     ang += 9.0;
   }

   ads_command(RTSTR, "", RTNONE);

   if (ads_ssget((char *)"P", NULL, NULL, NULL, ss) == RTNORM)
      ads_ssdel(circle, ss);

   return(TRUE);
}

int checkents(ads_name ss)
{
   long     ssl;
   long     i;
   long     count = 0L;
   ads_name ent;
   struct   resbuf *ent_g;
   int      ent_t;
   int      chg = FALSE;
   int      ignore;
   struct   resbuf *tmp;

   if (ads_sslength(ss, &ssl) != RTNORM)
      return(FALSE);

   for (i = 0L; i < ssl; i++)
   {
      if (ads_ssname(ss, i, ent) != RTNORM)
         continue;

      if ((ent_g = ads_entget(ent)) == (struct resbuf *)NULL)
         continue;

      if ((tmp = a_find_restype(ent_g, 0)) == (struct resbuf *)NULL)
         return(FALSE);

      if (!strcmp(tmp->resval.rstring, "POLYLINE") || !strcmp(tmp->resval.rstring, "DIMENSION"))
      {
         ads_command(RTSTR, "explode", RTENAME, ent, RTNONE);
         chg = TRUE;
      }
      else
      if (!strcmp(tmp->resval.rstring, "INSERT"))
      {
         if ((tmp = a_find_restype(ent_g, 2)) == (struct resbuf *)NULL)
            return(FALSE);
         ignore = FALSE;

         if (!strcmp(tmp->resval.rstring, "TICK"))
            ignore = TRUE;
         if (!strcmp(tmp->resval.rstring, "ARROW"))
            ignore = TRUE;
         if (!strcmp(tmp->resval.rstring, "ARROWN"))
            ignore = TRUE;
         if (!strcmp(tmp->resval.rstring, "HOLE"))
            ignore = TRUE;
         if (!strcmp(tmp->resval.rstring, "HOLEH"))
            ignore = TRUE;
         if (!strcmp(tmp->resval.rstring, "HOLEP"))
            ignore = TRUE;
         if (!strcmp(tmp->resval.rstring, "HOLEPH"))
            ignore = TRUE;
         if (!strcmp(tmp->resval.rstring, "KHOLEC"))
            ignore = TRUE;
         if (!strcmp(tmp->resval.rstring, "KHOLEH"))
            ignore = TRUE;
         if (!strcmp(tmp->resval.rstring, "KHOLEO"))
            ignore = TRUE;
         if (!strcmp(tmp->resval.rstring, "KHOLPC"))
            ignore = TRUE;
         if (!strcmp(tmp->resval.rstring, "KHOLPH"))
            ignore = TRUE;
         if (!strcmp(tmp->resval.rstring, "KHOLPO"))
            ignore = TRUE;
         if (!strcmp(tmp->resval.rstring, "FCTICK"))
            ignore = TRUE;

         if (!ignore)
         {
            ads_command(RTSTR, "explode", RTENAME, ent, RTNONE);
            chg = TRUE;
         }
      }

      ads_relrb(ent_g);
   }
   return(chg);
}

int triments(ads_name circle, ads_name ss)
{
   long        i;
   long        ssl;
   ads_name    ent;
   struct      resbuf *ent_g;
   int         ent_t;
   ads_point   cent;
   ads_real    radius;
   struct      resbuf *circle_g;
   struct      resbuf *tmp;
   struct      resbuf *tmp2;
   ads_point   end1;
   ads_point   end2;
   ads_real    dist1;
   ads_real    dist2;

   if ((circle_g = ads_entget(circle)) == (struct resbuf *)NULL)
      return(FALSE);
   
   tmp = a_find_restype(circle_g, 10);
   cent[X] = tmp->resval.rpoint[X];
   cent[Y] = tmp->resval.rpoint[Y];

   tmp = a_find_restype(circle_g, 40);
   radius = tmp->resval.rreal;

   ads_relrb(circle_g);

   if (ads_sslength(ss, &ssl) != RTNORM)
      return(FALSE);

   for (i = 0L; i < ssl; i++)
   {
      if (ads_ssname(ss, i, ent) != RTNORM)
         continue;

      if ((ent_g = ads_entget(ent)) == (struct resbuf *)NULL)
         continue;

      if ((tmp = a_find_restype(ent_g, ENTITY_START)) == (struct resbuf *)NULL)
         return(FALSE);

      if (!strcmp(tmp->resval.rstring, "LINE"))
      {
         if ((tmp2 = a_find_restype(ent_g, LINE_START)) == (struct resbuf *)NULL)
            return(FALSE);

         end1[X] = tmp2->resval.rpoint[X];
         end1[Y] = tmp2->resval.rpoint[Y];

         if ((tmp2 = a_find_restype(ent_g, LINE_END)) == (struct resbuf *)NULL)
            return(FALSE);

         end2[X] = tmp2->resval.rpoint[X];
         end2[Y] = tmp2->resval.rpoint[Y];

         dist1 = distance(cent, end1);
         dist2 = distance(cent, end2);

         if (dist1 > radius)
            ads_command(RTSTR, "trim", RTENAME, circle, RTSTR, "", RTENAME, ent, RTPOINT, end1, RTSTR, "", RTNONE);
         if (dist2 > radius)
            ads_command(RTSTR, "trim", RTENAME, circle, RTSTR, "", RTENAME, ent, RTPOINT, end2, RTSTR, "", RTNONE);
      }
      else
      if (!strcmp(tmp->resval.rstring, "ARC") || !strcmp(tmp->resval.rstring, "CIRCLE"))
      {
         if ((tmp2 = a_find_restype(ent_g, 10)) == (struct resbuf *)NULL)
            return(FALSE);

         end1[X] = tmp2->resval.rpoint[X];
         end1[Y] = tmp2->resval.rpoint[Y];

         if ((tmp2 = a_find_restype(circle_g, 40)) == (struct resbuf *)NULL)
            return(FALSE);

         ads_polar(end1, degtorad(angle(cent, end1)), tmp2->resval.rreal, end2);

         ads_command(RTSTR, "trim", RTENAME, circle, RTSTR, "", RTENAME, ent, RTPOINT, end2, RTSTR, "", RTNONE);
      }

      ads_relrb(ent_g);
   }

   return(TRUE);
}

int detail_scale()
{
   ads_point      pnt1;
   ads_point      pnt2;
   ads_point      textp;
   ads_name       ent;
   ads_name       ocircle;
   ads_name       circle;
   ads_name       ss;
   ads_real       scale_factor;
   int            chkblk;
   struct         resbuf *circle_g;
   struct         resbuf *tmp;
   ads_point      cent;
   ads_point      ocent;
   ads_real       radius;
   char           output_text[80];

   ads_command(RTSTR, "luprec", RTSHORT, 4, RTNONE);
   ads_command(RTSTR, "orthomode", RTSHORT, 0, RTNONE);
   ads_getpoint(NULL, "\nSelect centre point of circle: ", pnt1);
   ads_printf("\nSelect outer edge of circle: ");
   ads_command(RTSTR, "circle", RTPOINT, pnt1, RTSTR, PAUSE, RTNONE);

   if (ads_entlast(circle) != RTNORM)
      return(FALSE);

   scircle(circle, ss);
   ads_ssfree(ss);

   ads_getreal("\nScaling factor for detail: ", &scale_factor);
   ads_getpoint(NULL, "Select insertion point for detail: ", pnt2);

   ads_command(RTSTR, "copy", RTSTR, "p", RTSTR, "", RTPOINT, pnt1, RTPOINT, pnt2, RTNONE);
   ads_command(RTSTR, "copy", RTENAME, circle, RTSTR, "", RTPOINT, pnt1, RTPOINT, pnt2,  RTNONE);

   memcpy((void *)ocircle, (void *)circle, sizeof(ads_name));

   if (ads_entlast(circle) != RTNORM)
      return(FALSE);

   scircle(circle, ss);

   while (checkents(ss))
   {
      ads_ssfree(ss);
      scircle(circle, ss);
      chkblk = TRUE;
   }

   ads_ssfree(ss);

   if (chkblk)
      scircle(circle, ss);
      
   triments(circle, ss);

   ads_command(RTSTR, "scale", RTENAME, circle, RTPICKS, ss, RTSTR, "", RTPOINT, pnt2, RTREAL, scale_factor, RTNONE);

   if ((circle_g = ads_entget(circle)) == (struct resbuf *)NULL)
      return(FALSE);
   tmp = a_find_restype(circle_g, 10);
   cent[X] = tmp->resval.rpoint[X];
   cent[Y] = tmp->resval.rpoint[Y];

   tmp = a_find_restype(circle_g, 40);
   radius = tmp->resval.rreal;

   ads_relrb(circle_g);

   if ((circle_g = ads_entget(ocircle)) == (struct resbuf *)NULL)
      return(FALSE);
   tmp = a_find_restype(circle_g, 10);
   ocent[X] = tmp->resval.rpoint[X];
   ocent[Y] = tmp->resval.rpoint[Y];
   ads_relrb(circle_g);

   ads_command(RTSTR, "line", RTPOINT, cent, RTPOINT, ocent, RTSTR, "", RTNONE);

   if (ads_entlast(ent) != RTNORM)
      return(FALSE);

   ads_command(RTSTR, "trim", RTENAME, circle, RTENAME, ocircle, RTSTR, "", RTENAME, ent, RTPOINT, cent, RTENAME, ent, RTPOINT, ocent, 
      RTSTR, "", RTNONE);

   textp[X] = textp[Y] = 0.0;
   ads_getpoint(NULL, "\nSelect point for text: ", textp);
   if (!textp[X] || !textp[Y])
      ads_polar(cent, degtorad(90.0), radius + (SCALE * 5.0), textp);

   sprintf(output_text, "Detail Scale 1:%.2lf", scale_factor);
   ads_command(RTSTR, "text", RTSTR, "c", RTPOINT, textp, RTREAL, 0.0, RTSTR, output_text, RTNONE); 

   ads_ssfree(ss); 

   ads_printf("about to return\n");
   return RSRSLT;
/*   return(TRUE); */
}

