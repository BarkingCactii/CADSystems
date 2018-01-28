/*----------------------------------------------------------------------------
dlgmgr.c

AutoCad Dialog box Manager
by Jeff Hill
----------------------------------------------------------------------------*/

#define DLG_MAIN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "adslib.h"
#include "ads.h"
#include "adsdlg.h"

#include "llistman.h"
#include "alloc.h"

#include "dlgmgr.h"

#define  DIALOG_NOT_LOADED    -1
#define  NUM_DIALOGS    (sizeof(Dialog) / sizeof(_Dlg_event_table))

static int  dcl_id_tab[NUM_DIALOGS];
static _L_HANDLE  list_handle = LIST_FULL;
static int shutting_down = FALSE;
static int module_init_flag = FALSE;

/*----------------------------------------------------------------------------

	Module initialization

----------------------------------------------------------------------------*/

static void init_module()
{
   memset((void *)dcl_id_tab, DIALOG_NOT_LOADED, sizeof(dcl_id_tab));
}

/*----------------------------------------------------------------------------

	Load dialog box dcl definition

----------------------------------------------------------------------------*/

static int dlg_load_dialog(int idx)
{
   _Dlg_event_table  *dlg;

   if (idx < 0 || idx >= NUM_DIALOGS)
      return(DLG_ERR);

   dlg = &Dialog[idx];

   if (dcl_id_tab[idx] == DIALOG_NOT_LOADED)
   {
      if (ads_load_dialog(dlg->dcl_file, &dcl_id_tab[idx]) != RTNORM)
         return(DLG_ERR);
   }

   return(DLG_OK);
}

/*----------------------------------------------------------------------------

	Display all dialog boxes on the stack

----------------------------------------------------------------------------*/

static int dlg_display_stack()
{
   _Dlg_event_table  *dlg;

   if ((dlg = (_Dlg_event_table *)l_last(list_handle)) == (_Dlg_event_table *)NULL)
      return(DLGCANCEL);

   while (dlg)
   {
      if (dlg_load_dialog(dlg->dlg_ref) == DLG_ERR)
         return(DLG_ERR);

      ads_new_positioned_dialog(dlg->dlg_name, dcl_id_tab[dlg->dlg_ref], dlg->callback_func, dlg->x_pos, dlg->y_pos, &dlg->handle); 

      if (dlg->init_func)
         (*dlg->init_func)(dlg->user_data, dlg->dlg_name);
      
      dlg = (_Dlg_event_table *)l_prev(list_handle);
   }

   return(DLGOK);
}

/*----------------------------------------------------------------------------

	Get your data as declared in dlg_open()

----------------------------------------------------------------------------*/

void *dlg_get_user_data()
{
   _Dlg_event_table  *dlg_data;

   dlg_data = (_Dlg_event_table *)l_head(list_handle);
   if (dlg_data)
      return(dlg_data->user_data);

   return(NULL);
}

/*----------------------------------------------------------------------------

	Call to re-display all dialogs that were previously dropped

----------------------------------------------------------------------------*/

int dlg_comeback()
{
   _Dlg_event_table  *dlg;
   int   status;

   shutting_down = FALSE;

   if (dlg_display_stack() == DLGCANCEL)
      return(DLGCANCEL);

   if ((dlg = (_Dlg_event_table *)l_head(list_handle)) == (_Dlg_event_table *)NULL)
      return(DLGCANCEL);

   while (dlg)
   {
      ads_start_dialog(dlg->handle, &status);

      if (status == DLGCLOSEALL)
      {
         ads_term_dialog();         
         l_free(list_handle);
         break;
      }

      if (status == DLGCOMEBACK)
      {
         ads_term_dialog();         
         break;
      }

      dlg = (_Dlg_event_table *)l_head(list_handle);
   }

   return(status);
}

/*----------------------------------------------------------------------------

	Close a dialog box. Called from within a callback function

----------------------------------------------------------------------------*/

int dlg_close(int status)
{
   _Dlg_event_table  *ddata;

   if (status != DLGCOMEBACK)
   {
      if (l_pop(list_handle, (void *)&ddata) != LIST_OK)
         return(DLGCLOSEALL);
   }
   else
   {
      if (!shutting_down)
      {
         shutting_down = TRUE;
         ddata = (_Dlg_event_table *)l_head(list_handle);
      }
      else
         ddata = (_Dlg_event_table *)l_next(list_handle);
   }

   if (ddata)
      ads_done_positioned_dialog(ddata->handle, status, &ddata->x_pos, &ddata->y_pos);

   return(status);
}

/*----------------------------------------------------------------------------

	Open a dialog box

----------------------------------------------------------------------------*/

int dlg_open(int dialog_num, void *data)
{
   int               status;
   _Dlg_event_table  *dlg;   
 
   if (!module_init_flag)
   {
      module_init_flag = TRUE;
      init_module();
   }

   dlg = &Dialog[dialog_num];

   if (dlg_load_dialog(dialog_num) == DLG_ERR)
      return(DLG_ERR);

   ads_new_positioned_dialog(dlg->dlg_name, dcl_id_tab[dialog_num], dlg->callback_func, dlg->x_pos, dlg->y_pos, &dlg->handle);

   if (list_handle == LIST_FULL)
   {
      if ((list_handle = l_gethandle()) == LIST_FULL)
         return(DLG_ERR);
   }

   dlg->dlg_ref = dialog_num;
   dlg->user_data = data;
   if (l_push(list_handle, (void *)dlg) != LIST_OK)
      return(DLG_ERR);

   if (dlg->init_func)
			(*dlg->init_func)(data, dlg->handle);
      
   ads_start_dialog(dlg->handle, &status);

   return(status);
}
