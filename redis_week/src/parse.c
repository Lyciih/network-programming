#include "network-programming.h"
extern char prompt[30];
extern char change_name_temp[20];

//解析指令
int parse(char * command, dllNode_t * count_list, int connect_fd, int server_op_pipe)
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

	//用來紀錄是否是每行的首個指令的狀態
	int first_time = 1;
	int check;

	char * getenv_state;
	int setenv_state;
	
	sigval_t server_op;

	while(1)
	{
		//參數計數器
		arg_count = 0;

		//紀錄判斷指令是否為數字的結果
		int not_number = 0;

		//命令處理---------------------------------------------

		//是否是第一次分析
		if(first_time == 1)
		{
			//用管道符號來切割
			pipe_split = strtok_r(command, "|\n", &save_p);
		}
		else
		{
			pipe_split = strtok_r(NULL, "|\n", &save_p);
		}


		if(pipe_split == NULL)
		{
			break;
		}



		//如果第一個指令就是數字,視為無效,退出分析函數
		if(first_time == 1)
		{
			//檢查指令是否為數字
			check = 0;
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
				break;
			}
		}

		


		//將剛剛拿到的指令段以空白切割,取得所有參數

		space_split = strtok_r(pipe_split, " \n", &save_s);
		arg[0] = space_split;

		if(strcmp(arg[0], "yell") == 0)
		{
			arg[1] = save_s;
			arg[2] = NULL;
		}
		else if(strcmp(arg[0], "tell") == 0)
		{
			space_split = strtok_r(NULL, " \n", &save_s);
			arg[1] = space_split;
			arg[2] = save_s;
			arg[3] = NULL;
		}
		else
		{
			while(1)
			{
				space_split = strtok_r(NULL, " \n", &save_s);
				if(space_split != NULL)
				{
					arg_count++;
					arg[arg_count] = space_split;
				}
				else //在最後填入一個NULL參數
				{
					arg_count++;
					arg[arg_count] = NULL;
					break;
				}
			
			}
		}


		//比對是否為可執行的指令,是的話就更新buffer計數器
		if(strcmp(arg[0], "ls") == 0)
		{
			count_list_update(count_list, first_time);
			command_need_fork(arg[0], sizeof(arg[0]), arg, buffer, save_p, count_list);
		}
		else if(strcmp(arg[0], "more") == 0)
		{
			count_list_update(count_list, first_time);
			command_need_fork(arg[0], sizeof(arg[0]), arg, buffer, save_p, count_list);
		}
		else if(strcmp(arg[0], "number") == 0)
		{
			count_list_update(count_list, first_time);
			command_need_fork(arg[0], sizeof(arg[0]), arg, buffer, save_p, count_list);
		}
		else if(strcmp(arg[0], "cat") == 0)
		{
			count_list_update(count_list, first_time);
			command_need_fork(arg[0], sizeof(arg[0]), arg, buffer, save_p, count_list);
		}
		else if(strcmp(arg[0], "printenv") == 0)
		{
			count_list_update(count_list, first_time);


			getenv_state = getenv(arg[1]);
			if(getenv_state != NULL)
			{
				printf("%s\n", getenv_state);
			}
			else
			{
				printf("getenv error\n");
			}
		}
		else if(strcmp(arg[0], "setenv") == 0)
		{

			count_list_update(count_list, first_time);
			//改變環境變數要在主進程做
			setenv_state = setenv(arg[1],arg[2], 1);
			if(setenv_state != 0)
			{
				printf("setenv error\n");
			}
		}
		else if(strcmp(arg[0], "who") == 0)
		{
			server_op.sival_int = 0;
			sigqueue(getppid(), 34, server_op);
			return 0;
		}
		else if(strcmp(arg[0], "tell") == 0)
		{
			if(*arg[2] == '\0')
			{
				printf("please input message\n");
				break;
			}
			write(server_op_pipe, arg[2], strlen(arg[2]));
			server_op.sival_int = atoi(arg[1]);
			sigqueue(getppid(), 35, server_op);
			return 0;
		}
		else if(strcmp(arg[0], "yell") == 0)
		{
			if(*arg[1] == '\0')
			{
				printf("please input message\n");
				break;
			}
			write(server_op_pipe, arg[1], strlen(arg[1]));
			server_op.sival_int = 1;
			sigqueue(getppid(), 34, server_op);
			return 0;
		}
		else if(strcmp(arg[0], "name") == 0)
		{
			if(arg[1] == NULL)
			{
				printf("arg[1] is NULL\n");
				break;
			}
			else if(strlen(arg[1]) > 14)	//雖然結構是 15 byte 但要留 1 byte 給結束符
			{
				printf("name is too long\n");
				break;
			}
			else
			{
				strcpy(change_name_temp, arg[1]);
				write(server_op_pipe, arg[1], strlen(arg[1]));
				server_op.sival_int = 2;
				sigqueue(getppid(), 34, server_op);
			}
			return 0;
		}
		else if(strcmp(arg[0], "quit") == 0)
		{
			while(count_list->next != NULL)
			{
				dllNode_t * temp = count_list->next;
				DLL_delete(count_list->next);
				free(((number_temp *)temp)->temp);
				free(temp);
			}
			free(count_list);
			close(connect_fd);
			close(server_op_pipe);
			server_op.sival_int = 0;
			sigqueue(getppid(), 36, server_op);
		
			exit(0);
		}
		else if(*save_p == '\0')
		{
			//檢查指令是否為數字
			check = 0;
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

			//如果最後一個指令是數字,就新增一個節點,把buffer存起來
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
						//把buffer的資料複製進節點
						strcpy(new->temp, buffer);
						//加進串列
						DLL_add_tail(&new->node, count_list);
						break;
					}
				}
			}
			else
			{
				printf("Unknow command: [%s].\n", arg[0]);
				break;
			}
		}
		else
		{
			printf("Unknow command: [%s].\n", arg[0]);
		}
		
		//只有第一次切割時會是1,之後都會是0
		if(first_time == 1)
		{
			first_time = 0;
		}
	}
	send(connect_fd, prompt, sizeof(prompt), 0);
	return 0;
}
