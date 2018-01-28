/*****************************************************************************
  Name: msq.c

  Description: Show program to use sqldial.exp.

  Author: Zimin R.
          Autodesk, Inc.
          St. Petersburg, Russia.

 *****************************************************************************

    msq.c
        Copyright (C) 1991-1992 by Autodesk, Inc.

        Permission to use, copy, modify, and distribute this software 
        for any purpose and without fee is hereby granted, provided 
        that the above copyright notice appears in all copies and that 
        both that copyright notice and this permission notice appear in 
        all supporting documentation.

        THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED
        WARRANTY.  ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR
        PURPOSE AND OF MERCHANTABILITY ARE HEREBY DISCLAIMED.
 *****************************************************************************

  Entry Points:

    void
    main (argc, argv)
        int argc;                     [arguments number]
        char **argv;                  [arguments array]

    static int
    loadfuncs ()

    static void
    initsq ()

    static void
    endsq ()

    static void
    msq ()

    static void
    compile (stm)
        char *stm;                    [SQL statement]

    static void
    scan ()

    static void
    print_row ()

    static void
    drvsq ()

    static void
    fullsq ()

    static void
    print_acad_str (str)
        char *str;                    [String to print]

    static void
    print_prompt ()

 *****************************************************************************

  Modification History:
         03/27/1992    - Creation.

 *****************************************************************************

  Bugs and restriction on use:
        Platform    PC/AT-386, AutoCAD Release 12., HighC 386, 
                    DOS Extender  - PharLap 386. 

 *****************************************************************************

  Notes:
        1. There are following commands:

            INITSQ -    Load the driver and connect to the database.

            ENDSQ  -    Disconnect the database and terminate the driver.

            DRVSQ  -    Make SQL statement in dialogue (invoke(sqldial)).
                        Program offers SQL syntax witch current driver
                        supports.

            FULLSQ -    Make SQL statement in dialogue (invoke(sqldial)).
                        Program offers full SQL syntax.

            MSQ    -    Make SQL statement manually.

        2. You have to load sqldial.exp before execution DRVSQ and FULLSQ
           commands.

*****************************************************************************/
/* INCLUDES */
/****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "adslib.h"
#include "asi.h"
/****************************************************************************/
/* DEFINES */
/****************************************************************************/
#define MAX_BUFF 132                  /* Maximum string length in
                                         ads_printf() and ads_getstring()
                                         without '\0' */
#ifdef _
#undef _
#endif  /* _ */
/* #define _(x) () */
#ifdef UNIX
#define _max(a,b)  (((a) > (b)) ? (a) : (b))
#endif


/****************************************************************************/
/* TYPEDEFS */
/****************************************************************************/
/****************************************************************************/
/* FUNCTIONS */
/****************************************************************************/
static void compile         (char *);
static void drvsq           (void);
static void endsq           (void);
static void fullsq          (void);
static void initsq          (void);
static int  loadfuncs       (void);
void        main            (int, char **);
static void msq             (void);
static void compileSQL      (char *);
static void print_acad_str(char *);
static void print_prompt    (void);
static void print_row       (void);
static void scan            (void);
/****************************************************************************/
/* GLOBAL VARIABLES */
/****************************************************************************/
static ASIHANDLE    drvhand;          /* Driver handle */
static ASIHANDLE    cnchand;          /* Connection handle */
static ASIHANDLE    comhand;          /* Communication handle */
static short drv_active = FALSE;      /* driver flag */
static short sql_active = FALSE;      /* database flag */
static char drvname[MAX_BUFF+1];      /* Driver name */
static char basename[MAX_BUFF+1];     /* Database name */
static char username[MAX_BUFF+1];     /* User name */
static char password[MAX_BUFF+1];     /* Password */
/****************************************************************************/
/*.doc main(external) */
/*+
   ADS application main entry point.
-*/
/****************************************************************************/
void
/*FCN*/main (argc, argv)
  int argc;                           /* arguments number */
  char **argv;                        /* arguments array */
{
    int stat;
    short scode = RSRSLT;             /* Default result code */

    ads_init (argc, argv);            /* Initialize the interface */

    while (TRUE) {                    /* Note loop conditions */

        if ((stat = ads_link (scode)) < 0) {
            printf("\nCannot link with AutoCAD (stat = %d)\n", stat);
            fflush (stdout);
            exit (1);
        }
        scode = RSRSLT;               /* Default return value */

        /* Check for the following cases here */
        switch (stat) {

        case RQXLOAD:

            scode = loadfuncs ();     /* Register ADS external functions */
            if (scode == RSRSLT)
                asi_initsql ();       /* Initialization of 
                                         SQL-interface */
            break;

        case RQXUNLD:
        case RQQUIT:
        case RQEND:
            if (sql_active) {         /* Logoff from database */
                asi_lof (&cnchand);
                sql_active = FALSE;
            }
            if (drv_active) {         /* Terminate driver */
                asi_termdrv (&drvhand);
                drv_active = FALSE;
            }
            asi_termsql ();           /* Terminate SQL-interface */
        }
    }
}
/****************************************************************************/
/*.doc loadfuncs(internal) */
/*+
   Load functions.
-*/
/****************************************************************************/
typedef struct {
    char        *cmdname;
    void        (*cmdfunc)(void);
} cmdtab;

static cmdtab cmd[] = 
{ 
      {"C:INITSQ",    initsq},
      {"C:ENDSQ",     endsq} ,
      {"C:DRVSQ",     drvsq} ,
      {"C:FULLSQ",    fullsq},
      {"C:MSQ",       msq}    
};

static int
/*FCN*/loadfuncs ()
{
    int i = 5;

    while (i--) {                     /* Define function */
        if (ads_defun(cmd[i].cmdname, i) != RTNORM)
            return RSERR;
        if (ads_regfunc((int(*)())cmd[i].cmdfunc, i) != RTNORM)
            ads_abort ("\nCannot register");
    }
    return RSRSLT;
}
/****************************************************************************/
/*.doc initsq(internal) */
/*+
   Initialization if SQL - interface. 
-*/
/****************************************************************************/
static void
/*FCN*/initsq ()
{
    int ret_code;

    /* 1. Logoff from old database */

    if (sql_active) {
        asi_lof (&cnchand);
        sql_active = FALSE;
    }

    /* 2. Terminate old driver */

    if (drv_active) {
        asi_termdrv (&drvhand);
        drv_active = FALSE;
    }

    /* 3. Load SQL driver */

    if ((ret_code = ads_getstring (FALSE,
                                   "\nEnter SQL driver name: ",
                                   drvname) == RTNORM) == TRUE) {
        drv_active = ret_code =  asi_initdrv (&drvhand, drvname)
                     == ASI_GOOD;
        ads_printf ("\n%s\n", ret_code ? "Driver loaded."
                                       :  asi_errmsg (&drvhand));

        /* 4. If driver is loaded, then connect to database. */

        if (ret_code == TRUE &&
            (ret_code = ads_getstring (FALSE, "\nDatabase name ->", basename)
             == RTNORM           &&
             ads_getstring (FALSE, "\nUser name ->", username)
             == RTNORM           &&
             ads_getstring (FALSE, "\nPassword ->", password)
             == RTNORM) == TRUE) {

            /* Logon to database */

            sql_active = ret_code = asi_lon (&cnchand, &drvhand, basename,
                                             username, password) == ASI_GOOD;
            ads_printf ("\n%s\n", ret_code ? "Connected to database."
                                           :  asi_errmsg (&cnchand));
        }
    }

    ads_retvoid ();
}
/****************************************************************************/
/*.doc endsq(internal) */
/*+
   End of SQL interface. 
-*/
/****************************************************************************/
static void
/*FCN*/endsq ()
{
    if (sql_active) asi_lof (&cnchand);
    if (drv_active) asi_termdrv (&drvhand);
    drv_active = sql_active = FALSE;

    ads_retvoid ();
}
/****************************************************************************/
/*.doc msq(internal) */
/*+
   Manually make and execute SQL statement.
-*/
/****************************************************************************/
static void
/*FCN*/msq ()
{
    char statement[MAX_BUFF+1];

    if (sql_active == FALSE) ads_prompt ("\nNo active database");
    else {
        print_prompt ();
        if (ads_getstring (1, NULL, statement) == RTNORM &&
            *statement != '\0')     compileSQL (statement);
    }

    ads_retvoid ();
}
/****************************************************************************/
/*.doc compile(internal) */
/*+
   Execute SQL statement.
-*/
/****************************************************************************/
static void
/*FCN*/compileSQL (char *stm)
                         /* SQL statement */
{
    int err_position;

    asi_ohdl (&comhand, &cnchand);    /* Open communication handle */

    if (asi_com (&comhand, stm) == ASI_BAD) {

        /* Syntax error processing. */

        print_acad_str (stm);
        err_position = asi_synerrpos (&comhand);
        ads_prompt ("\n");
        while (err_position--)  ads_prompt (" ");
        ads_prompt ("");
        ads_printf ("\n%s", asi_errmsg (&comhand));
    } else {

        /* Execute SQL statement. */

        if (asi_exe (&comhand) == ASI_BAD)
            ads_printf ("\n%s", asi_errmsg (&comhand));
        else if (asi_stm (&comhand) == ASI_CURSOR)
            scan ();
        else
            ads_printf ("\nOK");
    }

    asi_chdl (&comhand);
}
/****************************************************************************/
/*.doc scan(internal) */
/*+
   Scan results of SQL CURSOR statement.
-*/
/****************************************************************************/
static void
/*FCN*/scan (void)
{
    char input[9],
         last = 'N',
         *prompt;

    ads_printf ("\n%d rows selected", asi_rowqty(&comhand));
    asi_fet (&comhand);
    if (asi_currow (&comhand) == -2)    ads_prompt ("\nNothing to fetch");
    else
        while (last != 'E') {
            print_row ();
            prompt = (last == 'N')
                     ? "\nExit/First/Last/Previous/<Next>: "
                     : "\nExit/First/Last/Next/<Previous>: ";
            ads_initget (0, "First Last Next Previous Exit");
            switch (ads_getkword (prompt, input)) {
            case RTNORM:
                break;

            case RTNONE:
                *input = last;
                break;

            default:
                *input = 'E';
            }
            last = *input;

            switch (last) {
            case 'N':
                asi_fet (&comhand);
                if (asi_currow (&comhand) == -2)
                    asi_fbr (&comhand);
                break;

            case 'P':
                asi_fbk (&comhand);
                if (asi_currow (&comhand) == -1)
                    asi_ftr (&comhand);
                break;

            case 'F':
                asi_ftr (&comhand);
                last = 'N';
                break;

            case 'L':
                asi_fbr (&comhand);
                last = 'P';
            }
        }
}
/****************************************************************************/
/*.doc print_row(internal) */
/*+
   Print current row. 
-*/
/****************************************************************************/
static void
/*FCN*/print_row ()
{
    char format[81],
         value[256];
    int j,
        type,
        width;
    COLDSC coldsc;

    for (j = 0; asi_cds (&comhand, j, &coldsc) == ASI_GOOD; j++) {
        width = _max (coldsc.precision, (int)(strlen(coldsc.colname)));
        asi_cvl (&comhand, j, value, 255, &type);
        ads_printf ("\n%3d|%-15s", asi_currow(&comhand)+1, coldsc.colname);
        switch (type) {
        case ASI_SINT:
            sprintf (format, "%%-%dd ", width);
            ads_printf (format, *(integer *)value);
            break;

        case ASI_SREAL:
            sprintf (format, "%%-%d.%df ", width, coldsc.scale);
            ads_printf (format, *(real *) value);
            break;

        case ASI_SCHAR:
            if (width > MAX_BUFF)   width = MAX_BUFF;
            sprintf (format, "%%-%ds ", width);
            ads_printf (format, value);
            break;

        case ASI_SNULL:
            ads_printf ("%s", "Null");
        }
    }
}
/****************************************************************************/
/*.doc drvsq(internal) */
/*+
   Make SQL statement with syntax witch current driver supports.
-*/
/****************************************************************************/
static void
/*FCN*/drvsq ()
{
    struct resbuf *s = NULL,
                  *call;

    if (sql_active == FALSE) ads_prompt ("\nNo active database");
    else {
        print_prompt ();
        call = ads_buildlist (RTSTR, "c:sqldial",
                              RTSTR, drvname,
                              NULL);
        if (ads_invoke (call, &s) != RTNORM)
            ads_prompt ("\nCan't invoke sqldial.exp");
        if (s != NULL) {
            print_acad_str (s->resval.rstring);
            compileSQL (s->resval.rstring);
        }
        ads_relrb (call);
        ads_relrb (s);
    }
    ads_retvoid ();
}
/****************************************************************************/
/*.doc fullsq(internal) */
/*+
   Make SQL statement with full syntax.
-*/
/****************************************************************************/
static void
/*FCN*/fullsq ()
{
    struct resbuf *s = NULL,
                  *call;

    if (sql_active == FALSE) ads_prompt ("\nNo active database");
    else {
        print_prompt ();
        call = ads_buildlist (RTSTR, "c:sqldial",
                              NULL);
        if (ads_invoke (call, &s) != RTNORM)
            ads_prompt ("\nCan't invoke sqldial.exp");
        if (s != NULL) {
            print_acad_str (s->resval.rstring);
            compileSQL (s->resval.rstring);
        }
        ads_relrb (call);
        ads_relrb (s);
    }
    ads_retvoid ();
}
/****************************************************************************/
/*.doc print_acad_str(internal) */
/*+
   Print long string (more 132 bytes).
-*/
/****************************************************************************/
static void
/*FCN*/print_acad_str (char *str)
                         /* String to print */
{
    char c;
    size_t l;

    ads_printf ("\n");
    while ((l = ((l = strlen(str)) > MAX_BUFF) ? MAX_BUFF : l) > 0) {
        c = *(str+l);
        *(str+l) = '\0';
        ads_printf ("%s", str);
        str += l;
        *str = c;
    }
}
/****************************************************************************/
/*.doc print_prompt(internal) */
/*+
   Print prompt string.
-*/
/****************************************************************************/
static void
/*FCN*/print_prompt ()
{
    ads_prompt ("\n");
    ads_printf ("%s", drvname);
    ads_prompt ("\\");
    ads_printf ("%s", basename);
    ads_prompt ("\\");
    ads_printf ("%s", username);
    ads_prompt ("\\");
    ads_printf ("%s", password);
    ads_prompt ("> ");
}
/*EOF*/
