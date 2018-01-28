/*----------------------------------------------------------------------------
ctp.cpp

Converts entities to polylines

by Jeff Hill
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "adslib.h"
#include "adsdlg.h"
#include "alistman.h"
#include "geom.h"

int covert_to_polyline()
{
/* (defun c:ctp (/ ssl sso ssn en eg typ) */
   ads_name    ss1;
   ads_name    ssn;
   ads_name    sso;
   ads_real    pw;

/* (prompt "\nSelect entities to convert to polyline: ") */
   ads_printf("\nSelect entities to convert to polyline: ");

/*  (command "select" "au" pause) */
   ads_command(RTSTR, "select", RTSTR, "au", RTSTR, PAUSE, RTNONE);

/*   (setq
    sso (ssget "P")
    pw (getreal "\nPolyline width: <10>: ")
    pw (if pw pw 10.0)
    ssn (ssadd)
  )
*/
   if (ads_ssget((char *)"P", NULL, NULL, NULL, sso) != RTNORM)
      return(FALSE);
   pw = 0.0;
   ads_getreal("\nPolyline width: <10>: ", &pw);
   if (!pw)
      pw = 10.0;
   if (ads_ssadd(NULL, NULL, ssn) != RTNORM)
      return(FALSE);

  (if sso
    (progn
      (setq ssl (sslength sso))
      (while (> (setq ssl (1- ssl)) -1)
        (setq
          en (ssname sso ssl)
          eg (entget en)
          typ (cdr (assoc 0 eg))
        )
        (cond
          ((= typ "LINE")
            (command "pedit" en "Y" "")
            (setq ssn (ssadd (entlast) ssn))
          )
          ((= typ "ARC")
            (command "pedit" en "Y" "")
            (setq ssn (ssadd (entlast) ssn))
          )
          ((= typ "PLINE")
            (setq ssn (ssadd (ssname sso ssl) ssn))
          )
          (1 (setq sso (ssdel en sso)))
        )
      )
      (setq ssl (sslength ssn))
      (command "pedit" (ssname ssn 0) "J")
      (while (> (setq ssl (1- ssl)) -1)
        (command (ssname ssn ssl))
      )
    )
  )
  (command "" "W" pw "X")
  (princ)
)
