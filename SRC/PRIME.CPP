#include <stdio.h>
#include <math.h>

#define MAX_NUMBER 110000.0

main()
{
   double   i;
   double   j;
   int      prime_num;

   printf("prime numbers are :");

   for (i = 99900.0; i < MAX_NUMBER; i++)
   {
      for (j = 2.0, prime_num = 1; j < i - 1.0; j++)
      {
         if (fmod(i, j) == 0.0)
         {
            prime_num = 0;
            break;
         }
      }
      if (prime_num)
         printf("%d ", (int)i);
   }
}
