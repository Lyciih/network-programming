#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

int main()
{
	int client_fd = 0;
	char read_buf[4096];
	char write_buf[4096];
	int child_pid;
	

	//建立 socket
	if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("socket error\n");
	}
	else
	{
		printf("socket success %d\n", client_fd);
	}

	//填 server 資料
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(1223);
	inet_pton(AF_INET, "59.127.115.95", &server_addr.sin_addr);

	//connect
	if(connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("connect error \n");
		printf("%d", client_fd);
	}
	else
	{
		printf("connect success\n");

		child_pid = fork();

		if(child_pid == -1)
		{
			perror("client fork error : ");
		}
		else if(child_pid == 0)
		{
			while(1)
			{
				fgets(write_buf, 4096, stdin);
				send(client_fd, write_buf, sizeof(write_buf), 0);
			}
		}
		else
		{
			while(1)
			{
				read(client_fd, read_buf, 4096);
				printf("%s", read_buf);
				if(*read_buf == 'b' && *(read_buf + 1) == 'y' && *(read_buf + 2) == 'e' && (*(read_buf + 3) == ' ' || *(read_buf + 3) == '\r' || *(read_buf + 3) == '\n'))
				{
					kill(child_pid, 9);
					printf("leave\n");
					break;
				}
				memset(read_buf, 0, 1);
			}
		}
	}
	close(client_fd);
	return 0;
}
