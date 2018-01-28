#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "m_hshtbl.h"

static MEMBER_STATS  m_stat;

main()
{
   char  string[80];
   char  string2[80];
   int   ch;

   mf_open();

   while (1)
   {
      printf("\n[A]dd\n");
      printf("[D]elete\n");
      printf("[L]ist (sorted)\n");
      printf("[S]earch\n");
      printf("[M]odify\n");
      printf("[Q]uit\n");

      gets(string);

      ch = toupper(*string);

      if (ch == 'Q')
         break;                       
      if (ch == 'A')
      {
         printf("Enter name to add ");
         gets(m_stat.desc);
         if (strlen(m_stat.desc))
            mf_add(m_stat.desc, &m_stat);
      }
      if (ch == 'D')
      {
         printf("Enter name to delete ");
         gets(string);
         if (strlen(string))
            mf_delete(string);
      }
      if (ch == 'L')
         mf_dump();
      if (ch == 'S')
      {
         printf("Enter name to search ");
         gets(string);
         if (strlen(string))
         {
            mf_search(string, &m_stat);
            printf("found [%s]\n", m_stat.desc);
         }
      }
      if (ch == 'M')
      {
         printf("Enter name to search ");
         gets(string);
         printf("Enter new name ");
         gets(m_stat.desc);
         if (strlen(string) && strlen(m_stat.desc))
            mf_modify(string, m_stat.desc, &m_stat);
      }
   }

   mf_close();
}

