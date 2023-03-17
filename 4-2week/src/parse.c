#include "network-programming.h"


int parse(char * command)
{
	char * pipe_split = NULL;
	char * space_split = NULL;
	char * save_p = NULL;
	char * save_s = NULL;

	char write_buf[1024] = {0};
	char read_buf[1024] = {0}; 

	int first_time = 1;
	int pipe_fd[2];
	pipe(pipe_fd);
	pid_t child_pid;
	
	while(1)
	{
		if(first_time == 1)
		{
			pipe_split = strtok_r(command, "|\n", &save_p);
			space_split = strtok_r(pipe_split, " \n", &save_s);

			first_time = 0;
		}
		else
		{
			pipe_split = strtok_r(NULL, "|\n", &save_p);
			space_split = strtok_r(pipe_split, " \n", &save_s);
		}

		if(strcmp(space_split, "quit") == 0)
		{
			exit(0);
		}


		//後面還有指令就分裂，並接好水管
		if(*save_p != '\0')
		{
			child_pid = fork();

			if(child_pid == -1)
			{
				printf("error");
			}
			else if(child_pid == 0)
			{
				dup2(pipe_fd[0], STDIN_FILENO);
				close(pipe_fd[0]);
				dup2(pipe_fd[1], STDOUT_FILENO);
				close(pipe_fd[1]);
				execv("./bin/ls",NULL);
				exit(0);
			}
			else
			{
				wait(0);
			}
		}
		else
		{
			
			child_pid = fork();

			if(child_pid == -1)
			{
				printf("error");
			}
			else if(child_pid == 0)
			{
				//記得把寫端關掉，不然more會被子進程自己堵塞住
				close(pipe_fd[1]);

				dup2(pipe_fd[0], STDIN_FILENO);
				close(pipe_fd[0]);

				execv("./bin/test",NULL);
				exit(0);
			}
			else
			{
				close(pipe_fd[0]);
				close(pipe_fd[1]);
				wait(0);
			}
			printf("final %s\n", space_split);
			break;
		}
	}

	return 0;
}
