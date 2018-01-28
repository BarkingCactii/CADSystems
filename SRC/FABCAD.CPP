/*---------------------------------------------------------------------------
fabcad.cpp

Main function for Fabcad Ads

by Jeff Hill
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "adslib.h"

#include "ctp.h"
#include "dscale.h"

typedef struct 
{ 
   char *func_name; 
   int (*func)(); 
} func_entry;

static func_entry func_table[] =
{ 
   "c:ctp", convert_to_polyline,         
   "c:dscale", detail_scale,         
};

#define NUM_FUNCTIONS (sizeof(func_table) / sizeof(func_entry))


/*-----------------------------------------------------------------------*/
/* FUNCLOAD  --  Define this application's external functions.  Return
                 RTERROR on error, else RTNORM.                   */

static int funcload()
{
   int i;

   for (i = 0; i < NUM_FUNCTIONS; i++) 
      if (!ads_defun(func_table[i].func_name, i))
      {
         ads_fail("failed in defining functions .");
         return RTERROR;
      }
   return RTNORM;
}

/*-----------------------------------------------------------------------*/
/* DOFUN -- Execute external function (called upon an RQSUBR request).  
            Return value from the function executed, RTNORM or RTERROR. */

static int dofun()
{
   struct resbuf *rb;
   int val;

   /* Get the function code and check that it's within range.
       (It can't fail to be, but paranoia doesn't hurt.) */
   if ((val = ads_getfuncode()) < 0 || val >= NUM_FUNCTIONS)
   {
      ads_fail("Received nonexistent function code.");
      return RTERROR;
   }

   /* Call the handler and return its success-failure status. */
/*   return (*func_table[val].func)(); */
   (*func_table[val].func)(); 
   return RSRSLT;
}

/*-----------------------------------------------------------------------*/
/* MAIN - the main routine */

void main(int argc, char *argv[])
{
    short scode = RSRSLT;             /* Normal result code (default) */
    int stat;

    ads_init(argc, argv);             /* Open communication with AutoLISP */

    for ( ;; ) {                      /* Request/Result loop */

        if ((stat = ads_link(scode)) < 0) {
            printf(/*MSG1*/"FACT: bad status from ads_link() = %d\n", stat);
            fflush(stdout);
            exit(1);
        }

        scode = RSRSLT;               /* Reset result code */

        switch (stat) {

        case RQXLOAD:                 /* Load & define functions */
            scode = funcload() == RTNORM ? RSRSLT : RSERR;
            break;

        case RQSUBR:                  /* Handle external function requests */
            scode = dofun() == RTNORM ? RSRSLT : RSERR;
             ads_printf("after func call\n");
            break;

        default:
            break;
        }
   
        ads_printf("test\n");
/*        ads_retvoid(); */
    }
}
