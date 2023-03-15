#include "network-programming.h"




int main()
{


	printf("%% ");

	setenv("PATH", "bin:.", 1);
	char command_buffer[256];

	dllNode_t * count_list = DLL_init();
//	dllNode_t * current = NULL;
//	current = count_list;

	while(1)
	{
/*
		current = count_list;

		while(current->next != NULL)
		{
			current = current->next;
			printf("%d\n", ((number_temp *)current)->count);
			printf("%s\n", ((number_temp *)current)->temp);
		}
*/

		fgets(command_buffer, 256, stdin);
		parse(command_buffer, count_list);
		printf("%% ");
	}
}
