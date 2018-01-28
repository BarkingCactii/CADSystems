/*    TEMPLATE.C
      Copyright (C) 1990-1992 by Autodesk, Inc.
         
      Permission to use, copy, modify, and distribute this software 
      for any purpose and without fee is hereby granted, provided 
      that the above copyright notice appears in all copies and that 
      both that copyright notice and this permission notice appear in 
      all supporting documentation.

      THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED
      WARRANTY.  ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR
      PURPOSE AND OF MERCHANTABILITY ARE HEREBY DISCLAIMED.


      DESCRIPTION:
  
      This is a template file to demonstrate the structure required by      
      an ADS application.
  
      Prototype for ADS application.
   
      by Amy Berger       
      April 16, 1990
  
      Updated July 30, 1990
  
*/


#include  <stdio.h>
#include  "adslib.h"
#include "adsdlg.h"
#include "geom.h"
#include "breakall.h"

static int loadfuncs();
int adsfunc();


/* MAIN - the main routine */


void
main(argc, argv)
  int argc;
  char *argv[];
{
    int stat;
    short scode = RSRSLT;             /* This is the default result code */

    ads_init(argc, argv);             /* Initialize the interface */


    for ( ;; ) {                      /* Note loop conditions */

        if ((stat = ads_link(scode)) < 0) {

/*            printf("TEMPLATE: bad status from ads_link() = %d\n", stat);*/

            /* Can't use ads_printf to display 
               this message, because the link failed */
            fflush(stdout);
            exit(1);
        }

        scode = RSRSLT;               /* Default return value */

        /* Check for the following cases here */
        switch (stat) {

        case RQXLOAD:                 /* Register your ADS external functions.
                                         Register your function handlers if you
                                         want your ADS functions to be called
                                         transparent to this dispatch loop.
                                         Required for all applications.  */

            scode = loadfuncs() ? RSRSLT : RSERR;
            break;

        case RQSUBR:                  /* This case is normally expanded to 
                                         select one of the application's 
                                         external functions */
/*            ads_printf("ads_getfuncode() = %d\n", ads_getfuncode());*/
            adsfunc();
            break;

        case RQXUNLD:                 /* Do C program cleanup here.
                                         Not required unless you need to
                                         clean up your own allocated resources.

                                         Note: You don't have to undefine ADS
                                         functions.  LISP does it for you.  */
            break;

        case RQSAVE:                  /* AutoCAD SAVE command notification.
                                         You can use it for your own database
                                         synchronization.  Not required.  */
            break;

        case RQQUIT:                  /* AutoCAD QUIT command notification.
                                         Not required.  */
            break;

        case RQEND:                   /* AutoCAD END command notification.
                                         Not required.  */
            break;

        default:
            break;
        }
    }
}


/* LOADFUNCS  --  Define external functions with AutoLISP.

                  Normally expanded to call ads_defun() once for each
                   external function to be defined, assigning each one a
                  different ADS function code.  ads_regfunc() is then
                  called to specify the handler function for each ADS
                  function code.
*/
static int loadfuncs()
{
    if (ads_defun("C:JEFF", 0) == RTNORM) {   /* Define function */
  /*      ads_regfunc(adsfunc, 0);*/      /* Register handler */
/*        l_init();*/
        return 1;
    } else
        return 0;
}


/* ADSFUNC  --  Sample handler for ADS function code 0.

                This function will handle (ADSFUNC) calls.  You can have
                one handler for each external function, or use one handler
                for several functions.  ads_getfuncode() tells the handler
                which function (ADS function code) it's dealing with.

                If you choose to use the RQSUBR method, you should place
                this function call following the RQSUBR switch statement.
*/
int adsfunc()
{
   ads_point   p1, p2;
   ads_name       ssname;

/*   ads_command(RTSTR, "select", RTNONE);*/
/*   ads_printf("Select 2 points for SS\n");
   ads_getpoint(NULL, "First point:\n", p1);
   ads_getpoint(NULL, "Second point:\n", p2);
*/

   if (ads_ssget((char *)NULL, (void *)NULL, (void *)NULL, NULL, ssname) != RTNORM)
      return RSRSLT;

   break_lines(ssname);

   ads_ssfree(ssname);

   return RSRSLT;                    /* Normal completion */

}

