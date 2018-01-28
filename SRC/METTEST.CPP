#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

#include "mettoimp.h"
#include "imptomet.h"

main()
{
   int      i;
   double   millis = 0.0;
   char     str[15][80]; 
   int      method[] = {
      IMP_FORMAT_2,
      IMP_FORMAT_3,
      IMP_FORMAT_1,
      IMP_FORMAT_2,
      IMP_FORMAT_3,
      IMP_FORMAT_2,
      IMP_FORMAT_2,
      IMP_FORMAT_1,
      IMP_FORMAT_1,
      IMP_FORMAT_1,
      IMP_FORMAT_3,
      IMP_FORMAT_1,
      IMP_FORMAT_1,
      IMP_FORMAT_2,
   };

   do
   {
      strcpy(str[0], "132\'7~1\"");
      strcpy(str[1], "132\'7.5\"");
      strcpy(str[2], "132");
      strcpy(str[3], "132\'~1");
      strcpy(str[4], "132\'.5");
      strcpy(str[5], "132\'7~Q\"");
      strcpy(str[6], "132~t\"");
      strcpy(str[7], "132\'7-12/16\"");
      strcpy(str[8], "132\'-12/16\"");
      strcpy(str[9], "132\'7-3/4\"");
      strcpy(str[10], "132\'");
      strcpy(str[11], "132'-7\"");
      strcpy(str[12], "132'-7-12/16\"");

      for (i = 0; i < 13; i++)
      {
         if (impstr_to_metric(str[i], &millis) != IMPCV_ERR)
         {
            printf("[%13s] = [%06.04lf] mls <-> ", str[i], millis);
            if (metric_to_impstr(str[i], millis, method[i]) == IMPCV_OK)
               printf("[%15s] method [%d]\n", str[i], method[i]);
         }
      }

      if (gets(str[13]))
      {
         if (impstr_to_metric(str[13], &millis) != IMPCV_ERR)
         {
            printf("[%13s] = [%06.04lf] mls <-> ", str[13], millis);
            if (metric_to_impstr(str[13], millis, method[13]) == IMPCV_OK)
               printf("[%15s] method [%d]\n", str[13], method[13]);
            else
               printf("m->i conversion ERROR\n");
         }
         else
            printf("i->m conversion ERROR\n");
      }
      else
         break;
   } while (1);
}


