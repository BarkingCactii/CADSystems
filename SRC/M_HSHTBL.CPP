/*----------------------------------------------------------------------------
m_hshtbl.cpp

Member data hash table routines

by Jeff Hill
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "m_hshtbl.h"

#ifndef TRUE
#define TRUE   1
#endif

#ifndef FALSE
#define FALSE  0
#endif

#define HASH_TABLE_NAME "member.dat"
#define ASCII_MODIFIER  31
#define BASE            64

static M_HASH_TABLE  m_tab;

static FILE *mem_fp;
 
static int hash(char *key)
{
   int   value;

   for (value = 0; *key; key++)
      value = ((BASE * value) + (*key - ASCII_MODIFIER)) % MAX_M_HASH_ITEMS;

   return(value);
}

static int get_abs_ref()
{
   int   curr_slot = m_tab.control.next_slot;
   int   i;

   for (i = 0; i < MAX_M_HASH_ITEMS; i++)
   {
      if (m_tab.item[i].status == REC_DELETED)
      {
         m_tab.item[i].status = REC_EMPTY;
/*         printf("found deleted slot [%d] ref [%d]\n", i, m_tab.item[i].abs_ref);*/
         return(m_tab.item[i].abs_ref);
      }
   }

/*   printf("adding new ref %d\n", curr_slot);*/
   m_tab.control.next_slot++;
   return(curr_slot);
}

static int read_record(int num, MEMBER_STATS *m)
{
   if (fseek(mem_fp, (long)(sizeof(m_tab) + (m_tab.item[num].abs_ref * sizeof(MEMBER_STATS))), SEEK_SET))
      return(FALSE);

   if (!fread((char *)m, sizeof(MEMBER_STATS), 1, mem_fp))
      return(FALSE);
   
/*   printf("desc [%s]\n", m->desc); */

   return(TRUE);      
}

static int write_record(int num, MEMBER_STATS *m)
{
   if (fseek(mem_fp, (long)(sizeof(m_tab) + (m_tab.item[num].abs_ref * sizeof(MEMBER_STATS))), SEEK_SET))
      if (fseek(mem_fp, (long)0L, SEEK_END))
         return(FALSE);

   if (!fwrite((char *)m, sizeof(MEMBER_STATS), 1, mem_fp))
      return(FALSE);
   
   return(TRUE);      
}


static void del_sort(int idx)
{
   int   prev = NO_REF;
   int   i;

   for (i = m_tab.control.first_sorted; i != NO_REF; prev = i, i = m_tab.item[i].next_sorted)
   {
      if (i == idx)
      {
         if (prev != NO_REF)
            m_tab.item[prev].next_sorted = m_tab.item[i].next_sorted;
         else
            m_tab.control.first_sorted = m_tab.item[i].next_sorted;
      }
   }
}

static int sort_hash(char *key, int idx)
{
   int   i;
   int   prev = NO_REF;

   for (i = m_tab.control.first_sorted; i != NO_REF; prev = i, i = m_tab.item[i].next_sorted)
   {
      if (strcmp(key, m_tab.item[i].name) <= 0)
      {
         if (prev == NO_REF)
            m_tab.control.first_sorted = idx;
         else
            m_tab.item[prev].next_sorted = idx;

         return(i);
      }
   }

   if (prev == NO_REF)
      m_tab.control.first_sorted = idx;
   else
      m_tab.item[prev].next_sorted = idx;

   return(NO_REF);
}



static void initialize_hash_table()
{
   int   i;

   memset((char *)&m_tab, 0, sizeof(m_tab));

   m_tab.control.next_slot = 0;
   m_tab.control.first_sorted = NO_REF;
   m_tab.control.table_full = FALSE;

   for (i = 0; i < MAX_M_HASH_ITEMS; i++)
   {
      m_tab.item[i].next_sorted = NO_REF;
      m_tab.item[i].status = REC_EMPTY;
      m_tab.item[i].abs_ref = NO_REF;
   }
}

static int create_new_file()
{
   initialize_hash_table();

   if (!fwrite((char *)&m_tab, sizeof(m_tab), 1, mem_fp))
      return(FALSE);

   return(TRUE);
}

static int load_table()
{
   if (!fread((char *)&m_tab, sizeof(m_tab), 1, mem_fp))
      return(FALSE);

   return(TRUE);
}

static int open_file()
{
   if ((mem_fp = fopen(HASH_TABLE_NAME, "r+b")) == (FILE *)NULL)
   {
      if ((mem_fp = fopen(HASH_TABLE_NAME, "w+b")) == (FILE *)NULL)
         return(FALSE);
      else
      {
         if (!create_new_file())
            return(FALSE);
      }
   }

   if (!load_table())
      return(FALSE);

   return(TRUE);
}

void mf_dump()
{
   int   i;

   for (i = 0; i < MAX_M_HASH_ITEMS; i++)
      if (strlen(m_tab.item[i].name))
         printf("[%d] [%s]\n", i, m_tab.item[i].name);

   printf("Sorted\n");
   for (i = m_tab.control.first_sorted; i != NO_REF; i = m_tab.item[i].next_sorted)
      if (strlen(m_tab.item[i].name))
         printf("[%d] [%s] abs[%d]\n", i, m_tab.item[i].name, m_tab.item[i].abs_ref);
}

int mf_delete(char *key)
{
   int   x = hash(key);
   int   y;

   y = x;
   while (strcmp(m_tab.item[x].name, key))
   {
      x = (x + 1) % MAX_M_HASH_ITEMS;
      if (x == y)
      {
/*         printf("not found\n"); */
         return(FALSE);
      }
   }

   del_sort(x);

   m_tab.item[x].status = REC_DELETED;
   memset((char *)m_tab.item[x].name, 0, sizeof(m_tab.item[x].name));

   return(TRUE);
}

int mf_add(char *key, MEMBER_STATS *m)
{
   int  x = hash(key);

   while (m_tab.item[x].status == REC_ACTIVE)
   {
      if (!strcmp(m_tab.item[x].name, key))
      {
         /* since we don't allow duplicate keys, we modify the existing rec */
         if (!write_record(x, m))
            return(FALSE);
         return(TRUE);
      }

      x = (x + 1) % MAX_M_HASH_ITEMS;
   }

   m_tab.item[x].abs_ref = get_abs_ref();
   strcpy(m_tab.item[x].name,key);
   m_tab.item[x].status = REC_ACTIVE;
   m_tab.item[x].next_sorted = sort_hash(key, x);

   if (!write_record(x, m))
      return(FALSE);

   return(TRUE);
}

int mf_modify(char *olds, char *news, MEMBER_STATS *m)
{
   if (mf_delete(olds))
      mf_add(news, m);
   else
      return(FALSE);

   return(TRUE);
}

int mf_search(char *key, MEMBER_STATS *s)
{
   int   x = hash(key);
   int   y;

   y = x;
   while (strcmp(m_tab.item[x].name, key))
   {
      x = (x + 1) % MAX_M_HASH_ITEMS;
      if (x == y)
      {
/*         printf("not found\n");*/
         return(FALSE);
      }
   }

   if (!read_record(x, s))
      return(FALSE);

   return(TRUE);
}

void mf_close()
{
   fseek(mem_fp, (long)0L, SEEK_SET);

   fwrite((char *)&m_tab, sizeof(m_tab), 1, mem_fp);

   fclose(mem_fp);
}

int mf_open()
{
   if (!open_file())
      return(FALSE);

   return(TRUE);
}

