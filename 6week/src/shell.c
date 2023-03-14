#include "network-programming.h"

int main()
{
	setenv("PATH", "bin:.", 1);
	char command_buffer[256];

	while(1)
	{
		printf("%% ");
		fgets(command_buffer, 256, stdin);
		parse(command_buffer);
	}
}
