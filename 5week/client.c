#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
	int client_fd = 0;
	

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
	}



	close(client_fd);
	return 0;
}
