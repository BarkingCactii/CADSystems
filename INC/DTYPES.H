/*----------------------------------------------------------------------------
dtypes.h

Data storage definitions
by Jeff Hill

The following data types must match the storage size below

        char            1 byte
        short           2 bytes
        int             2 bytes
        long            4 bytes
        float           8 bytes
        double          8 bytes
        
to meet standardization accross hardware platforms

----------------------------------------------------------------------------*/
#ifndef _DTYPES_H
#define _DTYPES_H

/* #define HIGH_C  */                 /* the profile file should do this */

/* #ifdef HIGH_C   */                 /* data type declaration for HighC/DOS */

#define _CHAR    char
#define _UINT    unsigned short  
#define _INT     short           
#define _SHORT   short           
#define _USHORT  short           
#define _LONG    long            
#define _ULONG   unsigned long   
#define _FLOAT   double          /* storage in IEEE format */
#define _DOUBLE  double          /* storage in IEEE format */

/* #endif */

/* use for sizeof(type) calculations */
#define SIZE_INT         sizeof(_INT)
#define SIZE_UINT        sizeof(_UINT)
#define SIZE_SHORT       sizeof(_SHORT)
#define SIZE_USHORT      sizeof(_USHORT)
#define SIZE_LONG        sizeof(_LONG)
#define SIZE_ULONG       sizeof(_ULONG)
#define SIZE_FLOAT       sizeof(_FLOAT)
#define SIZE_DOUBLE      sizeof(_DOUBLE)

#endif
