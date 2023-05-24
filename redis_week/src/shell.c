#include "network-programming.h"

dllNode_t * client_list;


int main(int argc, char ** argv)
{
	redisContext * pContext = redisConnect("127.0.0.1", 6379);
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

	printf("redis success\n");
	redisFree(pContext);


	int listen_fd = 0;
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
				char command_buffer[256];
				dllNode_t * count_list = DLL_init();

				setenv("PATH", "bin:.", 1);

				//dup2(connect_fd, STDIN_FILENO);
				dup2(connect_fd, STDOUT_FILENO);

			
				if((send_state = send(connect_fd, "hello\n", sizeof("hello\n"), 0)) < 0)
				{
					perror("\n");
				}


 				send(connect_fd, "% ", sizeof("% "), 0);
				
				while(1)
				{
					int count;

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

	
//	release_all_client(client_list);
	close(listen_fd);
	return 0;

}
