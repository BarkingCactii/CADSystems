static int recalculate_block(_DIM *dim, _D_text *t, int last_block)
{
   _D_DRAW  *d;
   _D_block *b;
   int      delete_flag = FALSE;

   if (dim->mark_type == TICK_T)
      return(DIM_OK);

   if ((d = (_D_DRAW *)l_head(dim->draw_ll)) == (void *)NULL)
      return(DIM_ERR);

   while (d)
   {
      if (d->type == BLOCK)
      {
         b = &d->d.b;
         if (b->tag == t->tag)
         {
            if (!(b->tag))
            {
               /* first or last block, so just rotate */
               b->angle += _180_DEG;
            }
            else
            if (last_block)
            {
               b->angle += _180_DEG;
            }
            else
            {
               if (!delete_flag)
               {
                  l_delete(dim->draw_ll, (void *)d);
                  d = (_D_DRAW *)l_current(dim->draw_ll);
                  delete_flag = TRUE;
                  continue;
               }
               else
               {
                  b->angle += _180_DEG;
                  b->name = dim->alt_dim_marks;
               }
            }
         }
      }
      d = (_D_DRAW *)l_next(dim->draw_ll);
   }

   return(DIM_OK);
}


static int calc_text_pos(_DIM *dim, int start, int curr, int total)
{
   _D_DRAW  *draw;
   _D_text  *t;
   _D_text  *prev = (_D_text *)NULL;
   _D_text  *pprev = (_D_text *)NULL;
   ads_real new_offset;

   if ((draw = (_D_DRAW *)l_nth(dim->draw_ll, curr)) == (void *)NULL)
      return(DIM_ERR);

   t = &draw->d.t;
   if (curr > start)
   {
      draw = (_D_DRAW *)l_nth(dim->draw_ll, curr - 1);
      prev = &draw->d.t;
   }

   memcpy((void *)t->n_insert_pt, (void *)t->insert_pt, sizeof(ads_point));

   if (t->avail_dist >= 0.0)
      return(DIM_OK);

   /* available distance is negative, therefore the text is larger than
      the dimension it refers to */

   new_offset = t->width + (t->avail_dist / 2);

   if (curr == total - 1 && prev->placement == CENTRE_PLACE)
   {
      ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle)), new_offset, t->n_insert_pt);
      t->placement = RIGHT_PLACE;
      return(DIM_OK);
   }

   if (curr > start)
   {  
      if (prev->placement == CENTRE_PLACE)
      {
         t->placement = LEFT_PLACE;
         t->avail_dist = t->width + t->avail_dist;
         ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle - _180_DEG)), new_offset, t->n_insert_pt);
      }
      if (prev->placement == LEFT_PLACE)
      {
         t->placement = RIGHT_PLACE;
         t->avail_dist = t->width + t->avail_dist;
         ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle)), new_offset, t->n_insert_pt);
      }
      if (prev->placement == RIGHT_PLACE)
      {
         if (curr - start < 2)
            return(DIM_OK);

         if ((draw = (_D_DRAW *)l_nth(dim->draw_ll, curr - 2)) == (void *)NULL)
            return(DIM_ERR);
         pprev = &draw->d.t;

         t->placement = RIGHT_PLACE;
         ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle)), new_offset, t->n_insert_pt);

         /* adjust previous right point */
         prev->placement = CENTRE_PLACE;
         memcpy((void *)prev->n_insert_pt, (void *)prev->insert_pt, sizeof(ads_point));
/*         ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle - _180_DEG)), 
            distance(pprev->n_insert_pt, t->n_insert_pt) / 2, prev->n_insert_pt);*/
      }
   }
   else
   {
      t->placement = LEFT_PLACE;
      t->avail_dist = t->width + t->avail_dist;
      ads_polar(t->insert_pt, degtorad(adjust_angle(t->angle - _180_DEG)), new_offset, t->n_insert_pt);
   }

   return(DIM_OK);
}

