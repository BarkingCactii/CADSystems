/*----------------------------------------------------------------------------
llistman.c

Generic linked list manager
by Jeff Hill
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "llistman.h"
#include "alloc.h"

#ifdef DEBUG_ADS
#include "adslib.h"
#include "ads.h"
#endif

struct Node
{
	struct   Node		*next;
   struct   Node		*prev;
	void		*l;
};

typedef struct Node _Node;

static _Node      *head[MAX_LISTS]; /* points to the head of each list */
static _Node      *z[MAX_LISTS];    /* points to the last node of each list */
static _Node	   *pos[MAX_LISTS];  /* points to the current position in list */
static int        module_init_flag = FALSE;

static _L_HANDLE     handlist[MAX_LISTS];
#define L_AVAIL      1
#define L_TAKEN      0

#define _END_P(x)    (z[x]->prev)
#define _END(x)      (z[x])
#define _HEAD_N(x)   (head[x]->next)
#define _HEAD(x)     (head[x])
#define _CURR(x)     (pos[x])

#define  L_GETHANDLE    0
#define  L_HEAD         1
#define  L_LAST         2

static char *_Diag_Message[] = 
{
   "l_gethandle",
   "l_head",
   "l_last",
};

static void *list_critical(int function, _L_HANDLE handle)
{
#ifdef DEBUG_ADS
   ads_printf("CRITICAL LIST ERROR [%s] on handle [%d]\n", _Diag_Message[function], (int)handle);
#else
   printf("CRITICAL LIST ERROR [%s] on handle [%d]\n", _Diag_Message[function], (int)handle);
#endif
   return((void *)NULL);
}

static int empty_list(_L_HANDLE hand)
{
   if (_HEAD_N(hand) == _END(hand))
      return(TRUE);
   else
      return(FALSE);
}

static _Node *l_nodebase(_L_HANDLE hand, void *list_elem)
{
	_Node		*cn = _HEAD_N(hand);

   if (empty_list(hand))
		return((_Node *)NULL);

	while (cn->l != list_elem && cn->next != _END(hand))
		cn = cn->next;

   if (cn->l == list_elem)
   	return(cn);
   else
		return((_Node *)NULL);
}

int l_addnode(void *list_elem, _Node *prev, _Node *next)
{
	_Node		*newn = (_Node *)NULL;

	if (alloc_mem((void *)&newn, sizeof(_Node)) == MEM_ERR)
		return(LIST_ERR);

	newn->next = (void *)next;
	newn->prev = (void *)prev;
	newn->l = list_elem;

	prev->next = next->prev = (void *)newn;		

	return(LIST_OK);
}

int l_deletenode(_L_HANDLE hand, _Node *del_node)
{
   del_node->next->prev = del_node->prev;
	del_node->prev->next = del_node->next;

	/* adjust current node position */
	if (del_node == _CURR(hand))
   {
      if (_CURR(hand) == _END_P(hand))
         _CURR(hand) = _CURR(hand)->prev;
      else
         _CURR(hand) = _CURR(hand)->next;
   }

	free_mem((void *)del_node);

	return(LIST_OK);
}

static int create_head(int list_num)
{
   /* create the head node */
	if (alloc_mem((void *)&head[list_num], sizeof(_Node)) == MEM_ERR)
		return(LIST_ERR);

   /* create the last node */
	if (alloc_mem((void *)&z[list_num], sizeof(_Node)) == MEM_ERR)
		return(LIST_ERR);

   /* set up pointers */
   head[list_num]->prev = (void *)NULL;
   head[list_num]->next = (void *)z[list_num];
   head[list_num]->l = (void *)NULL;

   z[list_num]->prev = head[list_num];
   z[list_num]->next = (void *)z[list_num];
   head[list_num]->l = (void *)NULL;

	return(LIST_OK);
}

/*----------------------------------------------------------------------------

	Init the list handling module

----------------------------------------------------------------------------*/

static int l_init()
{
   int   i;

	memset((void *)head, 0, sizeof(head));
	memset((void *)z, 0, sizeof(z));
	memset((void *)pos, 0, sizeof(pos));

   for (i = 0; i < MAX_LISTS; i++)
   {
      handlist[i] = L_AVAIL;
      if (create_head(i) == LIST_ERR)
	      return(LIST_ERR);
   }
	return(LIST_OK);
}


/*----------------------------------------------------------------------------

	Retrieves a handle for the user

----------------------------------------------------------------------------*/

_L_HANDLE l_gethandle()
{
   int   i;

   if (module_init_flag == FALSE)
   {
      if (l_init() == LIST_ERR)
         return(LIST_FULL);

      module_init_flag = TRUE;
   }

   for (i = 0; i < MAX_LISTS; i++)
   {
      if (handlist[i] == L_AVAIL)
      {
/*         ads_printf("handle [%d] allocated\n", i);*/
         handlist[i] = L_TAKEN;
         return(i);
      }
   }

   list_critical(L_GETHANDLE, (_L_HANDLE)-1);
   return(LIST_FULL);
}

/*----------------------------------------------------------------------------

	Add an item to the list

----------------------------------------------------------------------------*/


int l_add(_L_HANDLE hand, void **list_elem, int l_sz)
{
	if (alloc_mem(list_elem, l_sz) == MEM_ERR)
		return(LIST_ERR);

	return(l_addnode(*list_elem, _END_P(hand), _END(hand)));
}


/*----------------------------------------------------------------------------

	Insert an item at a specified point

----------------------------------------------------------------------------*/

int l_insert(_L_HANDLE hand, void **list_elem, int l_sz, void *insert_pt)
{
   _Node  *curr;

	if (alloc_mem(list_elem, l_sz) == MEM_ERR)
		return(LIST_ERR);

	curr = l_nodebase(hand, insert_pt);
	return(l_addnode(*list_elem, curr, curr->next));
}


/*----------------------------------------------------------------------------

	Delete an item from the list

----------------------------------------------------------------------------*/

int l_delete(_L_HANDLE hand, void *list_elem)
{
	_Node		*curr;
	
   if ((curr = l_nodebase(hand, list_elem)) == (_Node *)NULL)
      return(LIST_ERR);

	free_mem(list_elem);

	return(l_deletenode(hand, curr));
}


/*----------------------------------------------------------------------------

	Returns a pointer to first item in list

----------------------------------------------------------------------------*/

void *l_head(_L_HANDLE hand)
{
   if (empty_list(hand))
      return(list_critical(L_HEAD, hand));

	_CURR(hand) = _HEAD_N(hand);
	return(_CURR(hand)->l);
}


/*----------------------------------------------------------------------------

	Returns a pointer to last item in list

----------------------------------------------------------------------------*/

void *l_last(_L_HANDLE hand)
{
   if (empty_list(hand))
      return(list_critical(L_LAST, hand));

   _CURR(hand) = _END_P(hand);
	return(_CURR(hand)->l);
}


/*----------------------------------------------------------------------------

	Returns a pointer to the next item in list

----------------------------------------------------------------------------*/

void *l_next(_L_HANDLE hand)
{
   if (empty_list(hand) || _CURR(hand) == _END_P(hand))
      return((void *)NULL);

   _CURR(hand) = _CURR(hand)->next;
	return(_CURR(hand)->l);
}


/*----------------------------------------------------------------------------

	Returns a pointer to the last item in list

----------------------------------------------------------------------------*/

void *l_prev(_L_HANDLE hand)
{
   if (empty_list(hand) || _CURR(hand) == _HEAD_N(hand))
      return((void *)NULL);

   _CURR(hand) = _CURR(hand)->prev;
	return(_CURR(hand)->l);
}

/*----------------------------------------------------------------------------

	Returns a pointer to the current item in list

----------------------------------------------------------------------------*/

void *l_current(_L_HANDLE hand)
{
   if (empty_list(hand) || _CURR(hand) == _HEAD_N(hand))
      return((void *)NULL);

	return(_CURR(hand)->l);
}

/*----------------------------------------------------------------------------

	Makes a designated pointer the current item

----------------------------------------------------------------------------*/

void l_make_current(_L_HANDLE hand, void *list_elem)
{
   _CURR(hand) = l_nodebase(hand, list_elem);
}

/*----------------------------------------------------------------------------

	Returns a pointer to nth item in list

----------------------------------------------------------------------------*/

void *l_nth(_L_HANDLE hand, int nth)
{
   int   i;
   void  *data;

   if (empty_list(hand))
      return((void *)NULL);

   data = l_head(hand);

   for (i = 0; i < nth; i++)
      if ((data = l_next(hand)) == (void *)NULL)
         return((void *)NULL);

	return(data);
}


/*----------------------------------------------------------------------------

	Swaps 2 elements in the linked list

----------------------------------------------------------------------------*/

int l_swap(_L_HANDLE hand, int elem1, int elem2)
{
   void  *node1;
   void  *node2;
   _Node *base1;
   _Node *base2;

   if ((node1 = l_nth(hand, elem1)) == (void *)NULL)
      return(LIST_ERR);

   if ((node2 = l_nth(hand, elem2)) == (void *)NULL)
      return(LIST_ERR);

   if ((base1 = l_nodebase(hand, node1)) == (_Node *)NULL)
      return(LIST_ERR);

   if ((base2 = l_nodebase(hand, node2)) == (_Node *)NULL)
      return(LIST_ERR);

   /* swap data pointer rather than next and previous */   
   base1->l = node2;
   base2->l = node1;

   return(LIST_OK);
}


/*----------------------------------------------------------------------------

	De-allocate the linked list

----------------------------------------------------------------------------*/

void l_free(_L_HANDLE hand)
{
	_Node		*cn = _HEAD_N(hand);
   _Node    *n;

   if (hand >= 0 && hand < MAX_LISTS)
      handlist[hand] = L_AVAIL;

   if (empty_list(hand))
      return;

   while (cn != cn->next)
   {
      n = cn->next;
      
		free_mem(cn->l);
		free_mem(cn);

      cn = n;
   }

   /* tidy up pointers */
   _HEAD_N(hand) = _END(hand);
   _END_P(hand) = _HEAD(hand);
}


/*----------------------------------------------------------------------------

	Number of user elements in list

----------------------------------------------------------------------------*/

int l_count(_L_HANDLE hand)
{
	_Node		*cn = _HEAD_N(hand);
   int      num_nodes = 0;

   while (cn != cn->next)
   {
      num_nodes++;
      cn = cn->next;
   }

   return(num_nodes);
}


/*----------------------------------------------------------------------------

	Push an item on the stack

----------------------------------------------------------------------------*/

int l_push(_L_HANDLE hand, void *item)
{
	if (l_addnode(item, _HEAD(hand), _HEAD_N(hand)) == LIST_ERR)
      return(LIST_ERR);

   return(LIST_OK);
}


/*----------------------------------------------------------------------------

	Pop an item off the stack

----------------------------------------------------------------------------*/

int l_pop(_L_HANDLE hand, void **item)
{
   if ((*item = l_head(hand)) == (void *)NULL)
      return(LIST_EMPTY);
	
	if (l_deletenode(hand, _HEAD_N(hand)) == LIST_ERR)
      return(LIST_ERR);

   return(LIST_OK);
}


/*----------------------------------------------------------------------------

	Is list empty?

----------------------------------------------------------------------------*/

int l_empty(_L_HANDLE hand)
{
   return(empty_list(hand));
}

/*----------------------------------------------------------------------------

	List walk - for debugging only

----------------------------------------------------------------------------*/

int l_walk(_L_HANDLE hand)
{
   _Node *node = _HEAD(hand);

   while (node != node->next)
   {
#ifdef DEBUG_ADS
      ads_printf("Node [%p] user data [%p] prev [%p] next [%p]\n", (void *)node, node->l, (void *)node->prev, (void *)node->next);
#else
      printf("Node [%p] user data [%p] prev [%p] next [%p]\n", (void *)node, node->l, (void *)node->prev, (void *)node->next);
#endif
      node = node->next;
   }

   return(LIST_OK);
}

/*----------------------------------------------------------------------------

	Lists taken - for debugging only

----------------------------------------------------------------------------*/

int l_taken()
{
   int   i;

   for (i = 0; i < MAX_LISTS; i++)
   {
      if (handlist[i] != L_AVAIL)
      {
#ifdef DEBUG_ADS
         ads_printf("handle [%d] in use\n", i);
#else
         printf("handle [%d] in use\n", i);
#endif
      }
   }

   return(LIST_OK);
}

#ifdef DEBUG

void disp_list(_L_HANDLE hand)
{
   _Node  *curr = _HEAD(hand);
   int   i = 0;
   int   *num;

   do
   {
      if (curr == _HEAD(hand))
         printf("[head] ");
      else
      {
         num = (int *)(curr->l);
         printf("%d ", *num);
      }
      curr = curr->next;
   }
	while (curr != _END(hand));

	printf("[end]\n");
}

main()
{
	int	*list[12];
	int	*ptr;
	int	i;
   int  *stack[5];
   _L_HANDLE   hand;
   _L_HANDLE   hand2;

   printf("l_gethandle()\n");
   if ((hand = l_gethandle()) == LIST_FULL)
   {
      printf("FAILED\n");
      exit(0);
   }

   disp_list(hand);

   printf("l_add()\n");
	for (i = 0; i < 10; i++)
	{
		l_add(hand, (void *)&list[i], sizeof(int));
		*(list[i]) = i;
	}
   disp_list(hand);

	ptr = (int *)l_head(hand);
	printf("l_head() %d\n", *ptr);

	printf("l_next() ");
	while (ptr)
	{
		ptr = (int *)l_next(hand);
      if (ptr)
   		printf("%d ", *ptr);
	}
   printf("\n");

	ptr = (int *)l_last(hand);
	printf("l_last() %d\n", *ptr);

   printf("l_add()\n");
	l_add(hand, (void *)&list[10], sizeof(int));
	*(list[10]) = 10;
   disp_list(hand);

	ptr = (int *)l_last(hand);
	printf("l_last() %d\n", *ptr);

	printf("l_prev() ");
	while (ptr)
	{
		ptr = (int *)l_prev(hand);
      if (ptr)
   		printf("%d ", *ptr);
	}
   printf("\n");

   printf("l_next() ");
	ptr = (int *)l_next(hand);
	printf("%d ", *ptr);
	ptr = (int *)l_next(hand);
	printf("%d ", *ptr);
	ptr = (int *)l_next(hand);
	printf("%d ", *ptr);
   printf("\n");

   printf("l_delete() %d\n", *ptr);
	l_delete(hand, ptr);
   disp_list(hand);

   printf("l_prev() ");
	ptr = (int *)l_prev(hand);
	printf("%d ", *ptr);
	ptr = (int *)l_prev(hand);
	printf("%d ", *ptr);
   printf("\n");

	printf("l_insert() -> node 11 at curr pos\n");
	l_insert(hand, (void **)&list[11], sizeof(int), ptr);
	*list[11] = 11;
   disp_list(hand);

	printf("l_delete() -> head\n");
	l_delete(hand, l_head(hand));
   disp_list(hand);

	printf("l_delete() -> last\n");
	l_delete(hand, l_last(hand));
   disp_list(hand);

/*	l_free(hand);
   printf("l_free()\n");
   disp_list(hand);
*/
   printf("\n[stack]\n");

   hand2 = l_gethandle();
   printf("l_push() ");
   for (i = 0; i < 4; i++)
   {
   	if (alloc_mem((void *)&stack[i], sizeof(int)) == MEM_ERR)
	   	return(LIST_ERR);

      *(stack[i]) = i;

      if (l_push(hand2, (void *)stack[i]) == LIST_ERR)
         return(LIST_ERR);

      printf("%d ", *(stack[i]));
   }
   disp_list(hand2);

   printf("l_pop() ");

   while (l_pop(hand2, (void *)&ptr) != LIST_EMPTY)
   {
      printf("%d ", *ptr);
      free_mem((void *)ptr);
   }

   disp_list(hand2);
   disp_list(hand);
	l_free(hand2);
	l_free(hand);
   disp_list(hand2);
   disp_list(hand);
}

#endif

