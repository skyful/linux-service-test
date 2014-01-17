#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#define MY_PORT 8888
int main(int argc,char **argv)
{
   int listen_fd,accept_fd;
   struct sockaddr_in client_addr;
   int n;
   int res=0;
   printf("service start\n");
   if((listen_fd=socket(AF_INET,SOCK_STREAM,0))<0)
   {
      printf("socket error:%s\n\a",strerror(errno));
      exit(1);
   }
   bzero(&client_addr,sizeof(struct sockaddr_in));
   client_addr.sin_family=AF_INET;
   client_addr.sin_port=htons(MY_PORT);
   client_addr.sin_addr.s_addr=htonl(INADDR_ANY);
   n=1;
   /*如果服务器终止后。可以第二次快速启动，不需要等待*/
   setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&n,sizeof(int));
   res=bind(listen_fd,(struct sockaddr*)&client_addr,sizeof(client_addr));
   if(res<0)
   {
      printf("bind error:%s\n\a",strerror(errno));
      exit(1);
   }
   listen(listen_fd,5);
   printf("service start listen!\n");
   while(1)
   {
     accept_fd=accept(listen_fd,NULL,NULL);
	 printf("accept sockfd %d\n",accept_fd);
     if((accept_fd<0)&&(errno==EINTR))
     {
        continue;
     }
     else if(accept_fd<0)
     {
     	printf("accept error:%s\n\a",strerror(errno));
	continue;
     }
     if((n=fork())==0)
     {
        //子进程处理客户端的链接 
	char buffer[1024];
	close(listen_fd);
	n=read(accept_fd,buffer,1024);
	if(n<(sizeof(buffer)-1))
	    buffer[n]='\0';
	printf("recv data: %d %s\n",n,buffer);
	write(accept_fd,buffer,n);
	close(accept_fd);
	exit(0);
     }
     else if(n<0)
     {
        printf("fork error:%s\n\a",strerror(errno));
     }
     close(accept_fd);
   }

}

