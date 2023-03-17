#include "network-programming.h"


//用來更新uffer計數器
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

//用來尋找計數器變為0的buffer
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


//解析指令
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

	//用來紀錄是否是每行的首個指令的狀態
	int first_time = 1;


	pid_t child_pid;
	
	while(1)
	{
		//主進程通往子進程的管道
		int pipe_p_c[2];
		pipe(pipe_p_c);

		//子進程通往主進程的管道
		int pipe_c_p[2];
		pipe(pipe_c_p);

		//參數計數器
		arg_count = 0;

		//紀錄判斷指令是否為數字的結果
		int not_number = 0;

		//命令處理---------------------------------------------

		//每行的第一次分析
		if(first_time == 1)
		{
			//用管道符號來切割
			pipe_split = strtok_r(command, "|\n", &save_p);

			if(pipe_split == NULL)
			{
				break;
			}

			//檢查指令是否為數字，前面幾次是多餘的只在最後一個指令才會發生作用
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


			//如果第一個指令就是數字,視為無效,退出分析函數
			if(not_number == 0)
			{
				printf("Unknow command: [%s].\n", pipe_split);
				return 0;
			}


			//將剛剛拿到的指令段以空白切割,取得所有參數

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
				else //在最後填入一個NULL參數
				{
					arg_count++;
					arg[arg_count] = NULL;
					break;
				}
			
			}


			//比對是否為可執行的指令,是的話就更新buffer計數器
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

			//首次切割結束,將狀態改變
			first_time = 0;
		}
		else //第一次切割做完後,以後的指令分析都在這做
		{
			//以管道符號切割命令
			pipe_split = strtok_r(NULL, "|\n", &save_p);

			if(pipe_split == NULL)
			{
				break;
			}

			//檢查命令是否為數字---------------------------
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


			//以空格切割,得到所有參數

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
				else  //在最後填一個NULL參數
				{
					arg_count++;
					arg[arg_count] = NULL;
					break;
				}
			
			}
		}

		//如果發現指令是quit,釋放所有buffer並退出
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
			//子進程做的事
			else if(child_pid == 0)
			{
				//把管道接好
				close(pipe_p_c[1]);

				dup2(pipe_p_c[0], STDIN_FILENO);
				close(pipe_p_c[0]);
				
				
				close(pipe_c_p[0]);

				dup2(pipe_c_p[1], STDOUT_FILENO);
				close(pipe_c_p[1]);


				//分析是否是可執行的指令,是的話就執行
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
			else //主進程如果發現是無法執行的指令,就回覆unknow
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
					//改變環境變數要在主進程做
					setenv(arg[1],arg[2], 1);
				}
				else
				{
					printf("Unknow command: [%s].\n", arg[0]);
				}


				//把管道接好
				close(pipe_p_c[0]);
				
				//檢查串列中是否有計數器為0的
				number_temp * zero = count_list_check(count_list);
				if(zero != NULL) //有的話就拿出來寫進pipe
				{
					//寫的時候不要連沒用到的空白也寫進去，所以用strlen
					write(pipe_p_c[1], zero->temp, strlen(zero->temp));

					//從list中刪除用完的節點
					DLL_delete(&(zero->node));
					free(zero->temp);
					free(zero);
				}
				else //沒有的話就寫進主進程的buffer
				{
					write(pipe_p_c[1], buffer, strlen(buffer));
				}
				close(pipe_p_c[1]);

				wait(0);

				close(pipe_c_p[1]);
				//讀的時候不知道對方有沒有裝滿，所以要全讀，用sizeof
				read(pipe_c_p[0], buffer, sizeof(buffer));
				close(pipe_c_p[0]);
			}
		}
		else  //使用者所輸入的整行命令的最後一段會來這裡分析
		{

			if(not_number == 0) //如果最後是數字,就新增一個節點,把buffer存起來
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
			else 			{
			
				child_pid = fork();

				if(child_pid == -1)
				{
					printf("error");
				}
				else if(child_pid == 0) //子進程的工作
				{
					//記得把寫端關掉，不然more會被子進程自己堵塞住
					//接好管線
					close(pipe_p_c[1]);
					dup2(pipe_p_c[0], STDIN_FILENO);
					close(pipe_p_c[0]);
					
					//最後一段命令若不是數字會直接output,不用接回主進程
					close(pipe_c_p[0]);
					close(pipe_c_p[1]);

					//分析是否為可執行的指令,可以的話就執行
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
				else //主進程的工作
				{


					//修改環境變數只在父進程做才有用
					if(strcmp(arg[0], "setenv") == 0)
					{
						setenv(arg[1],arg[2], 1);
					}


					close(pipe_p_c[0]);
					//檢查是否有計數器變為0的temp			
					number_temp * zero = count_list_check(count_list);
					if(zero != NULL)
					{
						//有的話就拿出來寫入
						write(pipe_p_c[1], zero->temp, strlen(zero->temp));

						//從list中刪除用完的節點
						DLL_delete(&(zero->node));
						free(zero->temp);
						free(zero);
					}
					else
					{
						//沒有的話就輸出主進程原本的buffer
						write(pipe_p_c[1], buffer, strlen(buffer));
					}

					//關閉所有用完的管線
					close(pipe_p_c[1]);
					close(pipe_c_p[1]);
					close(pipe_c_p[0]);

					//等待子進程結束
					wait(&child_pid);
					return 0;
				}
			}
		}
	}
	return 0;
}
