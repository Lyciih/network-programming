#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>		//close()
#include <string.h>		//memset()
#include <sys/socket.h>
#include <netinet/in.h>		//sockaddr_in
#include <arpa/inet.h>

int main()
{
	int listen_fd = 0;
	int connect_fd = 0;

	//申請用來 listen 的socket
	if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error\n");
		exit(1);
	}
	else
	{
		printf("create listen socket success\n");
	}


	//用來填 server 地址的結構
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(1223);

	//bind
	if((bind(listen_fd,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0))
	{
		perror("bind error\n");
		exit(1);
	}
	else
	{
		printf("bind success\n");
	}


	//listen
	if((listen(listen_fd, 5) < 0))
	{
		perror("listen error\n");
		exit(1);
	}
	else
	{
		printf("listen success\n");
	}


	//accept
	socklen_t len;
	struct sockaddr_in client_addr;
	len = sizeof(client_addr);
	char client_ip[INET_ADDRSTRLEN];

	if(connect_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &len) < 0)
	{
		perror("accept error\n");
		exit(1);
	}
	else
	{
		inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
		printf("accept success %s\n", client_ip);
		close(connect_fd);
	}

	
	close(listen_fd);
	return 0;

}
