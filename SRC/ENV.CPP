/*----------------------------------------------------------------------------
env.cpp

Sets and restores the Autocad environment

by Jeff Hill
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "adslib.h"
#include "ads.h"

#include "alloc.h"

typedef struct
{
   struct resbuf  *rb;
   char           *varname;
} SAVE_VARS;

static SAVE_VARS   acad_var[] = 
{
   (struct resbuf *)NULL,  "BLIPMODE",
   (struct resbuf *)NULL,  "HIGHLIGHT",
   (struct resbuf *)NULL,  "UCSICON",
   (struct resbuf *)NULL,  "ORTHOMODE",
   (struct resbuf *)NULL,  "OSMODE", 
   (struct resbuf *)NULL,  "SNAPMODE",
   (struct resbuf *)NULL,  "ANGDIR", 
   (struct resbuf *)NULL,  "SNAPANG",
   (struct resbuf *)NULL,  "CLAYER", 
   (struct resbuf *)NULL,  "TEXTSTYLE",
   (struct resbuf *)NULL,  "CMDECHO",
};

#define NUM_ACAD_VARS (sizeof(acad_var) / sizeof(SAVE_VARS))

int save_environment()
{
   int   i;

   for (i = 0; i < NUM_ACAD_VARS; i++)
   {
      if (alloc_mem((void **)&(acad_var[i].rb), sizeof(struct resbuf)) == MEM_ERR)
         return(FALSE);

      if (ads_getvar(acad_var[i].varname, acad_var[i].rb) != RTNORM)
         return(FALSE);
   }

   return(TRUE);
}

int restore_environment()
{
   int   i;

   for (i = 0; i < NUM_ACAD_VARS; i++)
   {
      if (ads_setvar(acad_var[i].varname, acad_var[i].rb) != RTNORM)
         return(FALSE);

      ads_relrb(acad_var[i].rb);
   }

   return(TRUE);
}
