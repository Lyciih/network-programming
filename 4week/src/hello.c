#include "network-programming.h"

int main()
{
	char buffer[5000];

	//用來等待最後一個孫子
	int final_pipe[2];
	pipe(final_pipe);
	int final_pid;

	
	char main_read_buf[1024] = {0};
	char main_write_buf[1024] = {0};

	while(1)
	{
		printf("%%");
		fgets(buffer, 5000, stdin);

		pid_t parse_pid;
		parse_pid = fork();

		if(parse_pid == -1)
		{
			printf("error\n");
		}
		else if(parse_pid == 0)
		{
			parse(buffer, final_pipe[1]);
		}
		else
		{
			kill(getpid(), SIGSTOP);
			/*
			//取得最後一個孫子的PID 用來等待所有孫子結束
			read(final_pipe[0], main_read_buf, sizeof(main_read_buf));

			final_pid = atoi(main_read_buf);
			wait(&final_pid);
			printf("%s\n", main_read_buf);
			*/
		}
	}
}
