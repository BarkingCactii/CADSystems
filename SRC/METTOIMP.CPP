/*----------------------------------------------------------------------------
mettoimp.c

Metric to Imperial string conversion routines
by Jeff Hill

----------------------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

#include "mettoimp.h"
#include "imptomet.h"

/* --------------------------------------------------------------------------
   The maximum value a number can differ from its integer portion before
   it is truncated to a whole number
*/

#define ROUNDING_FACTOR 0.0001

/* --------------------------------------------------------------------------
   equal to integer 1
*/

#define WHOLE_NUM       1.0

/* --------------------------------------------------------------------------
   The fractional boundary which determines rounding up or rounding down
*/

#define ROUND_UP_LIMIT  0.5

/* --------------------------------------------------------------------------
   The base number fractions will always expressed in
*/

#define FRAC_DIVH_RES   16

/* --------------------------------------------------------------------------
   An entry to fit the requested imperial format and the number to be converted
   not found.
*/

#define NO_TABLE_ENTRY  -1

/* --------------------------------------------------------------------------
   Bitsets for use with type _imp_format_str
*/

#define DISP_FEET       0x01
#define DISP_INCH       0x02
#define DISP_FRAC       0x04

typedef struct
{
   void  (*print_func)(char *);
   int   format_ref;
   int   format_condition;
} _imp_format_str;

static int     foot_res = 0;
static int     inch_res = 0;
static double  frac_dec_res = 0.0;
static char    frac_tilde_res;
static int     frac_div_res = 0;
static double  inch_frac_res = 0.0;

static void init_locals()
{
   foot_res = 0;
   inch_res = 0;
   frac_dec_res = 0.0;
   frac_tilde_res = 0;
   frac_div_res = 0;
   inch_frac_res = 0.0;
}

static void pf1(char *str)
{
   sprintf(str, "%d\'", foot_res);
}

static void pf2(char *str)
{
   sprintf(str, "%d\'%d", foot_res, inch_res);
}

static void pf3(char *str)
{
   sprintf(str, "%d", inch_res);
}

static void pf4(char *str)
{
   if (frac_div_res)
      sprintf(str, "%d\'%d-%d/%d", foot_res, inch_res, frac_div_res, FRAC_DIVH_RES);
   else
      sprintf(str, "%d\'%d", foot_res, inch_res);
}

static void pf5(char *str)
{
   if (frac_div_res)
      sprintf(str, "%d-%d/%d", inch_res, frac_div_res, FRAC_DIVH_RES);
   else
      sprintf(str, "%d", inch_res);
}

static void pf6(char *str)
{
   if (frac_div_res)
      sprintf(str, "%d/%d", frac_div_res, FRAC_DIVH_RES);
   else
      sprintf(str, "%d", frac_div_res);
}

static void pf7(char *str)
{
   if (frac_div_res)
      sprintf(str, "%d\'%d/%d", foot_res, frac_div_res, FRAC_DIVH_RES);
   else
      sprintf(str, "%d\'", foot_res);
}

static void pf8(char *str)
{
   sprintf(str, "%d\'%d`%c", foot_res, inch_res, frac_tilde_res);
}

static void pf9(char *str)
{
   sprintf(str, "%d`%c", inch_res, frac_tilde_res);
}

static void pf10(char *str)
{
   sprintf(str, "`%c", frac_tilde_res);
}

static void pf11(char *str)
{
   sprintf(str, "%d\'`%c", foot_res, frac_tilde_res);
}

static void pf12(char *str)
{
   if (inch_frac_res)
      sprintf(str, "%d\'%0.4lf", foot_res, inch_frac_res);
   else
      sprintf(str, "%d\'", foot_res);
}

static void pf13(char *str)
{
   sprintf(str, "%0.4lf", inch_frac_res);
}

static void pf14(char *str)
{
   sprintf(str, "%0.4lf", inch_frac_res);
}

static void pf15(char *str)
{
   if (inch_frac_res)
      sprintf(str, "%d\'%0.4lf", foot_res, inch_frac_res);
   else
      sprintf(str, "%d\'", foot_res);
}

static void pf16(char *str)
{
   if (frac_div_res)
      sprintf(str, "%d-%d/%d", inch_res, frac_div_res, FRAC_DIVH_RES);
   else
      sprintf(str, "%d", inch_res);
}

static void pf17(char *str)
{
   sprintf(str, "%d`%c", inch_res, frac_tilde_res); 
}

static void pf18(char *str)
{
   if (inch_frac_res)
      sprintf(str, "%d-%0.4lf", inch_res, inch_frac_res); 
   else
      sprintf(str, "%d", inch_res);
}

static void pf19(char *str)
{
   sprintf(str, "0"); 
}

static _imp_format_str format_table[] = 
{
   pf1, IMP_FORMAT_1 | IMP_FORMAT_2 | IMP_FORMAT_3,
                       DISP_FEET                        ,
   pf2, IMP_FORMAT_1 | IMP_FORMAT_2 | IMP_FORMAT_3,
                       DISP_FEET | DISP_INCH            ,
   pf3, IMP_FORMAT_1 | IMP_FORMAT_2 | IMP_FORMAT_3,
                                   DISP_INCH            ,
   pf4, IMP_FORMAT_1,  DISP_FEET | DISP_INCH | DISP_FRAC,
   pf5, IMP_FORMAT_1,              DISP_INCH | DISP_FRAC,
   pf6, IMP_FORMAT_1,                          DISP_FRAC,
   pf7, IMP_FORMAT_1,  DISP_FEET |             DISP_FRAC,
     
   pf8, IMP_FORMAT_2,  DISP_FEET | DISP_INCH | DISP_FRAC,
   pf9, IMP_FORMAT_2,              DISP_INCH | DISP_FRAC,
   pf10,IMP_FORMAT_2,                          DISP_FRAC,
   pf11,IMP_FORMAT_2,  DISP_FEET |             DISP_FRAC,

   pf12,IMP_FORMAT_3,  DISP_FEET | DISP_INCH | DISP_FRAC,
   pf13,IMP_FORMAT_3,              DISP_INCH | DISP_FRAC,
   pf14,IMP_FORMAT_3,                          DISP_FRAC,
   pf15,IMP_FORMAT_3,  DISP_FEET |             DISP_FRAC,

   pf16,IMP_FORMAT_1,              DISP_INCH | DISP_FRAC,
   pf17,IMP_FORMAT_2,              DISP_INCH | DISP_FRAC,
   pf18,IMP_FORMAT_3,              DISP_INCH | DISP_FRAC,
   pf19,IMP_FORMAT_1 | IMP_FORMAT_2 | IMP_FORMAT_3,
                       0,
};

#define  NUM_IMP_FORMATS    (sizeof(format_table) / sizeof(_imp_format_str))

static double mills_to_inches(double mills)
{
   return(mills / (double)MILLS_PER_INCH);   
}

/* if a num < ROUNDING_FACTOR within a whole number, then the fractional
   part of the number is removed */

static double approx(double num)
{
   return(num);
/*
   if (floor(num) != floor(num + ROUNDING_FACTOR))
      return(floor(num + ROUNDING_FACTOR));
   else
   if (num - floor(num) < ROUNDING_FACTOR)
      return(floor(num));

   return(num); 
*/
}

static int nearest_div(double frac, int base)
{
   double   divisor;
   double   res;
   double   remainder;

/*   divisor = 1.0/(double)base;
   res = approx(frac / divisor);
   if (fmod(frac, divisor) >= ROUND_UP_LIMIT)
      res++;
   return((int)res);
*/
   divisor = 1.0/(double)base;
   res = frac / divisor;
   remainder = res - floor(res);
   if (remainder >= ROUND_UP_LIMIT || fabs(remainder - ROUND_UP_LIMIT) == DBL_EPSILON)
      res++;

   return((int)res);
}


static int calc_str_format(double inch)
{
   int      str_format = 0;
   double   inches = inch;

   foot_res = (int)floor(inches / (double)INCHES_PER_FOOT);
   inches -= ((double)foot_res * (double)INCHES_PER_FOOT);

   inch_frac_res = inches = approx(inches);
   inch_res = (int)floor(inches);
   inches -= (double)inch_res;

   frac_dec_res = inches;

   frac_tilde_res = closest_tilde(frac_dec_res);
   if (frac_tilde_res == (char)WHOLE_NUM)
   {
      if (inch_res + (int)WHOLE_NUM > (int)INCHES_PER_FOOT)
      {
         foot_res++;
         inch_res = (int)NULL;
      }
      else
         inch_res++;

      frac_dec_res = (double)0.0;
   }
   if (frac_tilde_res == (char)NULL)
      frac_dec_res = (double)0.0;

   frac_div_res = nearest_div(frac_dec_res, FRAC_DIVH_RES);
   if (foot_res)
      str_format |= DISP_FEET;
   if (inch_res)
      str_format |= DISP_INCH;
   if (frac_dec_res)
      str_format |= DISP_FRAC;

   return(str_format);
}

static int get_display_format(double num)
{
   double   inches;

   inches = mills_to_inches(num);

   return(calc_str_format(inches));
}

static int match_table_entry(int method, int format_for_str)
{
   int   i;
   int   j = NO_TABLE_ENTRY;
   
   for (i = 0; i < NUM_IMP_FORMATS; i++)
   {
      if (format_table[i].format_ref & method)
      {
         /* check this out *?????????????????????????? */
         if (format_table[i].format_condition == format_for_str)
         {
            j = i;
            break;
         }         
      }
   }
   return(j);
}

static void print_impstr(char *impstr, int table_idx)
{
	(*format_table[table_idx].print_func)(impstr);
}

int metric_to_impstr(char *impstr, double mills, int method)
{
   int   format_for_str;
   int   table_idx;

   init_locals();

   format_for_str = get_display_format(mills);
   if ((table_idx = match_table_entry(method, format_for_str)) == NO_TABLE_ENTRY)
      return(IMPCV_ERR);

   print_impstr(impstr, table_idx);

   return(IMPCV_OK);
}

