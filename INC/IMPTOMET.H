#ifndef  _IMPTOMET_H
#define  _IMPTOMET_H

#define IMPCV_OK           1
#define IMPCV_ERR          0

#define INCHES_PER_FOOT    12.0
#define MILLS_PER_INCH     25.4

/* --------------------------------------------------------------------------
   User routine to convert a imperial format string to a double
*/

char closest_tilde(double);
int impstr_to_metric(char *, double *);

#endif
