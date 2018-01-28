/*----------------------------------------------------------------------------
alistman.c

AutoCad resbuf * list utility routines
by Jeff Hill
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <adslib.h>
#include <ads.h>
#include <adscodes.h>

#include "alloc.h"
#include "llistman.h"

#define CDR_START	1

/*----------------------------------------------------------------------------

	Returns the resbuf * previous to item in list head

----------------------------------------------------------------------------*/

static struct resbuf *a_prev(struct resbuf *head, struct resbuf *item)
{
   struct   resbuf *prev = (struct resbuf *)NULL;

	while (head && head != item)
	{
		prev = head;
		head = head->rbnext;
	}

   if (head)
      return(prev);
   else
      return((struct resbuf *)NULL);
}

/*----------------------------------------------------------------------------

	Releases memory for a single resbuf *

----------------------------------------------------------------------------*/

static int free_rb(struct resbuf *item)
{
	if (item->restype == RTSTR)
	{
		free_mem(item->resval.rstring);
	}

	free_mem(item);

	return(LIST_OK);
}

/*----------------------------------------------------------------------------

	Returns the last resbuf * in a list

----------------------------------------------------------------------------*/

static struct resbuf *a_lastrb(struct resbuf *head)
{
	while (head->rbnext)
	{
		head = head->rbnext;
	}
	
	return(head);
}

/*----------------------------------------------------------------------------

	Copies a resbuf and returns a pointer to it

----------------------------------------------------------------------------*/

static struct resbuf *a_copy_item(struct resbuf *src)
{
	struct	resbuf	*newrb;

	if (alloc_mem((void *)&newrb, sizeof(struct resbuf)) == MEM_ERR)
		return((struct resbuf *)NULL);

	memcpy((void *)newrb, (void *)src, sizeof(struct resbuf));

	if (newrb->restype == RTSTR)
	{
		if (alloc_mem((void *)&(newrb->resval.rstring), strlen(src->resval.rstring) + 1) == MEM_ERR)
			return((struct resbuf *)NULL);
		strcpy(newrb->resval.rstring, src->resval.rstring);
	}

	newrb->rbnext = (struct resbuf *)NULL;

	return(newrb);
}

/*----------------------------------------------------------------------------

	Returns a pointer to the next item in a list not being a nested list
	The first buffer is assumed to be of RTLB type

----------------------------------------------------------------------------*/

static struct resbuf *a_next_litem(struct resbuf *head)
{
	if (head == (struct resbuf *)NULL)
		return(ads_newrb(RTNIL));

	if (head->rbnext)
		head = head->rbnext;
	else
		return(head);

	while (head->restype != RTLE)
	{
		if (head->restype == RTLB)
			head = a_next_litem(head);

		if (head->rbnext)
			head = head->rbnext;
		else
			return(head);
	}

	return(head->rbnext);
}

/*----------------------------------------------------------------------------

	Returns a pointer to the next item in a list

----------------------------------------------------------------------------*/

static struct resbuf *a_next_item(struct resbuf *head)
{
	if (head->restype == RTLB)
		return(a_next_litem(head));
	else
	{
		if (head->rbnext)
			return(head->rbnext);
		else
			return(ads_newrb(RTNIL));
	}
}



/*----------------------------------------------------------------------------

	Copies a linked list and returns a pointer to the head

----------------------------------------------------------------------------*/

static struct resbuf *a_copylist(struct resbuf *start)
{
	struct	resbuf	*head = NULL;
	struct	resbuf	*dest = NULL;

	while (start)
	{
		if (dest == (struct resbuf *)NULL)
		{
			head = dest = a_copy_item(start);
		}
		else
		{
			dest->rbnext = a_copy_item(start);
			dest = dest->rbnext;
		}
		start = start->rbnext;
	} 

	return(head);
}

/*----------------------------------------------------------------------------

	Copies a a nested linked list and returns a pointer to the head

----------------------------------------------------------------------------*/

static struct resbuf *a_copy_itemlist(struct resbuf *src)
{
	struct	resbuf	*head = NULL;
	struct	resbuf	*result = NULL;
 	struct	resbuf	*listb = NULL;

	/* walk thru list until the last nested RTLB is reached */
	while (src->restype == RTLB)
	{
		listb = src;
		src = src->rbnext;
	}

	/* add RTLB to the list */
	head = result = a_copy_item(listb);

	/* keep adding entries until we find a RTLE */
	while (src && src->restype != RTLE)
	{
		result->rbnext = a_copy_item(src);
		src = src->rbnext;
		result = result->rbnext;
	}

	if (src->restype == RTLE)
		result->rbnext = a_copy_item(src);

	/* if list is empty, then return lisp nil */
	if (head == result)
	{
		ads_relrb(head);
		return(ads_newrb(RTNIL));
	}

	return(head);
}

/*----------------------------------------------------------------------------

	finds the rescode

----------------------------------------------------------------------------*/

struct resbuf *a_find_restype(struct resbuf *head, int code)
{
	struct	resbuf	*start = head;

	while (start)
	{
      if (start->restype == code)
         return(start);

      start = start->rbnext;
	}

   return((struct resbuf *)NULL);
}

/*----------------------------------------------------------------------------

	lisp ads command car

----------------------------------------------------------------------------*/

struct resbuf *a_car(struct resbuf *head)
{
	if (head == (struct resbuf *)NULL)
		return(ads_newrb(RTNIL));
	
	if (head->restype == RTLB)
		return(a_copy_itemlist(head));
	else
		return(a_copy_item(head));
}

/*----------------------------------------------------------------------------

	Returns a pointer to the nth item in list

----------------------------------------------------------------------------*/

struct resbuf *a_find(struct resbuf *head, int nth)
{
	int		count  =  0;
	struct	resbuf	*start = head;

	while (count != nth)
	{
		start = a_next_item(start);

      if (start->restype == RTNIL)
         break;

		count++;
	}

	return(start);
}

/*----------------------------------------------------------------------------

	lisp ads command cdr

----------------------------------------------------------------------------*/

struct resbuf *a_cdr(struct resbuf *head)
{
	if (head == (struct resbuf *)NULL)
		return(ads_newrb(RTNIL));
	
	return(a_copylist(a_find(head, CDR_START)));
}


/*----------------------------------------------------------------------------

	Returns a pointer to the last item in list

----------------------------------------------------------------------------*/

struct resbuf *a_last(struct resbuf *head)
{
	struct	resbuf *item = head;
	struct	resbuf *last = head;

	do
	{
		last = item;
		item = a_next_item(item);
	}
	while (item->restype != RTNIL && item);

	return(last);
}

/*----------------------------------------------------------------------------

	Inserts an element in the front of the linked list to create a new
	linked list

----------------------------------------------------------------------------*/

struct resbuf *a_cons(struct resbuf *elem, struct resbuf *list)
{
	struct 	resbuf	*head;
	struct	resbuf	*tail;
	struct	resbuf	*tmp;

   if (elem == (struct resbuf *)NULL)
      /* just return a copy of list */
      return(a_copylist(list));

   if (list == (struct resbuf *)NULL)
      /* just return a copy of elem */
      return(a_copylist(elem));

	head = a_copylist(elem);
  	tail = a_copylist(list);

  	tmp = a_lastrb(head);
  	tmp->rbnext = tail;

	return(head);
}

/*----------------------------------------------------------------------------

	Appends element(s) to end of list

----------------------------------------------------------------------------*/

struct resbuf *a_append(struct resbuf *elem, struct resbuf *list)
{
	struct 	resbuf	*head;
	struct	resbuf	*tail;
	struct	resbuf	*tmp;

   if (elem == (struct resbuf *)NULL)
      /* just return a copy of list */
      return(a_copylist(list));

   if (list == (struct resbuf *)NULL)
      /* just return a copy of elem */
      return(a_copylist(elem));

	head = a_copylist(list);
	tail = a_copylist(elem);

	tmp = a_lastrb(head);
	tmp->rbnext = tail;

	return(head);
}

/*----------------------------------------------------------------------------

	Swap element(s)

----------------------------------------------------------------------------*/

int a_swap(struct resbuf *head, struct resbuf *item1, struct resbuf *item2)
{
	struct	resbuf	*i1 = (struct resbuf *)NULL;
	struct	resbuf	*i2 = (struct resbuf *)NULL;
	struct	resbuf	*i1n = (struct resbuf *)NULL;
	struct	resbuf	*i2n = (struct resbuf *)NULL;
	struct	resbuf 	*item = head;
	struct	resbuf 	*prev = head;
	struct 	resbuf	tmp;

	/* determine which item is first in list */
	for (item = head; item; item = item->rbnext)
	{
		if (item == item1)
		{
			i1 = item1;
			i2 = item2;
			break;
		}
		if (item == item2)
		{
			i1 = item2;
			i2 = item1;
			break;
		}
	}
	
	/* check to make sure pointers are valid */
   
	for (item = head; item; item = item->rbnext)
	{
		if (item == i1)
			break;
	}
	if (item == (struct resbuf *)NULL)
		return(LIST_ERR);

	for (item = head; item; item = item->rbnext)
	{
		if (item == i2)
			break;
	}

	if (item == (struct resbuf *)NULL)
		return(LIST_ERR);

	i1n = i1->rbnext;
	i2n = i2->rbnext;

	memcpy((void *)&tmp, (void *)i1, sizeof(struct resbuf));
	memcpy((void *)i1, (void *)i2, sizeof(struct resbuf));
	memcpy((void *)i2, (void *)&tmp, sizeof(struct resbuf));

	i1->rbnext = i1n;
	i2->rbnext = i2n;

	return(LIST_OK);
}

/*----------------------------------------------------------------------------

	Deletes an element

----------------------------------------------------------------------------*/

int a_delete(struct resbuf *head, struct resbuf *item)
{
	struct 	resbuf	*prev = head;
	struct	resbuf	*next;

   prev = a_prev(head, item);

   if (prev == (struct resbuf *)NULL && item != head)
		/* rb not in list */
		return(LIST_ERR);

   /* free record */
	next = prev->rbnext->rbnext;
	if (prev->rbnext)
	{
		free_rb(prev->rbnext);
	}

	if (next)
	{
		prev->rbnext = next;
	}
	else
	{
		prev->rbnext = (struct resbuf *)NULL;
	}

	return(LIST_OK);
}

/*----------------------------------------------------------------------------

	Adds element(s) after position addpt

----------------------------------------------------------------------------*/

int a_add(struct resbuf *addpt, struct resbuf *item)
{
	struct resbuf *addptn = addpt->rbnext;
	struct resbuf *prev = (struct resbuf *)NULL; 

	if (addpt == (struct resbuf *)NULL || item == (struct resbuf *)NULL)
	{
		return(LIST_ERR);
	}

	addpt->rbnext = item;

	while (item)
	{
		prev = item;
		item = item->rbnext;
	}

	prev->rbnext = addptn;

	return(LIST_OK);
}

#ifdef DEBUG

disp_resbuf(struct resbuf *head)
{
	while (head)
	{
		if (head->restype == RTLB)
			printf("{ ");
		else
		if (head->restype == RTLE)
			printf("} ");
		else
			printf("%d ", head->restype);
		head = head->rbnext;
	}
	printf("\n");
}

struct resbuf *create_list()
{
	struct resbuf	*head;
	struct resbuf	*tmp;
	int	i;

	tmp = head = ads_newrb(0);
	for (i = 1; i < 10; i++)
	{
		tmp->rbnext = ads_newrb(i);
		tmp = tmp->rbnext;
	}
	return(head);
}

struct resbuf *create_nested_list()
{
	struct resbuf	*head;
	struct resbuf	*tmp;
	int	i;
	int	j;

	tmp = head = ads_newrb(RTLB);
	for (i = 1; i < 10; i++)
	{
		j = i;
		if (i == 6)
			j = RTLB;
		if (i == 3 || i == 9)
			j = RTLE;
		tmp->rbnext = ads_newrb(j);
		tmp = tmp->rbnext;
	}
	return(head);
}

main()
{
	struct resbuf	*head;
	int	i;

	for (i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			head = create_list();
			printf("list\n");
		}
		if (i == 1)
		{
			head = create_nested_list();
			printf("nexted list\n");
		}

		disp_resbuf(head);
		printf("a_car\n");
		disp_resbuf(a_car(head));
		printf("a_cdr\n");
		disp_resbuf(a_cdr(head));
		printf("a_find (2)\n");
		disp_resbuf(a_find(head, 2));
		printf("a_last\n");
		disp_resbuf(a_last(head));
		printf("a_cons (1)\n");
		disp_resbuf(a_cons(a_find(head, 1), head));
		printf("a_append (1)\n");
		disp_resbuf(a_append(a_find(head, 1), head));
		printf("a_swap (1) (2)\n");
		a_swap(head, head->rbnext, head->rbnext->rbnext);
		disp_resbuf(head);
		printf("a_delete (1)\n");
		a_delete(head, head->rbnext);
		disp_resbuf(head);
		printf("a_add (2)\n");
		a_add(head->rbnext->rbnext, a_cdr(head));
		disp_resbuf(head);
	}
}

#endif

