/*----------------------------------------------------------------------------
dmain.c

Main function for dimensioning

by Jeff Hill
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "adslib.h"
#include "ads.h"

#include "geom.h"
#include "dmain.h"
#include "dsort.h"
#include "dmkline.h"
#include "dinterp.h"
#include "alloc.h"
#include "env.h"

#define DIM_LAYER    "D"         /* layer name for dimension lines */
#define EXT_LAYER    "E"         /* layer name for extension lines */
#define TEXT_LAYER   "T"         /* layer name for dimension text */

#define DIMASZ       "DIMASZ" 
#define DIMCLRD      "DIMCLRD"
#define DIMCLRE      "DIMCLRE"
#define DIMCLRT      "DIMCLRT"
#define DIMEXO       "DIMEXO" 
#define DIMTSZ       "DIMTSZ" 
#define DIMTVP       "DIMTVP" 
#define DIMTXT       "DIMTXT" 
#define DIMSCALE     "DIMSCALE" 
#define DIMBLK       "DIMBLK" 
#define DIMEXE       "DIMEXE"

static int process_ref(_DIM *dim)
{
   _D_ITEM     *dim_item;

   if ((dim_item = (_D_ITEM *)l_head(dim->item_ll)) == (_D_ITEM *)NULL)
      return(DIM_ERR);

   memcpy((void *)dim->r.start, (void *)dim->dim_loc, sizeof(ads_point));
   memcpy((void *)dim->r.end, (void *)dim_item->e_base_pt, sizeof(ads_point));
   dim->r.angle = angle(dim->r.end, dim->r.start);

   return(DIM_OK);
}

static void set_layers(_DIM *dim)
{
   char  color_num[MAX_COLOR_SZ];

   strcpy(dim->l.dlayer, DIM_LAYER);   
   strcpy(dim->l.elayer, EXT_LAYER);   
   strcpy(dim->l.tlayer, TEXT_LAYER);   

   if (!dim->v._dimclrd)
      dim->v._dimclrd = 1;
   sprintf(color_num, "%hd", dim->v._dimclrd);
   ads_command(RTSTR, "layer", RTSTR, "m", RTSTR, dim->l.dlayer, RTSTR, "c", RTSTR, color_num, RTSTR, "", RTSTR, "", RTNONE); 

   if (!dim->v._dimclre)
      dim->v._dimclre = 1;
   sprintf(color_num, "%hd", dim->v._dimclre);
   ads_command(RTSTR, "layer", RTSTR, "m", RTSTR, dim->l.elayer, RTSTR, "c", RTSTR, color_num, RTSTR, "", RTSTR, "", RTNONE); 

   if (!dim->v._dimclrt)
      dim->v._dimclrt = 1;
   sprintf(color_num, "%hd", dim->v._dimclrt);
   ads_command(RTSTR, "layer", RTSTR, "m", RTSTR, dim->l.tlayer, RTSTR, "c", RTSTR, color_num, RTSTR, "", RTSTR, "", RTNONE); 
}

static void quick_and_dirty(_DIM *dim)
{
/*   dim->a.rot_lower_limit = _90_DEG;
   dim->a.rot_upper_limit = _270_DEG;
*/
   dim->a.rot_lower_limit = 105.0;
   dim->a.rot_upper_limit = 285.0;

   dim->a.flip_lower_limit = 105.0;
   dim->a.flip_upper_limit = 285.0;

   dim->last_line = FALSE;

   ads_command(RTSTR, "style", RTSTR, "dim", RTSTR, "", RTREAL, 0.0, RTSTR, "",
      RTSTR, "",RTSTR, "",RTSTR, "",RTSTR, "", RTNONE);
}

static int get_acad_dimvars(_DIM *dim)
{
   struct resbuf  rb;
   _D_ACADVARS    *v;

   v = &dim->v;

   /* DIMASZ   dimension arrow size - implemented */
   if (ads_getvar(DIMASZ, &rb) != RTNORM)
      return(DIM_ERR);
   v->_dimasz = rb.resval.rreal;

   /* DIMCLRD  dimension line colour - implemented */
   if (ads_getvar(DIMCLRD, &rb) != RTNORM)
      return(DIM_ERR);
   v->_dimclrd = rb.resval.rint;

   /* DIMCLRE  extension line colour - implemented */
   if (ads_getvar(DIMCLRE, &rb) != RTNORM)
      return(DIM_ERR);
   v->_dimclre = rb.resval.rint;

   /* DIMCLRT  dimension text colour - implemented */
   if (ads_getvar(DIMCLRT, &rb) != RTNORM)
      return(DIM_ERR);
   v->_dimclrt = rb.resval.rint;

   /* DIMEXO   extension line offset  - implemented */
   if (ads_getvar(DIMEXO, &rb) != RTNORM)
      return(DIM_ERR);
   v->_dimexo = rb.resval.rreal;

   /* DIMTSZ   tick size - implemented */
   if (ads_getvar(DIMTSZ, &rb) != RTNORM)
      return(DIM_ERR);
   v->_dimtsz = rb.resval.rreal;

   /* DIMTVP   text vertical position - implemented */
   if (ads_getvar(DIMTVP, &rb) != RTNORM)
      return(DIM_ERR);
   v->_dimtvp = rb.resval.rreal;

   /* DIMTXT   text size - implemented */
   if (ads_getvar(DIMTXT, &rb) != RTNORM)
      return(DIM_ERR);
   v->_dimtxt = rb.resval.rreal;

   /* DIMSCALE   dimension scaling - implemented */
   if (ads_getvar(DIMSCALE, &rb) != RTNORM)
      return(DIM_ERR);
   v->_dimscale = rb.resval.rreal;
   if (v->_dimscale == 0.0)
      v->_dimscale = 1.0;

   /* DIMBLK   dimension mark block name - implemented */
   if (ads_getvar(DIMBLK, &rb) != RTNORM)
      return(DIM_ERR);
   strcpy(dim->dim_marks, rb.resval.rstring);
   if (rb.resval.rstring)
      free_mem(rb.resval.rstring);

   /* DIMEXE   extension line extension - implemented */
   if (ads_getvar(DIMEXE, &rb) != RTNORM)
      return(DIM_ERR);
   v->_dimexe = rb.resval.rreal;

   /* set up marks */
   if (v->_dimtsz)
      dim->mark_type = TICK_T;
   else
      dim->mark_type = ARROW_T;

   return(DIM_OK);
}

static void init_dim(_DIM *dim)
{
   quick_and_dirty(dim);

   get_acad_dimvars(dim);

   set_layers(dim);
}

int dimension(_DIM *dim)
{
   /* set up static values such as line colours layers */
   init_dim(dim);

   /* do some pre-processing on the reference line */
   if (process_ref(dim) == DIM_ERR)
      return(DIM_ERR);

   /* sort the list */
   if (sort_dimensions(dim) == DIM_ERR)
      return(DIM_ERR);

   /* build a list of lines to be drawn */

   if (build_dim_lines(dim) == DIM_ERR)
      return(DIM_ERR);

   dim_interpret(dim);

   l_free(dim->draw_ll);

   return(DIM_OK);
}
