#ifndef NETWORK_PROGRAMMING_H_INCLUDE
#define NETWORK_PROGRAMMING_H_INCLUDE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <dllSpec.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>



//儲存 client 相關資訊的結構
typedef struct client_data{
	dllNode_t node;
	int ID;
	char name[30];
	char ip[20];
	unsigned short port;
	int pid;
	int socket_fd;

}client_data;


//暫存編號管道輸出的結構
typedef struct number_temp{
	dllNode_t node;
	int count;
	char * temp;
}number_temp;


int count_list_update(dllNode_t * count_list, int first_time);

number_temp * count_list_check(dllNode_t * count_list);

int parse(char * command, dllNode_t * count_list, int connect_fd);

int command_need_fork(char * command, size_t command_len, char * arg[], char * buffer, char * save_p, dllNode_t * count_list);
#endif
