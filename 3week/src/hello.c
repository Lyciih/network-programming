#include "network-programming.h"

int main()
{
	char buffer[100];
	char * parse = NULL;
	char * save_p = NULL;

	while(1)
	{
		printf("%%");
		fgets(buffer, 100, stdin);
		int state = 1;

		parse = strtok_r(buffer, " \n", &save_p);
		if(parse != NULL)
		{
			printf("%s", parse);
		}

		while(state)
		{
			parse = strtok_r(NULL, " \n ", &save_p);
			if(parse != NULL)
			{
				printf("\n%s", parse);
			}
			else
			{
				state = 0;
				printf("\n");
			}
		}
	}
}
