#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 

#define portnumber 3333

int main(int argc, char* argv[])
{
        int sockfd;
        char buffer[1024];
        struct sockaddr_in server_addr;
        struct hostent *host;
        
        //使用hostname查询host名字
        if(argc!=2) 
        { 
                fprintf(stderr,"Usage:%s hostname \a\n",argv[0]); 
                return -1; 
        } 
        
        if((host = gethostbyname(argv[1])) == NULL)
        { 
                fprintf(stderr,"Gethostname error\n"); 
                return -1; 
        } 
        
        //客户端程序开始建立socket描述符
        if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
                fprintf(stderr,"Socket Error:%s\a\n",strerror(errno)); 
                return -1; 
        }
        
        //客户程序填充服务端信息
        bzero(&server_addr, sizeof(server_addr));    //初始化，置0
        server_addr.sin_family = AF_INET;                   //IPV4
        server_addr.sin_port = htons(portnumber);       //端口号，将本机上的的short数据转换为网络字节序
        server_addr.sin_addr = *((struct in_addr *)host->h_addr); //ip地址
        
        //客户端发起连接请求
        if(connect(sockfd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) == -1)
        {
                fprintf(stderr,"Connect Error:%s\a\n",strerror(errno)); 
                return -1;      
        }
        
        //连接成功
        printf("pls input char:\n");
        
        //发送数据
        fgets(buffer, 1024, stdin);
        write(sockfd, buffer, strlen(buffer));
        
        //通讯结束
        close(sockfd);
        return 1;

}