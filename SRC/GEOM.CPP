/*----------------------------------------------------------------------------
geom.c

Line Geometry
by Jeff Hill
----------------------------------------------------------------------------*/

#include <math.h>
#include <float.h>
#include "adslib.h"

#define SMALL_ROUNDUP   0.00001

double intlim(double fig)	/* rounds doubles to an integer value only if they
                              are a small variation from the integer value */
{
	long     fig1;
   long     fig2;
	double   fig3;
   double   tmp;

   fig1 = (long)fig;
   tmp = fig + SMALL_ROUNDUP;
   fig2 = (long)tmp;
   if (fig1 != fig2)
      return((double)fig2);

   tmp = fig - SMALL_ROUNDUP;
   fig2 = (long)tmp;
   if (fig1 != fig2)
      return((double)fig1);

   return(fig);
}

double radtodeg(double rads)
{
	return(180.0 * (rads / _PI));
}

double degtorad(double angl)
{
	return(_PI * (angl / 180.0));
}

double hypot(double x, double y)
{
   return(sqrt((x * x) + (y * y)));
}

double distance(ads_point p1, ads_point p2)
{
   return(hypot(p2[X] - p1[X], p2[Y] - p1[Y]));
}

double delta_x(ads_point p1, ads_point p2)
{
	return(p2[X] - p1[X]);
}

double delta_y(ads_point p1, ads_point p2)
{
	return(p2[Y] - p1[Y]);
}

double angle (ads_point p1, ads_point p2)	
{
	double dr,dx,dy;

	dx = delta_x(p1, p2);
	dy = delta_y(p1, p2);
      
	if ((dr = hypot(dx,dy)) == 0.0) 
      return(0.0);

	if (dy >= 0.0) 
      return(radtodeg(acos(dx/dr)));

	return(360.0 - radtodeg(acos(dx/dr)));
}

int parallel(ads_point p1, ads_point p2, ads_point q1, ads_point q2)
{
   double   ap, aq;

   ap = angle(p1, p2);
   aq = angle(q1, q2);

   if (ap >= 180.0)
      ap -= 180.0;

   if (aq >= 180.0)
      aq -= 180.0;

   if (ap == aq || fabs(ap - aq) == DBL_EPSILON)
      return(TRUE);

   return(FALSE);
}

double adjust_angle(double ang)
{
   while (ang < 0.0 || ang >= 360.0)
   {
      if (ang >= 360.0) 
         ang -= 360.0;    

      if (ang < 0) 
         ang += 360.0;
   }

   return(ang);
}

double min_rel_angle(double ang1, double ang2)
{
   double res;

   ang1 = adjust_angle(ang1);
   ang2 = adjust_angle(ang2);

   res = fabs(ang1 - ang2);

   return(360.0 - res < res ? 360.0 - res : res);
}
