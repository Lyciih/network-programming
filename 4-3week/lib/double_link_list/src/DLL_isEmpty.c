#include "dllSpec.h"

int DLL_isEmpty(const dllNode_t *head)
{
	if(head->prev == NULL)
	{
		printf(">>> prev is empty (response from DLL_isEmpty )\n");
		if(head->next == NULL)
		{
			printf(">>> next is empty (response from DLL_isEmpty )\n");
			printf(">>> The check is completed, the head of the list is normal and empty (response from DLL_isEmpty)\n");
		}
		else
		{
			printf(">>> The check is completed, the list is not empty (response from DLL_isEmpty)\n");
		}
	}
	return 0;
}