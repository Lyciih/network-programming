#include "dllSpec.h"

dllNode_t * DLL_get_tail(dllNode_t *head)
{
	dllNode_t *current = head;
	if(current->next == NULL)
	{
		//printf(">>> This is an empty list (response from DLL_get_tail)\n");
	}
	else
	{
		while(current->next != NULL)
		{
			current = current->next;
		}
		//printf(">>> Tail found (response from DLL_get_tail )\n");
	}
	return current;
}
