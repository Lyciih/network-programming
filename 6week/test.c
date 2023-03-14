#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main()
{

	char *arg[10] = {NULL};
	char command[] = "test";
	printf("%s\n", command);

	char * split = NULL;
	char * save = NULL;
	int args = 0;

	split = strtok_r(command, " \n", &save);
	arg[0] = split;

	while(1)
	{
		split = strtok_r(NULL, " \n", &save);
		if(split != NULL)
		{
			args++;
			arg[args] = split;
		}
		else
		{
			break;
		}
	}

	char * env[]={"PATH=./",NULL};
	execve("test", NULL, env);
}
