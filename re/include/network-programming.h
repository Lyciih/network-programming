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


typedef struct number_temp{
	dllNode_t node;
	int count;
	char * temp;
}number_temp;

int count_list_update(dllNode_t * count_list);
number_temp * count_list_check(dllNode_t * count_list);
int parse(char * command, dllNode_t * count_list);
int command_need_fork(char * command, size_t command_len, char * arg[], char * buffer, char * save_p, dllNode_t * count_list);
#endif
