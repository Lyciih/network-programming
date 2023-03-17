#include "network-programming.h"

int count_list_update(dllNode_t * count_list)
{
	dllNode_t * current = count_list;

	while(current->next != NULL)
	{
		current = current->next;
		((number_temp *)current)->count--;
	}
	return 0;
}


number_temp * count_list_check(dllNode_t * count_list)
{
	dllNode_t * current = count_list;

	while(current->next != NULL)
	{
		current = current->next;
		if(((number_temp *)current)->count == 0)
		{
			return (number_temp *)current;
		}
	}
	return NULL;
}


int parse(char * command, dllNode_t * count_list)
{
	char * pipe_split = NULL;
	char * space_split = NULL;
	char * save_p = NULL;
	char * save_s = NULL;

	char buffer[5000];
	memset(buffer, 0, sizeof(buffer));

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
		int not_number = 0;

		//命令處理---------------------------------------------
		if(first_time == 1)
		{
			pipe_split = strtok_r(command, "|\n", &save_p);

			if(pipe_split == NULL)
			{
				break;
			}

			//檢查最後是否為數字，前面幾次是多餘的只在最後一個指令才會發生作用
			int check = 0;
			while(check < strlen(pipe_split))
			{
				if(isdigit(*(pipe_split + check)) == 0)
				{
					not_number = 1;
					break;
				}
				else
				{
					check++;
				}
			}

			if(not_number == 0)
			{
				printf("Unknow command: [%s].\n", pipe_split);
				return 0;
			}


			//----------------------------------------------

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



			if(strcmp(arg[0], "ls") == 0)
			{
				count_list_update(count_list);
			}
			else if(strcmp(arg[0], "more") == 0)
			{
				count_list_update(count_list);
			}
			else if(strcmp(arg[0], "number") == 0)
			{
				count_list_update(count_list);
			}
			else if(strcmp(arg[0], "cat") == 0)
			{
				count_list_update(count_list);
			}
			else if(strcmp(arg[0], "printenv") == 0)
			{
				count_list_update(count_list);
			
			}
			else if(strcmp(arg[0], "setenv") == 0)
			{
				count_list_update(count_list);
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

			//檢查最後是否為數字---------------------------
			int check = 0;
			while(check < strlen(pipe_split))
			{
				if(isdigit(*(pipe_split + check)) == 0)
				{
					not_number = 1;
					break;
				}
				else
				{
					check++;
				}
			}


			//----------------------------------------------

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
			while(count_list->next != NULL)
			{
				dllNode_t * temp = count_list->next;
				DLL_delete(count_list->next);
				free(((number_temp *)temp)->temp);
				free(temp);
			}
			free(count_list);
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
					execvp("ls",arg);
				}
				else if(strcmp(arg[0], "more") == 0)
				{
					execvp("more",arg);
				}
				else if(strcmp(arg[0], "number") == 0)
				{
					execvp("number",arg);
				}
				else if(strcmp(arg[0], "cat") == 0)
				{
					execvp("cat",arg);
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


				close(pipe_p_c[0]);
				//寫的時候不要連沒用到的空白也寫進去，所以用strlen
				number_temp * zero = count_list_check(count_list);
				if(zero != NULL)
				{
					write(pipe_p_c[1], zero->temp, strlen(zero->temp));
				}
				else
				{
					write(pipe_p_c[1], buffer, strlen(buffer));
				}
				close(pipe_p_c[1]);

				wait(0);

				close(pipe_c_p[1]);
				//讀的時候不知道對方有沒有裝滿，所要要全讀，用sizeof
				read(pipe_c_p[0], buffer, sizeof(buffer));
				close(pipe_c_p[0]);
			}
		}
		else
		{

			if(not_number == 0)
			{
				number_temp * new = malloc(sizeof(number_temp));
				if(new == NULL)
				{
					printf("not enough memory");
					break;
				}
				else
				{
					new->node.prev = NULL;
					new->node.next = NULL;
					new->count =  atoi(arg[0]);
					new->temp = (char *)malloc(5000);
					if(new->temp == NULL)
					{
						printf("not enough memory");
						break;
					}
					else
					{
						strcpy(new->temp, buffer);
						DLL_add_tail(&new->node, count_list);
						break;
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
						execvp("ls",arg);
					}
					else if(strcmp(arg[0], "more") == 0)
					{
						execvp("more",arg);
					}
					else if(strcmp(arg[0], "number") == 0)
					{
						execvp("number",arg);
					}
					else if(strcmp(arg[0], "cat") == 0)
					{
						execvp("cat",arg);
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
					else if(not_number == 0)
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


					//setenv只能在父進程做才有用

					if(strcmp(arg[0], "setenv") == 0)
					{
						setenv(arg[1],arg[2], 1);
					}

					close(pipe_p_c[0]);
					//寫			
					number_temp * zero = count_list_check(count_list);
					if(zero != NULL)
					{
						write(pipe_p_c[1], zero->temp, strlen(zero->temp));
					}
					else
					{
						write(pipe_p_c[1], buffer, strlen(buffer));
					}
					close(pipe_p_c[1]);
					

					close(pipe_c_p[1]);
					close(pipe_c_p[0]);

					wait(&child_pid);
					return 0;
				}
			}
		}
	}
	return 0;
}
