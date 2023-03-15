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


int parse(char * command, dllNode_t * count_list);

#endif
