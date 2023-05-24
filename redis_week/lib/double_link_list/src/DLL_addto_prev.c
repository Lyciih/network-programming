#include "dllSpec.h"

void DLL_addto_prev(dllNode_t *new_node, dllNode_t *node)
{
	if(node->prev != NULL)
	{
		new_node->prev = node->prev;
		node->prev->next = new_node;
		node->prev = new_node;
		new_node->next = node;
		printf(">>> Insertion complete (response from DLL_addto_prev)\n");
	}
	else
	{
		printf(">>> Here is the head, which cannot be inserted in prev (response from DLL_addto_prev)\n");
	}
}