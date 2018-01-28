/*----------------------------------------------------------------------------
dtxtwid.c

Calculates the width of a text entity. Works for WCS only

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

#define TEXT_CLEARANCE  2.0

static ads_point        p1;
static ads_point        p2;
static ads_point        p3;
static ads_point        p4;

int get_text_bounds(ads_name tname, ads_point sel_pt, ads_real tangle, ads_real *text_width)
{
   struct resbuf  *textent;
   struct resbuf  *found;
   ads_real       aangle;
   ads_point      lowleft;
   ads_point      upright;

   if ((textent = ads_entget(tname)) == (struct resbuf *)NULL)
      return(DIM_ERR);

   if ((found = a_find_restype(textent, UCS_ORIGIN)) == (struct resbuf *)NULL)
      return(DIM_ERR);

   if (ads_textbox(textent, lowleft, upright) != RTNORM)
   {
      ads_relrb(textent);
      return(DIM_ERR);
   }

   lowleft[X] = found->resval.rpoint[X] + lowleft[X];
   lowleft[Y] = found->resval.rpoint[Y] + lowleft[Y];

   upright[X] = found->resval.rpoint[X] + upright[X];
   upright[Y] = found->resval.rpoint[Y] + upright[Y];

   /* determine the width of text */
/*   aangle = min_rel_angle(angle(lowleft, upright), tangle); */
/*   aangle = fabs(adjust_angle(angle(lowleft, upright) - tangle)); */
/*   *text_width = fabs(cos(degtorad(aangle)) * distance(lowleft, upright)) + TEXT_CLEARANCE; */
   p1[X] = lowleft[X] * cos(tangle) - lowleft[Y] * sin(tangle);
   p1[Y] = lowleft[X] * sin(tangle) + lowleft[Y] * cos(tangle);

   p2[X] = upright[X] * cos(tangle) - lowleft[Y] * sin(tangle);
   p2[Y] = upright[X] * sin(tangle) + lowleft[Y] * cos(tangle);

   p3[X] = upright[X] * cos(tangle) - upright[Y] * sin(tangle);
   p3[Y] = upright[X] * sin(tangle) + upright[Y] * cos(tangle);

   p4[X] = lowleft[X] * cos(tangle) - upright[Y] * sin(tangle);
   p4[Y] = lowleft[X] * sin(tangle) + upright[Y] * cos(tangle);

   *text_width = distance(p1, p2) + TEXT_CLEARANCE;
   return(DIM_OK);
}
