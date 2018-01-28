#ifndef _M_HSHTBL_H
#define _M_HSHTBL_H

#include "m_stats.h"

#define MAX_M_HASH_ITEMS   199

#define  REC_EMPTY         1
#define  REC_ACTIVE        2
#define  REC_DELETED       3

#define  NO_REF            -1

typedef struct
{
   char           name[MAX_MEMBER_DESC_STR + 1];   /* member name */
   int            next_sorted;                     /* pointer to next in table */
   int            status;                          /* record status */
   int            abs_ref;                         /* absolute position in file */

} M_HASH_ITEM;

typedef struct
{
   int            next_slot;                       /* next available slot */
   int            first_sorted;                    /* first logical record */
   int            table_full;                      /* TRUE if full */

} H_CNTL_REC;

typedef struct
{
   H_CNTL_REC     control;
   M_HASH_ITEM    item[MAX_M_HASH_ITEMS + 1];

} M_HASH_TABLE;

void mf_dump();
int mf_open();
void mf_close();
int mf_modify(char *, char *, MEMBER_STATS *);
int mf_search(char *, MEMBER_STATS *);
int mf_add(char *, MEMBER_STATS *);
int mf_delete(char *);

#endif
