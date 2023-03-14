#include "network-programming.h"

int main()
{
	char buffer[5000];


	char main_read_buf[1024] = {0};
	char main_write_buf[1024] = {0};

	while(1)
	{
		printf("%%");
		fgets(buffer, 5000, stdin);
		parse(buffer);
	}
}
