#include "network-programming.h"

dllNode_t * client_list;




int client_add(client_data * new, dllNode_t * client)
{
	int id_current;
	int id_next;
	dllNode_t * current = NULL;

	if(client->next == NULL)
	{
		DLL_addto_next(&new->node, client);
		new->ID = 0;
		return 0;
	}
	else
	{
		current = client;		
		current = current->next;

		if((((client_data *)current)->ID) > 0)
		{
			DLL_addto_prev(&new->node, current);
			new->ID = 0;
			return 0;
		}
		else
		{
			if(current->next == NULL)
			{
				id_current = ((client_data *)current)->ID;
				DLL_addto_next(&new->node, current);
				new->ID = 1;
				return 0;
			}
			else
			{
				while(current->next != NULL)
				{
					id_current = ((client_data *)current)->ID;
					id_next = ((client_data *)(current->next))->ID;
					if((id_next - id_current) > 1)
					{
						DLL_addto_next(&new->node, current);
						new->ID = id_current + 1;
						return 0;
					}
					else
					{
						current = current->next;
					}
				}
				id_current = ((client_data *)current)->ID;
				DLL_addto_next(&new->node, current);
				new->ID = id_current + 1;
				return 0;
			}
		}
	}
}


int release_all_client(dllNode_t * client)
{
	dllNode_t * current = NULL;

	while(client->next != NULL)
	{
		current = client->next;
		DLL_delete(client->next);
		close(((client_data *)current)->socket_fd);
		free(current);
	}
	free(client);
	return 0;
}

int client_leave(dllNode_t * client, int leave_pid)
{
	dllNode_t * current = client;

	while(current->next != NULL)
	{
		current = current->next;
		if(((client_data *)current)->pid == leave_pid)
		{
			close(((client_data *)current)->socket_fd);
			printf("%s leaved\n", ((client_data *)current)->ip);
			DLL_delete(current);
			free(current);
			current = NULL;
			//這裡要直接 return ，因為下一步while判斷式存取被free的地址會出錯
			return 0;
		}
	}
	return 0;
}


int get_client_socket_fd(dllNode_t * client, int client_pid)
{
	dllNode_t * current = client;

	while(current->next != NULL)
	{
		current = current->next;
		if(((client_data *)current)->pid == client_pid)
		{
			return ((client_data *)current)->socket_fd;
		}
	}
	return 0;
}




void client_leave_handler(int signal, siginfo_t *info, void *ctx)
{
	waitpid(info->si_pid, 0, WNOHANG);
	client_leave(client_list, info->si_pid);
}


void server_op_handler(int signal, siginfo_t *info, void *ctx)
{
	int client_fd;
	client_data * client_information;
	char who_table[] = "<ID>   <name>     <IP:port>                <indicate me>\n";
	char format_buf[100];

	if(info->si_value.sival_int == 0)
	{
		client_fd = get_client_socket_fd(client_list, info->si_pid);
		send(client_fd, who_table, sizeof(who_table), 0);
		dllNode_t * current = client_list;
		while(current->next != NULL)
		{
			current = current->next;
			client_information = (client_data *)current;
			if(client_fd == client_information->socket_fd)
			{
				sprintf(format_buf, "%4d   %s    %-15s:%5d    <-(me)", 
						client_information->ID,
						client_information->name,
						client_information->ip,
						client_information->port
						);
			}
			else
			{
				sprintf(format_buf, "%4d   %s    %-15s:%5d", 
						client_information->ID,
						client_information->name,
						client_information->ip,
						client_information->port
						);
			}
			send(client_fd, format_buf, strlen(format_buf), 0);
			send(client_fd, "\n", sizeof("\n"), 0);
		}
		send(client_fd, "% ", sizeof("% "), 0);
	}
	else if(info->si_value.sival_int == 1)
	{
		client_fd = get_client_socket_fd(client_list, info->si_pid);
		send(client_fd, "tell\n", sizeof("tell\n"), 0);
		send(client_fd, "% ", sizeof("% "), 0);
	}
	else if(info->si_value.sival_int == 2)
	{
		client_fd = get_client_socket_fd(client_list, info->si_pid);
		send(client_fd, "yell\n", sizeof("yell\n"), 0);
		send(client_fd, "% ", sizeof("% "), 0);
	}
	else if(info->si_value.sival_int == 3)
	{
		client_fd = get_client_socket_fd(client_list, info->si_pid);
		send(client_fd, "name\n", sizeof("name\n"), 0);
		send(client_fd, "% ", sizeof("% "), 0);
	}
}


void set_signal_child_terminate_action(void)
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = client_leave_handler;
	act.sa_flags = SA_SIGINFO;

	sigaction(SIGCHLD, &act, NULL);
}


void set_signal_server_op_action(void)
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = server_op_handler;
	act.sa_flags = SA_SIGINFO;

	sigaction(34, &act, NULL);
}

int main()
{
	int listen_fd = 0;
	int connect_fd = 0;
	int send_state;
	int child_pid;
	

	client_list = DLL_init();


	set_signal_child_terminate_action();
	set_signal_server_op_action();

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
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(1223);

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
			//perror("accept error\n");
			//exit(1);
		}
		else
		{
			inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
			client_port = client_addr.sin_port;
			printf("accept success from %s\n", client_ip);

			child_pid = fork();

			if(child_pid == -1)
			{
				perror("server fork error");
			}
			else if(child_pid == 0)
			{
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

					read(connect_fd, command_buffer, 256);
					char * cut = strstr(command_buffer, "\r\n");
					*cut = '\0';
					parse(command_buffer, count_list, connect_fd);
				}
			}
			else
			{


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

					client_add(new, client_list);
				}

			}
		}
	}

	
//	release_all_client(client_list);
	close(listen_fd);
	return 0;

}
