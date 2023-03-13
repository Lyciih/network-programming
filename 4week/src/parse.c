#include "network-programming.h"


int parse(char * command)
{
	char * split = NULL;
	char * save_p = NULL;

	char write_buf[1024] = {0};
	char read_buf[1024] = {0}; 

	int first_time = 1;

	int pipe_fd[2];
	
	while(1)
	{
		if(first_time == 1)
		{
			split = strtok_r(command, "|\n", &save_p);
			first_time = 0;
		}
		else
		{
			split = strtok_r(NULL, "|\n", &save_p);
		}


		//後面還有指令就分裂，並接好水管
		if(*save_p != '\0')
		{
			//產生新水管
			pipe(pipe_fd);

			pid_t other_pid;
			other_pid = fork();

			if(other_pid == -1)
			{
				printf("error");
				exit(0);
			}
			//父進程
			else if(other_pid != 0)
			{
				//關閉水管讀端
				close(pipe_fd[0]);
				//將寫端接上 stdout
				dup2(pipe_fd[1], STDOUT_FILENO);
				close(pipe_fd[1]);

				printf("father %d %s\n", getpid(), split);
				exit(0);
			}
			else
			{
				//關閉水管寫端
				close(pipe_fd[1]);
				//將讀端接上 stdin
				dup2(pipe_fd[0], STDIN_FILENO);
				close(pipe_fd[0]);
			}
		}

		//後面沒有指令，就把這回合做完，並接回stdout 同時向主進程發結束訊號
		else
		{
			//printf("final %s %d\n", split, getpid());
			sprintf(write_buf, "%d", getpid());


			write(final_write_fd, write_buf, sizeof(write_buf));
			close(final_write_fd);

			
			execv("./bin/ls",NULL);
		}
	}

	return 0;
}
