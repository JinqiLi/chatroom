#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 

#define portnumber 3333
#define MAX_CONNET_NUM 5

int main(int argc, char* argv[])
{
	int sockfd;
	int connect_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int sock_size;
	int nReadBytes;
	char buffer[1024];	

	//服务端建立sockfd描述符
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)   //AF_INET:IPV4;SOCK_STREAM:TCP
	{
   		fprintf(stderr, "Socket error:%s\n\a", strerror(errno));	
   		return -1; 
	}
	
	//填充sockaddr结构
	bzero(&server_addr, sizeof(struct sockaddr_in));	// 初始化,置0
	server_addr.sin_family = AF_INET;					// Internet
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// (将本机器上的long数据转化为网络上的long数据)和任何主机通信  //INADDR_ANY 表示可以接收任意IP地址的数据，即绑定到所有的IP
	//server_addr.sin_addr.s_addr=inet_addr("192.168.1.1");  //用于绑定到一个固定IP,inet_addr用于把数字加格式的ip转化为整形ip
	server_addr.sin_port = htons(portnumber);			// (将本机器上的short数据转化为网络上的short数据)端口号
	
	//捆绑sockfd描述符到ip地址
	if(bind(sockfd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) == -1)
	{
		fprintf(stderr, "bind error:%s\n\a", strerror(errno));
		return -1; 
	}
	
	//设置允许连接的客户端数
	if(listen(sockfd,MAX_CONNET_NUM) == -1)
	{
		fprintf(stderr, "listen error:%s\n\a", strerror(errno));
		return -1; 		
	}
	
	while(1)
	{
		//服务器阻塞，直到客户程序建立连接
		sock_size = sizeof(struct sockaddr_in);
		if( (connect_fd = accept(sockfd, (struct sockaddr*)(&client_addr), &sock_size)) == -1)
		{
			fprintf(stderr,"Accept error:%s\n\a",strerror(errno)); 
			return -1; 
		}
		fprintf(stderr, "server get connection from %s\n", inet_ntoa(client_addr.sin_addr));
		
		if(	(nReadBytes = read(connect_fd, buffer, 1024)) == -1)
		{
			fprintf(stderr,"Read Error:%s\n",strerror(errno)); 
			return -1; 
		}
		buffer[nReadBytes] = '\0';
		printf("server received %s\n", buffer);	
		
		//此次通讯结束
		close(connect_fd);
		//循环下一个
	}
	
	//通讯结束
	close(sockfd);
	exit(0);
}
