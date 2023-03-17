#include "dllSpec.h"

void DLL_add_first(dllNode_t * new_node, dllNode_t * head)
{
	if(head->next == NULL)
	{
		head->next = new_node;
		new_node->prev = head;
		printf(">>> Add to empty list (response from DLL_add_first)\n");
	}
	else
	{
		new_node->next = head->next;
		head->next->prev = new_node;
		head->next = new_node;
		new_node->prev = head;
		printf(">>> Add to non-empty list (response from DLL_add_first)\n");
			
	}
}