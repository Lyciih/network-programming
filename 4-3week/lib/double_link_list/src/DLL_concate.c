#include "dllSpec.h"

dllNode_t * DLL_concate(dllNode_t *srcList, dllNode_t * dstList)//(將srcList 串在dstList之後)
{
	dllNode_t *tail = DLL_get_tail(dstList);
	tail->next = srcList->next;
	srcList->next->prev = tail;
	srcList->next = NULL;
	DLL_free_head(srcList);
}