#ifndef NETWORK_PROGRAMMING_H_INCLUDE
#define NETWORK_PROGRAMMING_H_INCLUDE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <dllSpec.h>
#include "hiredis.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>



//儲存 client 相關資訊的結構
typedef struct client_data{
	dllNode_t node;
	int ID;
	char name[20];
	char ip[20];
	unsigned short port;
	int pid;
	int socket_fd;
	int server_op_pipe;
}client_data;


//暫存編號管道輸出的結構
typedef struct number_temp{
	dllNode_t node;
	int count;
	char * temp;
}number_temp;

//server 管理
int client_add(client_data * new, dllNode_t * client);

int release_all_client(dllNode_t * client);

int client_leave(dllNode_t * client, int leave_pid);

client_data * get_client_data(dllNode_t *client, int client_pid);

void client_leave_handler(int signal, siginfo_t * info, void * ctx);

void name_handle(siginfo_t * info);

void who_handle(siginfo_t * info);

void yell_handle(siginfo_t * info);

void server_op1_handler(int signal, siginfo_t * info, void * ctx);

void server_op2_handler(int signal, siginfo_t * info, void * ctx);

void set_signal_child_terminate_action(void);

void set_signal_server_op1_action(void);

void set_signal_server_op2_action(void);

void set_signal_server_shutdown_action(void);

int count_list_update(dllNode_t * count_list, int first_time);

//命令處理
int count_list_update(dllNode_t * count_list, int first_time);

number_temp * count_list_check(dllNode_t * count_list);

int parse(char * command, dllNode_t * count_list, int connect_fd, int server_op_pipe);

int command_need_fork(char * command, size_t command_len, char * arg[], char * buffer, char * save_p, dllNode_t * count_list);
#endif
