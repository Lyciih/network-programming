#include "dllSpec.h"

dllNode_t * DLL_init()
{
	dllNode_t *init = (dllNode_t*)malloc(sizeof(dllNode_t));
	if(init == NULL)
	{
//		printf(">>> Insufficient space (response from DLL_init )\n");
	}
	else
	{
//		printf(">>> Application successful (response from DLL_init )\n");
		init->prev = NULL;
		init->next = NULL;
	}
	return init;
}
