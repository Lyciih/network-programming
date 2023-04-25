#include "network-programming.h"

//用來更新buffer計數器
int count_list_update(dllNode_t * count_list, int first_time)
{
	if(first_time == 1)
	{
		dllNode_t * current = count_list;

		while(current->next != NULL)
		{
			current = current->next;
			((number_temp *)current)->count--;
		}
	}
	return 0;
}

//用來尋找計數器變為0的buffer
number_temp * count_list_check(dllNode_t * count_list)
{
	dllNode_t * current = count_list;

	while(current->next != NULL)
	{
		current = current->next;
		if(((number_temp *)current)->count == 0)
		{
			return (number_temp *)current;
		}
	}
	return NULL;
}


