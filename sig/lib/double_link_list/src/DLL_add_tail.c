#include "dllSpec.h"

void DLL_add_tail(dllNode_t * new_node, dllNode_t *head)
{
	dllNode_t *current = DLL_get_tail(head);
	DLL_addto_next(new_node, current);
}