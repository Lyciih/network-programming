#include "dllSpec.h"

void DLL_addto_next(dllNode_t *new_node, dllNode_t *node)
{
	if(node->next != NULL)
	{
		node->next->prev = new_node;
	}
	new_node->next = node->next;
	node->next = new_node;
	new_node->prev = node;
	//printf(">>> Insertion complete (response from DLL_addto_next)\n");
}
