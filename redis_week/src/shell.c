#include "network-programming.h"

dllNode_t * client_list;
redisContext * pContext;
redisReply * reply;
redisReply * reply_2;
int listen_fd;
char prompt[30];
char name_temp[20];
char change_name_temp[20];


int main(int argc, char ** argv)
{



	listen_fd = 0;
	int connect_fd = 0;
	int send_state;
	int child_pid;
	int pipe_server_op[2];

	if(argc != 3)
	{
		printf("arg need ip and port\n");
		exit(1);
	}

	client_list = DLL_init();


	set_signal_child_terminate_action();
	set_signal_server_op1_action();
	set_signal_server_op2_action();
	set_signal_server_shutdown_action();

	//申請用來 listen 的socket
	if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error\n");
		exit(1);
	}
	else
	{
		printf("create listen socket success\n");
	}


	//用來填 server 地址的結構
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	//server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0)
	{
		printf( "ip addres error\n");
		exit(1);
	}
	server_addr.sin_port = htons(atoi(argv[2]));

	//bind
	if((bind(listen_fd,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0))
	{
		perror("bind error\n");
		exit(1);
	}
	else
	{
		printf("bind success\n");
	}


	//listen
	if((listen(listen_fd, 5) < 0))
	{
		perror("listen error\n");
		exit(1);
	}
	else
	{
		printf("listen success\n");
	}


	//accept
	socklen_t len;
	struct sockaddr_in client_addr;
	len = sizeof(client_addr);
	char client_ip[INET_ADDRSTRLEN];
	unsigned short client_port;

	while(1)
	{
		if((connect_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &len)) < 0)
		{
			if(errno = EINTR)
			{
				continue;
			}
			else
			{
				perror("accept error\n");
				exit(0);
			}
		}
		else
		{
			pipe(pipe_server_op);

			inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
			client_port = client_addr.sin_port;
			printf("accept success from %s %d\n", client_ip, client_port);





			child_pid = fork();

			if(child_pid == -1)
			{
				perror("server fork error");
			}
			else if(child_pid == 0)
			{
				close(pipe_server_op[0]);
				close(listen_fd);
				release_all_client(client_list);
				int count;
				char command_buffer[256];
				dllNode_t * count_list = DLL_init();

				setenv("PATH", "bin:.", 1);

			
				dup2(connect_fd, STDOUT_FILENO);
		
				pContext = redisConnect("127.0.0.1", 6379);
				if(pContext == NULL)
				{
					printf("pContext null!\n");
					exit(1);
				}
				if(pContext->err)
				{
					printf("connection error:%s\n", pContext->errstr);
					exit(1);
				}

				printf("redis connect success\n");



				char password_temp[20];

				while(1)
				{
					//取得帳號
					if((send_state = send(connect_fd, "user_name:", sizeof("user_name:"), 0)) < 0)
					{
						perror("\n");
					}

					count = read(connect_fd, name_temp, 20);
					if(count >= 19)
					{
						send(connect_fd, "name too long\n% ", sizeof("name too long\n%% "), 0);
					}


					char * cut;
					cut = strstr(name_temp, "\r\n");
					if(cut != NULL)
					{
						*cut = '\0';
					}
					reply = redisCommand(pContext, "hget client:%s password", name_temp);



					//取得密碼
					if((send_state = send(connect_fd, "password:", sizeof("password:"), 0)) < 0)
					{
						perror("\n");
					}


					count = read(connect_fd, password_temp, 20);
					if(count >= 19)
					{
						send(connect_fd, "password too long\n% ", sizeof("password too long\n%% "), 0);
					}
					cut = strstr(password_temp, "\r\n");
					if(cut != NULL)
					{
						*cut = '\0';
					}
					
					if(reply->type == 4)
					{
						if((send_state = send(connect_fd, "User not found !\n", sizeof("User not found !\n"), 0)) < 0)
						{
							perror("\n");
						}
						
						if((send_state = send(connect_fd, "Create account or login again ? <1/2> : ", sizeof("Create account or login again ? <1/2> : "), 0)) < 0)
						{
							perror("\n");
						}
						
						count = read(connect_fd, command_buffer, 256);
						if(count >= 255)
						{
							send(connect_fd, "input too long\n% ", sizeof("input too long\n%% "), 0);
						}
						cut = strstr(command_buffer, "\r\n");
						if(cut != NULL)
						{
							*cut = '\0';
						}

						if(strcmp(command_buffer, "1") == 0)
						{
							while(1)
							{
								int get_name = 1;
								while(get_name)
								{
									if((send_state = send(connect_fd, "your user name: ", sizeof("your user name: "), 0)) < 0)
									{
										perror("\n");
									}
									else
									{
										count = read(connect_fd, name_temp, 20);
									
										if(count >= 19)
										{
											send(connect_fd, "input too long\n% ", sizeof("input too long\n%% "), 0);
										}
										else
										{
											cut = strstr(name_temp, "\r\n");
											if(cut != NULL)
											{
												*cut = '\0';
											}
											get_name = 0;
										}
									}
								}
								

								int get_password = 1;
								while(get_password)
								{
									if((send_state = send(connect_fd, "your password: ", sizeof("your password: "), 0)) < 0)
									{
										perror("\n");
									}
									else
									{
									
										count = read(connect_fd, password_temp, 20);
										if(count >= 19)
										{
											send(connect_fd, "input too long\n% ", sizeof("input too long\n%% "), 0);
										}
										else
										{
											cut = strstr(password_temp, "\r\n");
											if(cut != NULL)
											{
												*cut = '\0';
											}
											get_password = 0;
										}
									}
								}
								

								reply = redisCommand(pContext, "keys client:%s", name_temp);
								if(reply->elements == 0)
								{
									reply = redisCommand(pContext, "hmset client:%s password %s", name_temp, password_temp);
									
									if(reply->type != 6)
									{
										if((send_state = send(connect_fd, "Create success !\n", sizeof("Create success !\n"), 0)) < 0)
										{
											perror("\n");
										}

										break;
									}
								}
								else
								{
									if((send_state = send(connect_fd, "User name already exist!\n", sizeof("User name already exist!\n"), 0)) < 0)
									{
										perror("\n");
									}
								}
							}
						}

					}
					else
					{
						if(strcmp(password_temp, reply->str) != 0)
						{
							if((send_state = send(connect_fd, "Password error !\n", sizeof("Password error !\n"), 0)) < 0)
							{
								perror("\n");
							}
						}
						else
						{
							break;
						}
					}
				}

				memset(prompt, 0, sizeof(prompt));
				sprintf(prompt, "(%s)%% ", name_temp);
				send(connect_fd, prompt, sizeof(prompt), 0);

				sigval_t server_op;
				write(pipe_server_op[1], name_temp, strlen(name_temp));
				server_op.sival_int = 3;
				sigqueue(getppid(), 34, server_op);

				while(1)
				{


					count = read(connect_fd, command_buffer, 256);
					if(count >= 255)
					{
						send(connect_fd, "command too long\n% ", sizeof("command too long\n%% "), 0);
					}
					else
					{
						char * cut = strstr(command_buffer, "\r\n");
						if(cut != NULL)
						{
							*cut = '\0';
							parse(command_buffer, count_list, connect_fd, pipe_server_op[1]);
						}
					}
				}
			}
			else
			{
				pContext = redisConnect("127.0.0.1", 6379);
				if(pContext == NULL)
				{
					printf("pContext null!\n");
					exit(1);
				}
				if(pContext->err)
				{
					printf("connection error:%s\n", pContext->errstr);
					exit(1);
				}

				close(pipe_server_op[1]);
			
				client_data * new = malloc(sizeof(client_data));
				if(new == NULL)
				{
					printf("not enough memory to add new client\n");
				}
				else
				{
					new->node.prev = NULL;
					new->node.next = NULL;
					strcpy(new->name, "no_name");
					strcpy(new->ip, client_ip);
					new->port = client_port;
					new->pid = child_pid;
					new->socket_fd = connect_fd;
					new->server_op_pipe = pipe_server_op[0];

					client_add(new, client_list);
				}



			}
		}
	}
	return 0;
}
