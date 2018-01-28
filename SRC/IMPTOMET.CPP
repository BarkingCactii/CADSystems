/*----------------------------------------------------------------------------
imptomet.c

Imperial to metric conversion routines
by Jeff Hill

Notes: The conversion routine is completely state driven, with
       1 exception in the case where an imperial expression can be:

       aaa'-bb-cc/dd"
           ^
       This character will put the machine into a state where fractions
       will be processed prematurely. The function convert_substring()
       will test for this explicitly by checking to see if the number
       is in the xx/yy format. If it is not, as the case above, bb will
       be correctly processed as inches.

----------------------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "llistman.h"
#include "alloc.h"
#include "imptomet.h"
#include "mettoimp.h"

#define NO_STATE           0

#define FOOT               1   
#define INCHES             2
#define DEC_FRACTIONS      3
#define FRAC_FRACTIONS     4
#define TILDE_FRACTIONS    5

#define MAX_STR_LEN        80
#define MAX_FRAC_LEN       80
#define MAX_DIVISOR_SYM    1
#define FRAC_HEAD          "0."

#define FOOT_CH            '\''
#define DEC_CH             '.'
#define FRAC_CH            '-'
#define TILDE_CH           '`'
#define DIVISOR_SYM        '/'
#define INCHES_CH          '\"'

typedef struct
{
   char  event_char;
   int   state;
   int   next_state;
   int   max_count;
} _impstate;

typedef struct
{
   char     tilde_char;
   double   metric_equiv;
} _tilde_conversion;

/* --------------------------------------------------------------------------
   State transition table to drive imperial strings to metric
*/

static _impstate state_table[] = 
{
   FOOT_CH,    FOOT,       INCHES,           1,
   DEC_CH,     INCHES,     DEC_FRACTIONS,    1,
   FRAC_CH,    INCHES,     FRAC_FRACTIONS,   2,
   TILDE_CH,   INCHES,     TILDE_FRACTIONS,  1,
};

#define NUM_STATES   (sizeof(state_table) / sizeof(_impstate))

/* --------------------------------------------------------------------------
   Table to store values for the various tildes 
*/

static _tilde_conversion tilde_table[] = 
{
   0,       0.0,
   '1',     1.0/16.0,
   '2',     2.0/16.0,
   '3',     3.0/16.0,
   '4',     4.0/16.0,
   '5',     5.0/16.0,
   '6',     6.0/16.0,
   '7',     7.0/16.0,
   '8',     8.0/16.0,
   '9',     9.0/16.0,
   '0',     10.0/16.0,
   'q',     11.0/16.0,
   'w',     12.0/16.0,
   'e',     13.0/16.0,
   'r',     14.0/16.0,
   't',     15.0/16.0,
   1,       1.0,
};

#define NUM_TILDE    (sizeof(tilde_table) / sizeof(_tilde_conversion))

static int curr_state = NO_STATE;/* reflects the current state of the
                                    state machine */


static double inches_to_mills(double inches)
{
   return(inches * (double)MILLS_PER_INCH);   
}

static double convert_frac_fraction(char *str)
{
   char     fraction[MAX_FRAC_LEN];
   char     *divisor_symbol;
   char     *divisor;
   char     *base;
   double   result;

   strcpy(fraction, str);
   if ((divisor_symbol = strchr(fraction, DIVISOR_SYM)) == (char *)NULL)
      return((double)0.0);
   *divisor_symbol = (char)NULL;

   divisor = fraction;
   base = divisor_symbol + 1;

   if (!atof(base))
      return((double)0.0);

   result = (double)atof(divisor)/(double)atof(base);
   return(result);
}

static double convert_tilde_fraction(char *str)
{
   int   i;

   for (i = 1; i < NUM_TILDE - 1; i++)
   {
      if (strchr(str, (int)tilde_table[i].tilde_char))
         return(tilde_table[i].metric_equiv);
   }
   return((double)0.0);
}

static double convert_fraction(char *str, int state)
{
   char     frac_str[MAX_FRAC_LEN];
   double   num;

   switch ((int)state)
   {
      case(DEC_FRACTIONS):
         strcpy(frac_str, FRAC_HEAD);
         strcat(frac_str, str);
         num = atof(frac_str);
      break;
      case(FRAC_FRACTIONS):
         num = convert_frac_fraction(str);
      break;
      case(TILDE_FRACTIONS):
         num = convert_tilde_fraction(str);
      break;
      default:
         num = 0.0;
      break;
   }
   return(num);
}

static double convert_substring(char *str, int state)
{
   double num = 0.0;

   switch (state)
   {
      case(FOOT):
         num = atof(str) * (double)INCHES_PER_FOOT;         
      break;
      case(INCHES):
         num = atof(str);
      break;
      case(FRAC_FRACTIONS):
         if (strchr(str, DIVISOR_SYM) == (char *)NULL)
         {
            /* not really a fraction, but a whole number */
            num = atof(str);
            break;
         }
      case(DEC_FRACTIONS):
      case(TILDE_FRACTIONS):
         num = convert_fraction(str, state);
      break;
   }
   return(num);
}

static double imperial_to_inches(char *imperial_str, int state)
{
   int      i = 0;
   int      j = 0;
   double   res = 0.0;
   char     tmpstr[MAX_STR_LEN];
   char     *impstr = imperial_str;

   memset(tmpstr, 0, sizeof(tmpstr));
   while (*impstr)
   {
      for (j = 0; j < NUM_STATES; j++)
      {
         if (*impstr == state_table[j].event_char)
         {
            res = convert_substring(tmpstr, state_table[j].state);
            curr_state = state_table[j].next_state; 

            res += imperial_to_inches(impstr + 1, state_table[j].state);
            return(res);
         }
      }
      tmpstr[i++] = *impstr;
      impstr++;
   }

   if (strchr(tmpstr, DIVISOR_SYM))
      curr_state = FRAC_FRACTIONS;

   res += convert_substring(tmpstr, (curr_state ? curr_state : INCHES));

   /* result is in inches */
   return(res);
}

static int nchrstr(char *str, char ch)
{
   int   count = 0;

   while (*str)
   {
      if (*str == ch)
         count++;
      str++;
   }
   return(count);
}

static int valid_char(char ch)
{
   int   i;

   for (i = 0; i < NUM_STATES; i++)
   {
      if (ch == state_table[i].event_char)
         return(IMPCV_OK);
   }

   if (ch == DIVISOR_SYM)
      return(IMPCV_OK);

   return(IMPCV_ERR);
}

static int valid_tilde_char(char ch)
{
   int   i;

   for (i = 1; i < NUM_TILDE - 1; i++)
   {
      if (ch == tilde_table[i].tilde_char)
         return(IMPCV_OK);
   }
   return(IMPCV_ERR);
}

static int valid_imperial_str(char *impstr)
{
   int   i;
   int   j;
   int   frac_found = FALSE;
   char  *prev_frac_ch = (char *)NULL;

   /* make sure the maximum allowable event chars is not exceeded */
   for (i = 0; i < NUM_STATES; i++)
   {
      if (nchrstr(impstr, state_table[i].event_char) > state_table[i].max_count)
         return(IMPCV_ERR);
   }

   if (nchrstr(impstr, DIVISOR_SYM) > MAX_DIVISOR_SYM)
         return(IMPCV_ERR);

   /* make sure there are no illegal characters, except event chars and
      characters following tilde are legal */
   for (i = 0; i < strlen(impstr); i++)
   {
      if (!isdigit(*(impstr+i)))
      {
         if (valid_char(*(impstr+i)) == IMPCV_ERR)
         {
            if (!i || (*(impstr+i-1) != TILDE_CH))
               return(IMPCV_ERR);
         }
      }
   }

   /* since the trailing inch sign has been removed, any other
   occurence of an inch sign is an error */
   if (nchrstr(impstr, INCHES_CH))
         return(IMPCV_ERR);


   /* if we find a tilde then a legal character must follow */
   for (i = 0; i < strlen(impstr); i++)
   {
      if (*(impstr+i) == TILDE_CH)
      {
         if (valid_tilde_char(*(impstr+i+1)) == IMPCV_ERR)
            return(IMPCV_ERR);
      }
   }

   /* number seperators '-' cant appear together */
   for (i = 0, frac_found = FALSE; i < NUM_STATES; i++)
   {
      if (state_table[i].event_char == FRAC_CH)
      {
         frac_found = TRUE;
         break;
      }
   }

   if (frac_found)
      j = i;
   else
      return(IMPCV_ERR);

   if (nchrstr(impstr, state_table[j].event_char) == state_table[j].max_count)
   {
      /* there are more than one '-' */
      for (prev_frac_ch = (char *)NULL, i = 0; i < strlen(impstr); i++)
      {
         if (*(impstr+i) == FRAC_CH)
         {
            if (prev_frac_ch && (prev_frac_ch == (impstr + i) - 1))
               return(IMPCV_ERR);

            prev_frac_ch = impstr + i;
         }
      }
   }

   return(IMPCV_OK);
}

static void preparse_imperial_str(char *str)
{
   char  *origstr = str;
   char  tmpstr[MAX_STR_LEN];
   char  *tmpstrptr = tmpstr;
   int   parse_str_len = 0;

   /* convert letters to lower case */
   memset(tmpstr, 0, sizeof(tmpstr));
   for (; *origstr; origstr++)
   {
      if (!isspace(*origstr))
         *tmpstrptr++ = tolower(*origstr);
   }

   /* remove trailing inches sign */
   parse_str_len = strlen(tmpstr);
   if (parse_str_len)
   {
      if (tmpstr[parse_str_len - 1] == INCHES_CH)
         tmpstr[parse_str_len - 1] = (char)NULL;
   }

   strcpy(str, tmpstr);
}

char closest_tilde(double num)
{
   int      i;
   int      j = NUM_TILDE - 1;   
   double   upper_diff;
   double   lower_diff;

   for (i = 1; i < NUM_TILDE; i++)
   {
      if (tilde_table[i].metric_equiv >= num)
      {
         j = i;
         break;
      }
   }

   upper_diff = fabs(tilde_table[j].metric_equiv - num);
   lower_diff = fabs(num - tilde_table[j - 1].metric_equiv);

   if (upper_diff > lower_diff && fabs(upper_diff - lower_diff) > DBL_EPSILON)
/*   if (fabs(tilde_table[j].metric_equiv - num) > fabs(num - tilde_table[j - 1].metric_equiv) */
      j--;

   return(tilde_table[j].tilde_char);
}

/*----------------------------------------------------------------------------

	User routine to convert an imperial string to metric

----------------------------------------------------------------------------*/

static double   result= 0.0;          /* for debugging only */

int impstr_to_metric(char *impstr, double *mills)
{
   char     str[MAX_STR_LEN + 2];

   if (strlen(impstr) > MAX_STR_LEN)
      return(IMPCV_ERR);

   memset(str, (char)NULL, sizeof(str));
   strcpy(str, impstr);
   preparse_imperial_str(str);

   if (valid_imperial_str(str) == IMPCV_ERR)
   {
      printf("INVALID STRING [%s]\n", str);
      return(IMPCV_ERR);
   }

   curr_state = NO_STATE;
   result = imperial_to_inches(str, INCHES);
   *mills = inches_to_mills(result);

   return(IMPCV_OK);
}

