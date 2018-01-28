#ifndef _MEMBIO_H
#define _MEMBIO_H

#define  REC_EMPTY      1
#define  REC_ACTIVE     2
#define  REC_DELETED    3

typedef struct 
{
   MEMBER_STATS   data;
   int            status;
} MEMBER_REC;

#endif
