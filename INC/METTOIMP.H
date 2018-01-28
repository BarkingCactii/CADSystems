#ifndef _METTOIMP_H
#define _METTOIMP_H

#include "imptomet.h"

/* --------------------------------------------------------------------------
   Imperial string available formats for use with metric_to_impstr()
*/

#define IMP_FORMAT_1    0x01     /* aa'b-cc/dd */
#define IMP_FORMAT_2    0x02     /* aa'b~c */
#define IMP_FORMAT_3    0x04     /* aa'b.c */

/* --------------------------------------------------------------------------
   User routine to convert a double to a string in imperial formet
*/

int metric_to_impstr(char *, double, int);

#endif
