#include "network-programming.h"

int main()
{
	char buffer[5000];
	char * split = NULL;
	char * save_p = NULL;

	while(1)
	{
		printf("%%");
		fgets(buffer, 5000, stdin);

		split = strtok_r(buffer, "|\n", &save_p);
		if(split != NULL)
		{
			parse(split, save_p);
		}
	}

//	execlp("ls","ls","-l","-a",NULL);
}
