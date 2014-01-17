#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>
#include<sys/socket.h>
#include<netinet/in.h>
#define SPORT 8888
int main(int  argc,char **argv)
{
    int sock_fd;
    struct sockaddr_in s_addr;
    char buf[]="this is socke test!";
    char recvbuf[1024];
    int bytes;
    sock_fd=socket(AF_INET,SOCK_STREAM,0);
    if(-1==sock_fd)
    {
    	printf("socket failed.\n");
	return -1;
    }
    printf("socket success.\n");
    s_addr.sin_family=AF_INET;
    s_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    s_addr.sin_port=htons(SPORT);
    if(-1==connect(sock_fd,(struct sockaddr*)(&s_addr),sizeof(s_addr)))
    {
    	printf("connect err.\n");
	return -1;
    }
    printf("connect success.\n");
    if(-1==(bytes=write(sock_fd,buf,sizeof(buf))))
    {
    	printf("send err.\n");
	return -1;
    }
    printf("send success len:%d\n",bytes);
    if(-1==(bytes=read(sock_fd,recvbuf,sizeof(recvbuf))))
    {
    	printf("recv err.\n");
	return -1;
    }
    if(bytes<(sizeof(recvbuf)-1))
    	recvbuf[bytes]='\0';
    printf("recv success %d:%s.\n",bytes,recvbuf);
    close(sock_fd);

}
