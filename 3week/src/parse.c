#include "network-programming.h"


int parse(char * first_word, char * other)
{
	if(strcmp(first_word, "quit") == 0)
	{
		exit(0);
	}

	else if(strcmp(first_word, "ls") == 0)
	{
		pid_t childpid;
		switch(childpid = fork()){
			case -1:
				printf("error");
				break;
			case 0:
				//printf("i am child %d \n", getpid());
				//execlp("ls","ls","-l","-a",NULL);
				execv("./bin/ls",NULL);
				break;
			default:
				//printf("i am parent %d \n", getpid());
				waitpid(childpid, NULL, 0);
				break;
		}
	}
	else
	{
		printf("%s\n", first_word);
		printf("%s", other);
	}


	return 0;
}
