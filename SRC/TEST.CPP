#include <stdio.h>
#include <float.h>
#include <math.h>

#define MILLS_PER_INCH     25.4

static double inches_to_mills(double inches)
{
   return(inches * (double)MILLS_PER_INCH);   
}

static double f1(char *str1, char *str2)
{
   return((double)(atof(str1)/atof(str2)));
}
main()
{
   double num;
   double n;
   double *num2 = &n;
   char  *str1 = "123456";
   char  *str2 = "123456";
   double t1 = 0.5;
   double t2 = 0.5;


   num = f1(str1, str2);
   *num2 = inches_to_mills(num);
   printf("num %lf num2 %lf\n", num, *num2);

   if (t1 == t2)
      printf("t1 = t2\n");
}
