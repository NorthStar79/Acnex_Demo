// ============================================================================
// This is the Doubly-Linked List structure for A* pathfinder
// ============================================================================

#ifndef ASLINKEDLIST_H
#define ASLINKEDLIST_H

// This is the Doubly-linked list node structure.
typedef struct asNode{ 
  var f, g, h;
  VECTOR pos;
  unsigned int ID;
  
  struct asNode *creator;
  struct asNode *next;		// next node 
  struct asNode *prev;		// previous node
 
} asNode;  

// This is the Doubly-linked list container structure.
typedef struct asList{
	asNode* head;
	asNode* tail;
	int	count;
	
	asNode* itr;	// iterator
} asList;

function asZeroNode(asNode* node)
{
	node->f = 0;	// the sum of g and h
	node->g = 0;	// the actual shortest distance traveled from initial node to current node
	node->h = 0;	// the estimated (or "heuristic") distance from current node to goal
	node->ID = 0;
	
	node->creator = NULL;
	node->next = NULL;
	node->prev = NULL;
}

function asZeroList(asList* list)
{
	list->head = NULL;
	list->tail = NULL;
	list->count = 0;
	list->itr = NULL;
}

function asClearList(asList* list)
{
	// temporary node pointers.
	asNode* itr = list->head;
	asNode* next;
	
	while(itr != NULL)
	{
		// save the pointer to the next node.
		next = itr->next;
		// delete the current node.
		free(itr); 	
		// make the next node the current node.
		itr = next;
	}
	asZeroList(list);
}

// Addss a new node to the beginning of a list
function asPrepend(asList* list, asNode* node)
{		
	// if there is no head node (ie: list is empty)
	if(list->head == NULL)
   {
   	// create a new head node.
   	asNode* newnode = malloc(sizeof(asNode));
   	memcpy(newnode,node,sizeof(asNode));  
      list->head = list->tail = newnode;
      list->head->next = NULL;
      list->head->prev = NULL;
   }
   else
   {
   	// insert a new node before the head, and reset the head.
   	asNode* newnode = malloc(sizeof(asNode));
   	memcpy(newnode,node,sizeof(asNode));  
      newnode->next = list->head;
      newnode->prev = NULL;
      list->head->prev = newnode;
      list->head = newnode;
   }
	list->count += 1;
}

// Move iterator forward
function asForth(asList* list)
{
	if(list->itr != NULL)
		list->itr = list->itr->next;
}

//	Removes the node that the iterator points to.
function asRemoveNode(asList* list)
{
	// temporary node pointer.
	asNode* node;
	// if "itr" node is invalid, do nothing.
	if(list->itr == NULL)
		return;
	// save the pointer to the node we want to delete.
   node = list->itr;		
	// if the node we want to remove is the head or the tail
   // nodes, then move the head or tail to the next or
   // previous node.
   if(node == list->head)
   {
   	list->head = list->head->next;
   }
   else if(node == list->tail)
   {
   	list->tail = list->tail->prev;
   }
   // if a previous node exists, then make the previous
	// node point to the next node.
	if(node->prev != NULL)
		node->prev->next = node->next;
	// if the next node exists, then make the next node
	// point to the previous node.
	if(node->next != NULL)
		node->next->prev = node->prev;
		
	// if the head is NULL, then set the tail to NULL as well.
	if(list->head == NULL)
   	list->tail = NULL;
   	
	list->count -= 1;
	
	// delete the node.
	free(node);
}

#endif