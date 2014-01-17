#define _GNU_SOURCE 1
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>
#include<poll.h>
#include<string.h>
int main(int argc,char* argv[])
{
    if(argc<=2)
    {
        printf("usage:%s ip_address port_numner",basename(argv[0]));
        return 1;
    }
    char* ip=argv[1];
    int port=atoi(argv[2]);
    struct sockaddr_in service_addr;
    bzero(&service_addr,sizeof(service_addr));
    service_addr.sin_family=AF_INET;
    inet_pton(AF_INET,ip,&service_addr.sin_addr);
    service_addr.sin_port=htons(port);
    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0)
    {
        printf("create socker failed!\n");
        return 1;
    }
    if(connect(sockfd,(struct sockaddr*)&service_addr,sizeof(service_addr))<0)
    {
        printf("connect failed!\n");
        close(sockfd);
        return 1;
    }
    pollfd fds[2];
    /*注册描述符0（标准输入）和文件描述符 socket*/
    fds[0].fd=0;
	fds[0].events=POLLIN;
	fds[0].revents=0;
	fds[1].fd=sockfd;
	fds[1].events=POLLIN | POLLRDHUP;


}
