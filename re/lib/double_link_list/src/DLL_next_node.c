#include "dllSpec.h"

dllNode_t * DLL_next_node(const dllNode_t * node)//(下一個節點)
{
	return node->next;
}

dllNode_t * DLL_prev_node(const dllNode_t * node)//(上一個節點)
{
	return node->prev;
}