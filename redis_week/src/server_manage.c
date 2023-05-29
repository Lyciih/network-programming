#include "network-programming.h"

extern dllNode_t * client_list;
extern redisContext * pContext;
extern redisReply * reply;
extern int listen_fd;
extern char prompt[30];
extern char change_name_temp[20];
extern char name_temp[20];




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
				return 1;
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
						return id_current + 1;
					}
					else if((id_next - id_current) == 1)
					{
						current = current->next;
					}
					else
					{
						printf("ID apply has error or repeat\n");
					}
				}
				id_current = ((client_data *)current)->ID;
				DLL_addto_next(&new->node, current);
				new->ID = id_current + 1;
				return id_current + 1;
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
		close(((client_data *)current)->server_op_pipe);
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
			close(((client_data *)current)->server_op_pipe);
			printf("%s : %d leaved\n", ((client_data *)current)->ip, ((client_data *)current)->port);
			DLL_delete(current);
			free(current);
			current = NULL;
			//這裡要直接 return ，因為下一步while判斷式存取被free的地址會出錯
			return 0;
		}
	}
	return 0;
}


client_data * get_client_data(dllNode_t * client, int client_pid)
{
	dllNode_t * current = client;

	while(current->next != NULL)
	{
		current = current->next;
		if(((client_data *)current)->pid == client_pid)
		{
			return (client_data *)current;
		}
	}
	return 0;
}




void client_leave_handler(int signal, siginfo_t * info, void *ctx)
{
	waitpid(info->si_pid, 0, WNOHANG);
	client_leave(client_list, info->si_pid);
}

void name_handler(siginfo_t *info)
{
	client_data * client_information = NULL;
	client_data * other_client_information = NULL;
	int client_fd;
	char name_buf[20];
	memset(name_buf, 0, 20);
	char format_buf[43];
	char prompt[40];
	char old_name[20];

	client_information = get_client_data(client_list, info->si_pid);
	strcpy(old_name, client_information->name);	
	client_fd = client_information->socket_fd;
	read(client_information->server_op_pipe, name_buf, sizeof(client_information->name));

	dllNode_t * current = client_list;

	while(current->next != NULL)
	{
		current = current->next;
		other_client_information = (client_data *)current;
		if(strcmp(name_buf, other_client_information->name) == 0)
		{
			sprintf(format_buf, "User %s already exists !\n", name_buf);
			send(client_fd, format_buf, strlen(format_buf), 0);
			send(client_fd, prompt, strlen(prompt), 0);
			return;
		}
	}

	memset(&(client_information->name), 0, sizeof(client_information->name));
	strcpy(client_information->name, name_buf);
	send(client_fd, "name change accept!\n", sizeof("name change accept!\n"), 0);
	
	client_information = get_client_data(client_list, info->si_pid);
	sprintf(prompt, "(%s)%% ", client_information->name);	
	send(client_fd, prompt, strlen(prompt), 0);

	sigval_t server_op;
	server_op.sival_int = 4;
	sigqueue(info->si_pid, 34, server_op);
	
	reply = redisCommand(pContext, "rename client:%s client:%s", old_name, client_information->name);
	reply = redisCommand(pContext, "rename mailbox:%s mailbox:%s", old_name, client_information->name);
	reply = redisCommand(pContext, "get mailbox:%s", client_information->name);
	int mail_count = atoi(reply->str);
	for(int i = 1; i <= mail_count; i++)
	{
		reply = redisCommand(pContext, "rename mail:%s:%d mail:%s:%d", old_name, i, client_information->name, i);
	}
}



void who_handler(siginfo_t *info)
{
	int client_fd;
	client_data * client_information = NULL;
	char who_table[] = "<ID>   <name>             <IP:port>                <indicate me>\n";
	char format_buf[69];
	char prompt[40];

	client_fd = get_client_data(client_list, info->si_pid)->socket_fd;
	sprintf(prompt, "(%s)%% ", get_client_data(client_list, info->si_pid)->name);	
	send(client_fd, who_table, sizeof(who_table), 0);
	dllNode_t * current = client_list;
	
	while(current->next != NULL)
	{
		current = current->next;
		client_information = (client_data *)current;
		if(client_fd == client_information->socket_fd)
		{
			sprintf(format_buf, "%4d   %-15s    %-15s:%5d    <-(me)\n", 
					client_information->ID,
					client_information->name,
					client_information->ip,
					client_information->port
					);
		}
		else
		{
			sprintf(format_buf, "%4d   %-15s    %-15s:%5d\n", 
					client_information->ID,
					client_information->name,
					client_information->ip,
					client_information->port
					);
		}
		send(client_fd, format_buf, strlen(format_buf), 0);
	}


	send(client_fd, prompt, strlen(prompt), 0);
}


void yell_handler(siginfo_t *info)
{
	client_data * client_information = NULL;
	client_data * other_client_information = NULL;
	char message_buf[251];
	memset(message_buf, 0, 251);
	char format_buf[284];
	char prompt[40];

	client_information = get_client_data(client_list, info->si_pid);
	read(client_information->server_op_pipe, message_buf, 251);


	sprintf(format_buf, "<%s yelled>: %s\n",
			client_information->name, 
			message_buf
			);

	dllNode_t * current = client_list;

	while(current->next != NULL)
	{
		current = current->next;
		other_client_information = (client_data *)current;
		send(other_client_information->socket_fd, format_buf, strlen(format_buf), 0);
		sprintf(prompt, "(%s)%% ", other_client_information->name);
		send(other_client_information->socket_fd, prompt, strlen(prompt), 0);
	}
	return;
}


void login_handler(siginfo_t *info)
{
	client_data * client_information = NULL;
	char name_buf[15];
	memset(name_buf, 0, 15);

	client_information = get_client_data(client_list, info->si_pid);
	read(client_information->server_op_pipe, name_buf, sizeof(client_information->name));
	memset(&(client_information->name), 0, sizeof(client_information->name));
	strcpy(client_information->name, name_buf);
}


void child_name_handler(siginfo_t *info)
{
	strcpy(name_temp, change_name_temp);
	sprintf(prompt, "(%s)%% ", change_name_temp);
}


void gyell_handler(siginfo_t *info)
{
	char message_buf[251];
	memset(message_buf, 0, 251);
	char format_buf[284];
	char other_format_buf[284];
	char prompt[40];

	client_data * client_information = NULL;
	client_information = get_client_data(client_list, info->si_pid);
	read(client_information->server_op_pipe, message_buf, 251);
	
	reply = redisCommand(pContext, "get %s:gyell:target", client_information->name);

	sprintf(format_buf, "<%s-%s>: %s\n",
			reply->str,
			client_information->name, 
			message_buf
			);
	
	sprintf(other_format_buf, "\n<%s-%s>: %s\n",
			reply->str,
			client_information->name, 
			message_buf
			);

	reply = redisCommand(pContext, "hkeys group:%s", reply->str);
	for(int i = 1; i < reply->elements; i++)
	{
		dllNode_t * current = client_list;
		
		while(current->next != NULL)
		{
			current = current->next;
			client_information = (client_data *)current;
			if(strcmp(reply->element[i]->str, client_information->name) == 0)
			{
				sprintf(prompt, "(%s)%% ", client_information->name);
				send(client_information->socket_fd, format_buf, strlen(format_buf), 0);
				send(client_information->socket_fd, prompt, strlen(prompt), 0);
			}
		}
	}
}

void server_op1_handler(int signal, siginfo_t *info, void *ctx)
{
	if(info->si_value.sival_int == 0)
	{
		who_handler(info);
	}
	else if(info->si_value.sival_int == 1)
	{
		yell_handler(info);
	}
	else if(info->si_value.sival_int == 2)
	{
		name_handler(info);
	}
	else if(info->si_value.sival_int == 3)
	{
		login_handler(info);
	}
	else if(info->si_value.sival_int == 4)
	{
		child_name_handler(info);
	}
	else if(info->si_value.sival_int == 5)
	{
		gyell_handler(info);
	}

}


void server_op2_handler(int signal, siginfo_t *info, void *ctx)
{
	int target_ID = info->si_value.sival_int;
	
	client_data * client_information = NULL;
	int client_fd;
	char message_buf[251];
	memset(message_buf, 0, 251);
	char format_buf[284];
	char prompt[40];
	char self_prompt[40];


	client_information = get_client_data(client_list, info->si_pid);
	client_fd = client_information->socket_fd;
	read(client_information->server_op_pipe, message_buf, 251);

	sprintf(format_buf, "<%s told you>: %s\n",
			client_information->name, 
			message_buf
			);

	sprintf(self_prompt, "(%s)%% ", client_information->name);

	dllNode_t * current = client_list;
	
	while(current->next != NULL)
	{
		current = current->next;
		client_information = (client_data *)current;
		if(target_ID == client_information->ID)
		{
			sprintf(prompt, "(%s)%% ", client_information->name);
			send(client_information->socket_fd, format_buf, strlen(format_buf), 0);
			send(client_information->socket_fd, prompt, strlen(prompt), 0);
			
			send(client_fd, "send accept!\n", sizeof("send accept!\n"), 0);
			send(client_fd, self_prompt, strlen(prompt), 0);
			return;
		}
	}	
	send(client_fd, "no this ID\n", sizeof("no this ID\n"), 0);
	send(client_fd, self_prompt, strlen(self_prompt), 0);
	return;
}


void set_signal_child_terminate_action(void)
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = client_leave_handler;
	act.sa_flags = SA_SIGINFO;

//   2023.4.25 的實驗結果:當註冊在SIGCHLD時，若有很多人同時離開(比如tmux同步模式)，signal會有漏接的情況，因此也註冊在可靠信號上
	//sigaction(SIGCHLD, &act, NULL);
	sigaction(36, &act, NULL);
}



void server_shutdown_handler(int signal, siginfo_t *info, void *ctx)
{

	release_all_client(client_list);
	close(listen_fd);
	freeReplyObject(reply);
	redisFree(pContext);
	printf("exit\n");
	exit(0);
}



void set_signal_server_op1_action(void)
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = server_op1_handler;
	act.sa_flags = SA_SIGINFO;

	sigaction(34, &act, NULL);
}


void set_signal_server_op2_action(void)
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = server_op2_handler;
	act.sa_flags = SA_SIGINFO;

	sigaction(35, &act, NULL);
}

void set_signal_server_shutdown_action(void)
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = server_shutdown_handler;
	act.sa_flags = SA_SIGINFO;

	sigaction(SIGINT, &act, NULL);
}
