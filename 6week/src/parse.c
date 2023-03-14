#include "network-programming.h"


int parse(char * command)
{
	char * pipe_split = NULL;
	char * space_split = NULL;
	char * save_p = NULL;
	char * save_s = NULL;

	char buffer[5000];


	//分解參數
	char * arg[10] = {NULL};
	int arg_count = 0;


	int first_time = 1;


	pid_t child_pid;
	
	while(1)
	{
		int pipe_p_c[2];
		pipe(pipe_p_c);

		int pipe_c_p[2];
		pipe(pipe_c_p);

		arg_count = 0;

		//命令處理---------------------------------------------
		if(first_time == 1)
		{
			pipe_split = strtok_r(command, "|\n", &save_p);

			if(pipe_split == NULL)
			{
				break;
			}

			space_split = strtok_r(pipe_split, " \n", &save_s);
			arg[0] = space_split;

			while(1)
			{
				space_split = strtok_r(NULL, " \n", &save_s);
				if(space_split != NULL)
				{
					arg_count++;
					arg[arg_count] = space_split;
				}
				else
				{
					arg_count++;
					arg[arg_count] = NULL;
					break;
				}
			
			}

			first_time = 0;
		}
		else
		{
			pipe_split = strtok_r(NULL, "|\n", &save_p);

			if(pipe_split == NULL)
			{
				break;
			}

			space_split = strtok_r(pipe_split, " \n", &save_s);
			arg[0] = space_split;

			while(1)
			{
				space_split = strtok_r(NULL, " \n", &save_s);
				if(space_split != NULL)
				{
					arg_count++;
					arg[arg_count] = space_split;
				}
				else
				{
					arg_count++;
					arg[arg_count] = NULL;
					break;
				}
			
			}
		}

		if(strcmp(arg[0], "quit") == 0)
		{
			exit(0);
		}


		//後面還有指令的分裂
		if(*save_p != '\0')
		{
			child_pid = fork();

			if(child_pid == -1)
			{
				printf("error");
			}
			else if(child_pid == 0)
			{
				close(pipe_p_c[1]);

				dup2(pipe_p_c[0], STDIN_FILENO);
				close(pipe_p_c[0]);
				
				
				close(pipe_c_p[0]);

				dup2(pipe_c_p[1], STDOUT_FILENO);
				close(pipe_c_p[1]);

				if(strcmp(arg[0], "ls") == 0)
				{
					execv("./bin/ls",arg);
				}
				else if(strcmp(arg[0], "more") == 0)
				{
					execv("./bin/more",arg);
				}
				else if(strcmp(arg[0], "number") == 0)
				{
					execv("./bin/number",arg);
				}
				else if(strcmp(arg[0], "cat") == 0)
				{
					execv("./bin/cat",arg);
				}
				else if(strcmp(arg[0], "printenv") == 0)
				{
					printf("%s\n", getenv(arg[1]));
					exit(0);
				}
				else if(strcmp(arg[0], "setenv") == 0)
				{
					exit(0);
				}
				else
				{
					exit(0);
				}
			}
			else
			{
				close(pipe_p_c[0]);
				//寫的時候不要連沒用到的空白也寫進去，所以用strlen			
				write(pipe_p_c[1], buffer, strlen(buffer));
				close(pipe_p_c[1]);

				wait(0);

				close(pipe_c_p[1]);
				//讀的時候不知道對方有沒有裝滿，所要要全讀，用sizeof
				read(pipe_c_p[0], buffer, sizeof(buffer));
				close(pipe_c_p[0]);

				if(strcmp(arg[0], "ls") == 0)
				{
				}
				else if(strcmp(arg[0], "more") == 0)
				{
				}
				else if(strcmp(arg[0], "number") == 0)
				{
				}
				else if(strcmp(arg[0], "cat") == 0)
				{
				}
				else if(strcmp(arg[0], "printenv") == 0)
				{
				
				}
				else if(strcmp(arg[0], "setenv") == 0)
				{
					setenv(arg[1],arg[2], 1);
				}
				else
				{
					printf("Unknow command: [%s].\n", arg[0]);
				}
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

				close(pipe_p_c[1]);

				dup2(pipe_p_c[0], STDIN_FILENO);
				close(pipe_p_c[0]);
				
				close(pipe_c_p[0]);
				close(pipe_c_p[1]);

				if(strcmp(arg[0], "ls") == 0)
				{
					execv("./bin/ls",arg);
				}
				else if(strcmp(arg[0], "more") == 0)
				{
					execv("./bin/more",arg);
				}
				else if(strcmp(arg[0], "number") == 0)
				{
					execv("./bin/number",arg);
				}
				else if(strcmp(arg[0], "cat") == 0)
				{
					execv("./bin/cat",arg);
				}
				else if(strcmp(arg[0], "printenv") == 0)
				{
					printf("%s\n", getenv(arg[1]));
					exit(0);
				}
				else if(strcmp(arg[0], "setenv") == 0)
				{
					exit(0);
				}
				else
				{
					printf("Unknow command: [%s].\n", arg[0]);
					exit(0);
				}
			}
			else
			{
				if(strcmp(arg[0], "setenv") == 0)
				{
					setenv(arg[1],arg[2], 1);
				}


				close(pipe_p_c[0]);
				//寫			
				write(pipe_p_c[1], buffer, strlen(buffer));
				close(pipe_p_c[1]);
				

				close(pipe_c_p[1]);
				close(pipe_c_p[0]);

				wait(0);
			}
			break;
		}
	}

	return 0;
}
