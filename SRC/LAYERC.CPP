/*----------------------------------------------------------------------------
layerc.c

Layer control

by Jeff Hill
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "adslib.h"
#include "ads.h"

#include "llistman.h"

#define  LAYER_ERR         0
#define  LAYER_OK          1

#define  MAX_LINE_SZ       129
#define  LAYER_FILE        "layer.ctl"
#define  LAY_LINE_INPUTS   4

#define  MAX_LAY_NAME      10
#define  MAX_LAY_COLOR     3
#define  MAX_LAY_LINETYPE  15
#define  MAX_ALIAS         15
#define  in_format         "%10s %3s %15s %15s"

#define  MIN_COLOR         1
#define  MAX_COLOR         255

typedef struct
{
   char  name[MAX_LAY_LINETYPE + 1];
   int   type;

} LAYER_LINESTYLE;

typedef struct
{
   char  name[MAX_LAY_COLOR + 1];
   int   num;

} LAYER_COLOR;

typedef struct
{
   char  name[MAX_LAY_NAME + 1];
   char  color[MAX_LAY_COLOR + 1];
   char  linetype[MAX_LAY_LINETYPE + 1];
   char  alias[MAX_ALIAS + 1];
} LAYER_NODE;

typedef struct
{
   char  name[MAX_ALIAS + 1];
   int   x_ref;
} LAYER_ALIASES;

LAYER_ALIASES alias_tble[] = 
{
   "LAYER-7",     0,
   "LAYER-6",     0,
   "LAYER-5",     0,
   "LAYER-4",     0,
   "LAYER-3",     0,
   "LAYER-2",     0,
   "LAYER-1",     0,
};

#define ALIAS_TBLE_SZ   (sizeof(alias_tble) / sizeof(LAYER_ALIASES))

static _L_HANDLE  layer_handle = LIST_FULL;
static int        module_initialized = FALSE;

/*--------------------------------------------------------------------------

   Layer initialization

--------------------------------------------------------------------------*/

static int make_layer(int num)
{
   LAYER_NODE  *node;

   if ((node = l_nth(layer_handle, num)) == (void *)NULL)
      return(LAYER_ERR);

   ads_command(RTSTR, "layer", RTSTR, "m", RTSTR, node->name, RTSTR, "c", RTSTR, node->color, RTSTR, "", 
      RTSTR, "l", RTSTR, node->linetype, RTSTR, "", RTSTR, "", RTNONE);

   return(LAYER_OK);
}


static int open_layer_file(FILE **fp)
{
   if ((*fp = fopen(LAYER_FILE, "r")) == (FILE *)NULL)
      return(LAYER_ERR);

   return(LAYER_OK);
}

static int close_layer_file(FILE *fp)
{
   if (fp)
      fclose(fp);

   return(LAYER_OK);
}

static int validate_layer_name(char *name)
{
   return(LAYER_OK);
}

static int validate_layer_color(char *color)
{
   int   color_val;

   color_val = atoi(color);
   if (color_val < MIN_COLOR || color_val > MAX_COLOR)
   {
      ads_printf("Invalid color %d\n", color_val);
      return(LAYER_ERR);
   }
   return(LAYER_OK);
}

static int validate_layer_linetype(char *linetype)
{
   struct resbuf  *rb;

   if ((rb = ads_tblsearch("LTYPE", linetype, FALSE)) == (struct resbuf *)NULL)
   {
      ads_printf("Invalid linetype %s\n", linetype);
      return(LAYER_ERR);
   }

   ads_relrb(rb);
   return(LAYER_OK);
}

static int validate_layer_alias(char *alias, int idx)
{
   int   i;

   for (i = 0; i < ALIAS_TBLE_SZ; i++)
   {
      if (strcmp(alias_tble[i].name, alias) == (int)NULL)
      {
         alias_tble[i].x_ref = idx;
         return(LAYER_OK);
      }
   }

   ads_printf("Invalid alias %s\n", alias);
   return(LAYER_ERR);
}

static int read_layer_values(char *line, int idx)
{
   char        layer_name[MAX_LAY_NAME + 1];
   char        layer_color[MAX_LAY_COLOR + 1];
   char        layer_linetype[MAX_LAY_LINETYPE + 1];
   char        layer_alias[MAX_ALIAS + 1];
   LAYER_NODE  *node;

   if (sscanf(line, in_format, layer_name, layer_color, layer_linetype, layer_alias) != LAY_LINE_INPUTS)
      return(LAYER_ERR);

   if (!validate_layer_name(layer_name))
      return(LAYER_ERR);

   if (!validate_layer_color(layer_color))
      return(LAYER_ERR);

   if (!validate_layer_linetype(layer_linetype))
      return(LAYER_ERR);

   if (!validate_layer_alias(layer_alias, idx))
      return(LAYER_ERR);

   if (l_add(layer_handle, (void *)&node, sizeof(LAYER_NODE)) == LIST_ERR)
      return(LAYER_ERR);

   strcpy(node->name, layer_name);
   strcpy(node->color, layer_color);
   strcpy(node->linetype, layer_linetype);
   strcpy(node->alias, layer_alias);

   return(LAYER_OK);
}

static int read_layer_line(FILE *fp, char *line)
{
   if (fgets(line, MAX_LINE_SZ, fp) == NULL)
      return(LAYER_ERR);

   return(LAYER_OK);
}

static void strtoup(char *line)
{
   char  *chptr = line;

   while (*chptr)
   {
      *chptr = toupper(*chptr);
      chptr++;
   }
}

static int add_layer_line(char *line, int idx)
{
   /* convert to uppercase */
   strtoup(line);

   read_layer_values(line, idx);
   
   return(LAYER_OK);
}

static int load_layer_file(FILE *fp)
{
   char  line[MAX_LINE_SZ + 1];
   int   entry_num = 0;       /* entry number in linked list */

   while (read_layer_line(fp, line))
   {
      add_layer_line(line, entry_num);
      entry_num++;
   }

   return(LAYER_OK);
}

static void create_layers()
{
   int   i;

   for (i = 0; i < ALIAS_TBLE_SZ; i++)
   {
      make_layer(alias_tble[i].x_ref);
   }
}

static int init_layer_module()
{
   FILE  *fp;

   if ((layer_handle = l_gethandle()) == LIST_FULL)
      return(LAYER_ERR);

   if (open_layer_file(&fp) == LAYER_ERR)
      return(LAYER_ERR);

   load_layer_file(fp);

   close_layer_file(fp);

   create_layers();

   module_initialized = TRUE;

   return(LAYER_OK);
}

/*--------------------------------------------------------------------------

   Set Layers

--------------------------------------------------------------------------*/

static int set_layer(int num)
{
   LAYER_NODE  *node;

   if ((node = l_nth(layer_handle, num)) == (void *)NULL)
      return(LAYER_ERR);

   ads_command(RTSTR, "layer", RTSTR, "s", RTSTR, node->name, RTSTR, "", RTNONE);
   return(LAYER_OK);
}

static void switch_to_layer(int num)
{
   if (num < 0 || num >= ALIAS_TBLE_SZ)
   {
      ads_printf("Invalid layer [%d]\n", num);
      return;
   }

   set_layer(alias_tble[num].x_ref);
}

/*--------------------------------------------------------------------------

   User interface function

--------------------------------------------------------------------------*/

int layer_control(int layer_num)
{
   if (!module_initialized)
   {
      if (!init_layer_module())
      {
         if (layer_handle != LIST_FULL)
            l_free(layer_handle);
         ads_printf("Layer control module initialization failed\n");
         return(LAYER_ERR);
      }
   }

   switch_to_layer(layer_num);
   
   return(LAYER_OK);
}
