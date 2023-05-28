#include "network-programming.h"
extern char prompt[30];
extern char change_name_temp[20];
extern redisContext * pContext;
extern redisReply * reply;
extern redisReply * reply_2;
extern char name_temp[20];

time_t now;
int year, month, day, second, minute, hour;
char redirect_buffer[5000];

//解析指令
int parse(char * command, dllNode_t * count_list, int connect_fd, int server_op_pipe)
{
	char * pipe_split = NULL;
	char * space_split = NULL;
	char * save_p = NULL;
	char * save_s = NULL;

	char buffer[5000];
	memset(buffer, 0, sizeof(buffer));
	char redirect_buffer[5000];
	memset(buffer, 0, sizeof(redirect_buffer));

	//分解參數
	char * arg[10] = {NULL};
	int arg_count = 0;
	char * redirect_arg[10] = {NULL};
	int redirect_arg_count = 0;

	//用來紀錄是否是每行的首個指令的狀態
	int first_time = 1;
	int check;

	char * getenv_state;
	int setenv_state;
	
	sigval_t server_op;

	int mailto_type = 0;

	while(1)
	{
		mailto_type = 0;
		//參數計數器
		arg_count = 0;
		redirect_arg_count = 0;

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
		else if(strcmp(arg[0], "mailto") == 0)
		{
			space_split = strtok_r(NULL, " \n", &save_s);
			arg[1] = space_split;
			arg[2] = save_s;
			if(*arg[2] == '<')
			{
				arg[3] = NULL;
				mailto_type = 1;
				space_split = strtok_r(NULL, "<", &save_s);
				space_split = strtok_r(space_split, " \n", &save_s);
				redirect_arg[0] = space_split;
				
				while(1)
				{
					space_split = strtok_r(NULL, " \n", &save_s);
					if(space_split != NULL)
					{
						redirect_arg_count++;
						redirect_arg[redirect_arg_count] = space_split;
						break;
					}
					else //在最後填入一個NULL參數
					{
						redirect_arg_count++;
						redirect_arg[redirect_arg_count] = NULL;
						break;
					}
				
				}
				
				if(strcmp(redirect_arg[0], "ls") == 0)
				{
					command_need_fork(redirect_arg[0], sizeof(redirect_arg[0]), redirect_arg, redirect_buffer, "<", count_list);
				}
			}
			else
			{
				arg[3] = NULL;
			}
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
			count_list_update(count_list, first_time);
			server_op.sival_int = 0;
			sigqueue(getppid(), 34, server_op);
			return 0;
		}
		else if(strcmp(arg[0], "tell") == 0)
		{
			count_list_update(count_list, first_time);
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
			count_list_update(count_list, first_time);
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
			count_list_update(count_list, first_time);
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
		else if(strcmp(arg[0], "listmail") == 0)
		{
			count_list_update(count_list, first_time);
			reply = redisCommand(pContext, "keys mail:%s:*", name_temp);
			
			if(reply->elements == 0)
			{
				send(connect_fd, "Empty !\n", sizeof("Empty !"), 0);
			}
			else
			{
				printf("<id> <date>			<sender>	<message>\n");
	
				reply = redisCommand(pContext, "get mailbox:%s", name_temp);
				
				for(int i = atoi(reply->str); i > 0; i--)
				{	
					reply_2 = redisCommand(pContext, "hvals mail:%s:%d", name_temp, i);
					
					if(reply_2->elements != 0)
					{
						printf("%2s   %s %s	%s		%s\n", reply_2->element[0]->str, reply_2->element[1]->str, reply_2->element[2]->str, reply_2->element[3]->str, reply_2->element[4]->str);
					}
				}
			}
		}
		else if(strcmp(arg[0], "mailto") == 0)
		{
			count_list_update(count_list, first_time);
			reply = redisCommand(pContext, "keys client:%s", arg[1]);
			
			if(reply->elements == 0)
			{
				send(connect_fd, "User not found\n", sizeof("User not found\n"), 0);
			}
			else
			{
				reply = redisCommand(pContext, "incr mailbox:%s", arg[1]);
				
				reply = redisCommand(pContext, "get mailbox:%s", arg[1]);

				time(&now);
				struct tm * local = localtime(&now);
				hour = local->tm_hour;
				minute = local->tm_min;
				second = local->tm_sec;
				day = local->tm_mday;
				month = local->tm_mon + 1;
				year = local->tm_year + 1900;

				char time[20];
				char date[20];
				sprintf(time, "%02d:%02d:%02d", hour, minute, second);
				sprintf(date, "%04d-%02d-%02d", year, month, day);
				
				if(mailto_type == 0)
				{
					reply = redisCommand(pContext, "hmset mail:%s:%s id %s date %s time %s sender %s message %s", arg[1], reply->str, reply->str, date, time, name_temp, arg[2]);
				}
				else
				{
					reply = redisCommand(pContext, "hmset mail:%s:%s id %s date %s time %s sender %s message %s", arg[1], reply->str, reply->str, date, time, name_temp, redirect_buffer);
				}
				send(connect_fd, "Send accept !\n", sizeof("Send accept !\n"), 0);
			}
		}
		else if(strcmp(arg[0], "delmail") == 0)
		{
			count_list_update(count_list, first_time);
			reply = redisCommand(pContext, "keys mail:%s:%s",name_temp, arg[1]);
			
			if(reply->elements == 0)
			{
				send(connect_fd, "Mail id unexist!\n", sizeof("Mail id unexist\n"), 0);
			}
			else
			{
				reply = redisCommand(pContext, "del mail:%s:%s", name_temp, arg[1]);
				printf("Delete accept !\n");
			}
		}
		else if(strcmp(arg[0], "creategroup") == 0)
		{
			count_list_update(count_list, first_time);
			reply = redisCommand(pContext, "keys group:%s:*", arg[1]);
			
			if(reply->elements == 0)
			{
				reply = redisCommand(pContext, "hmset group:%s:%s owner %s %s 0", arg[1], name_temp, name_temp, name_temp);
				reply = redisCommand(pContext, "hmset %s:group %s %s", name_temp, arg[1], name_temp);
				printf("Create success !\n");

			}
			else
			{
				send(connect_fd, "Group already exist !\n", sizeof("Group already exist !\n"), 0);
			}
		}
		else if(strcmp(arg[0], "listgroup") == 0)
		{
			count_list_update(count_list, first_time);
			reply = redisCommand(pContext, "hgetall %s:group", name_temp);
			
			if(reply->elements == 0)
			{
				printf("Empty !\n");
			}
			else
			{
				printf("<owner>		<group>\n");
				
				for(int i = 0; i < reply->elements; i++)
				{	
					printf("%s		%s\n", reply->element[i+1]->str, reply->element[i]->str);
					i++;
				}
			}
		}
		else if(strcmp(arg[0], "addto") == 0)
		{
			count_list_update(count_list, first_time);
			reply = redisCommand(pContext, "keys group:%s:*", arg[1]);
			
			if(reply->elements == 0)
			{
				printf("group not found !\n");
			}
			else
			{
				reply = redisCommand(pContext, "keys group:%s:%s", arg[1], name_temp);
				if(reply->elements == 0)
				{
					printf("You don't have permission !\n");
				}
				else
				{

					//從第二個參數開始
					int count = 2;
					while(arg[count] != NULL)
					{
						reply = redisCommand(pContext, "keys client:%s", arg[count]);
						if(reply->elements == 0)
						{
							printf("%s not found !\n", arg[count]);
						}
						else
						{
							
							reply = redisCommand(pContext, "hexists group:%s:%s %s", arg[1], name_temp, arg[count]);
							if(reply->integer == 1)
							{
								printf("%s already in group !\n", arg[count]);
							}	
							else
							{
								reply = redisCommand(pContext, "hmset group:%s:%s %s 0", arg[1], name_temp, arg[count]);
								reply = redisCommand(pContext, "hmset %s:group %s %s", arg[count], arg[1], name_temp);
								printf("%s add success !\n", arg[count]);
							}
						}
						count++;
					}
				}
			}
		}
		else if(strcmp(arg[0], "leavegroup") == 0)
		{
			reply = redisCommand(pContext, "keys group:%s:*", arg[1]);
			
			if(reply->elements == 0)
			{
				printf("group not found !\n");
			}
			else
			{
				reply = redisCommand(pContext, "hexists %s:group %s", name_temp, arg[1]);
				if(reply->integer == 1)
				{
					reply = redisCommand(pContext, "hget %s:group %s", name_temp, arg[1]);
					reply = redisCommand(pContext, "hdel group:%s:%s %s", arg[1], reply->str, name_temp);
					reply = redisCommand(pContext, "hdel %s:group %s", name_temp, arg[1]);
					printf("Leave accept !\n");
				}
				else
				{
					printf("Leave fault !\n");
				}
			}

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
