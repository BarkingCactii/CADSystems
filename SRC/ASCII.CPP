#include <stdio.h>

main()
{
   int   i;

   for (i = 30; i < 128; i++)
   {
      printf("%d\t%c    ", i, (char)i);
      if (!(i % 5) && i)
         printf("\n");
   }
}
