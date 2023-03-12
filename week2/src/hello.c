#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include "my.h"


#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif


int main(int argc, char *argv[])
{
	int inputFd, outputFd, openFlags;
	mode_t filePerms;
	ssize_t numRead;
	char buf[BUF_SIZE];
	

	inputFd = open(argv[1], O_RDONLY);
	if(inputFd == -1)
	{
		printf("%s opening file %s\n", strerror(errno), argv[1]);
		exit(EXIT_FAILURE);
	}

	
	openFlags = O_CREAT | O_WRONLY | O_TRUNC;
	filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;




	outputFd = open(argv[2], openFlags, filePerms);
	if(inputFd == -1)
	{
		printf("%s opening file %s\n", strerror(errno), argv[2]);
		exit(EXIT_FAILURE);
	}


	printf("fd1 = %d, fd2 = %d\n", inputFd, outputFd);


	while((numRead = read(inputFd, buf, BUF_SIZE)) > 0)
	{
		if(write(outputFd, buf, numRead) != numRead)
		{
			perror("write() returned error or partial write occurred");
		}
	}

	if(numRead == -1)
	{
		perror("read");
	}




	if(close(inputFd) == -1) 
	{
		perror("close input");
	}

	if(close(outputFd) == -1) 
	{
		perror("close output");
	}
	
	exit(EXIT_SUCCESS); 
}
