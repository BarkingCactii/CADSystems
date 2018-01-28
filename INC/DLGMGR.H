#ifndef _DLGMGR_H
#define _DLGMGR_H

#include "llistman.h"

/* -------------------------------------------------------------------------- 
   Insert defininations of initialization and callback functions here
*/

#include "main.h"

/* ----------------------------------------------------------------------- */
#define DLG_ERR         0
#define DLG_OK          1
#define DLGCENTRE       -1

/* -------------------------------------------------------------------------- 
   Dialog initialization and callback function pointers. The position
   of these pointers coincides with the value of its dialog reference number
*/

typedef struct
{
   char        *dcl_file;
   char        *dlg_name;
   void        (*init_func)(void *, ads_hdlg);
   void  CALLB (*callback_func)(ads_callback_packet *);
   int         x_pos;
   int         y_pos;
   ads_hdlg    handle;
   void        *user_data;
   int         dlg_ref;
} _Dlg_event_table;


/* --------------------------------------------------------------------------
   Additional ads_done_dialog() or ads_done_positioned_dialog()
   return codes used by the dialog manager
*/

#define DLGERROR        (DLGSTATUS + 1)
#define DLGCLOSEALL     (DLGSTATUS + 2)
#define DLGCOMEBACK     (DLGSTATUS + 3)

/* --------------------------------------------------------------------------
   Dialog reference number. Starting from 0, each dialog box is given a
   unique number. 
*/

#define  DLG_DIALOG_1      0

/* --------------------------------------------------------------------------
   Contains information about each dialog box.
*/

#ifdef DLG_MAIN

static _Dlg_event_table  Dialog[] = 
{
   "1.dcl", "one", init_1, cb_1, DLGCENTRE, DLGCENTRE, 0, 0, 0,
   "2.dcl", "two", init_2, cb_2, DLGCENTRE, DLGCENTRE, 0, 0, 1,
   "3.dcl", "two", init_2, cb_3, DLGCENTRE, DLGCENTRE, 0, 0, 2,
   "4.dcl", "two", init_2, cb_4, DLGCENTRE, DLGCENTRE, 0, 0, 3,
   "5.dcl", "two", init_2, cb_5, DLGCENTRE, DLGCENTRE, 0, 0, 4,
};

#else

extern _Dlg_event_table Dialog[];

#endif

/* --------------------------------------------------------------------------
   User rountines
*/

int   dlg_close(int);
int   dlg_open(int, void *);
void *dlg_getptr();
void *dlg_get_user_data();
int   dlg_comeback();

#endif

