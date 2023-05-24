#include "dllSpec.h"

void DLL_delete(dllNode_t * node)//(從node所在的 Linked List 中刪除此點)
{
	if(node->next == NULL)
	{
		node->prev->next = NULL;
		node->prev = NULL;
	}
	else
	{
		node->prev->next = node->next;
		node->next->prev = node->prev;
		node->prev = NULL;
		node->next = NULL;
	}
}