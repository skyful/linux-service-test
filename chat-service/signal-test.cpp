#include<signal.h>
#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<errno.h>
#include"wheel_timer.h"
#define EVENT_MAX_NUMBER 1024
#define USER_MAX_NUMBER  65536
#define CLIENT_TIMEOUT   30   /*�ͻ��˳�ʱ�Ͽ�ʱ�� ��λ S*/
#define ALARM_TICK       1    /* s */
static int pipefd[2];
static bool timer_active=false;
client_data user_data[USER_MAX_NUMBER];
wheel_timer wh_timer;
void setnonblocking(int fd)
{
	int old_option=fcntl(fd,F_GETFL);
	int new_option=old_option | O_NONBLOCK;
	fcntl(fd,new_option,F_SETFL);
}
void addfd(int epollfd,int fd)
{
	epoll_event event;
	event.data.fd=fd;
	event.events=EPOLLIN | EPOLLET;
	epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
	setnonblocking(fd);
}
/*�ͻ��˳�ʱ������*/
void timer_func(client_data*)
{
	if(client_data!=NULL)
	{
		printf("client %d close becase of timeout",client_data->sockfd);
		close(client_data->sockfd);
		wh_timer.del_timer(client_data->timer);
		client_data->timer=NULL;
	}
}
/*�źŴ�����*/
void sig_handle(int sig)
{
	/*����ԭ����errno*/
	int save_errno=errno;
	int msg=sig;
	send(pipefd[1],(char*)&msg,1,0);
	errno=save_errno;
}

/*����źŴ���ĺ���*/
void addsig(int sig)
{
	struct sigaction sa;
	memset(&sa,'\0',sizeof(sa));
	sa.sa_handler=sig_handle; 
	sa.sa_flags |=SA_RESTART;
	sigfillset(&sa.sa_mask);
	assert(sigaction(sig,&sa,NULL)!=-1);
	
}
int main(int argc ,char* argv[])
{
  if(argc<=2)
  {
	  printf("%s ip_address port_number\n",argv[0]);
	  return 0;
  }
  char* ip_address=argv[1];
  short port=atoi(argv[2]);
  struct sockaddr_in address;
  bzero(&address,sizeof(address));
  address.sin_family=AF_INET;
  address.sin_port=htons(port);
  inet_pton(AF_INET,ip_address,&address.sin_addr);

  int listenfd=socket(PF_INET,SOCK_STREAM,0);
  assert(listenfd>=0);

  int ret=bind(listenfd,(struct sockaddr*)(&address),sizeof(address));
  if(ret==-1)
  {
	  printf("bind errno:%d",errno);
	  return 1;
  }

  ret=listen(listenfd,5);
  assert(ret!=-1);

  epoll_event events[EVENT_MAX_NUMBER];
  int epollfd=epoll_create(5);
  assert(epollfd!=-1);
  addfd(epollfd,listenfd);
	
  /*ʹ��socketpaire�����ܵ���������pipe[0]�Ķ��¼�*/
  ret=socketpair(PF_UNIX,SOCK_STREAM,0,pipefd);
  assert(ret!=-1);
  setnonblocking(pipefd[1]);//д������źŴ������д������Ը�ΪΪ����ģʽ
  addfd(epollfd,pipefd[0]);
  /*����һЩ�źŵĴ�����*/
  addsig(SIGHUP);
  addsig(SIGCHLD);
  addsig(SIGTERM);
  addsig(SIGINT);
  addsig(SIGALRM);
  bool stop_server=false;
  alarm(ALARM_TICK);
  while(!stop_server)
  {
	  int number=epoll_wait(epollfd,events,EVENT_MAX_NUMBER,-1);
	  if(number<0 && errno!=EINTR)
	  {
		  printf("epoll_wait errno:%d\n",errno);
		  return 1;
	  }
	  for(int i=0;i<number;i++)
	  {
		 int socketfd=events[i].data.fd;
		 /*����������ļ���������listenfd�������µ�����*/
		 if(socketfd==listenfd)
		 {
			 struct sockaddr_in client_address;
			 socklen_t client_addrlen=sizeof(client_address);
			 int connfd=accept(socketfd,(struct sockaddr*)&client_address,&client_addrlen);
			 if(connfd>=0)
			 {
				 addfd(epollfd,connfd);
				 user_data[connfd].address=client_address;
				 user_data[connfd].sockfd=connfd;
				 user_data[connfd].tw_timer=wh_timer.add_timer(CLIENT_TIMEOUT,&user_data[connfd]);
				 if(user_data[connfd].timer!=NULL)
				{
					user_data[connfd].timer->cb_func=timer_func;
				}
			 }
			 
		 }
		 /*�����pipefd[0]�����ź�*/
		 else if(socketfd==pipefd[0] && events[i].events & EPOLLIN)
		 {
			 char signals[1024];
			 ret=recv(pipefd[0],signals,sizeof(signals),0);
			 if(ret<0)
			 {
				 continue;
			 }
			 else if(ret==0)
			 {
				 continue;
			 }
			 else
			 {
				 for(int j=0;j<ret;j++)
				 {
					 switch(signals[j])
					 {
					 case SIGHUP:
						 continue;
						break;
					 case SIGCHLD:
						 continue;
						 break;
					 case SIGTERM:
						 stop_server=true;	
						 break;
					 case SIGINT:
						 stop_server=true;
						 break;
					 case SIGALRM:
						 alarm(ALARM_TICK);
						 timer_active=true;
						 
						 break;
					 }
				 }
			 }
		 }
		 else
		 {
			memset(user_data[socketfd].buf,'\0',BUFFER_SIZE]);
			ret=recv(socketfd,user_data[socketfd].buf,BUFFER_SIZE-1,0);
			if(ret<0)
			{
				if(errno!=EAGAIN)
				{/*�ر����Ӳ�ɾ����ʱ��*/
					close(socketfd);
					wh_timer.del_timer(user_data[socketfd].timer);
					user_data[socketfd].timer=NULL;
				}
			}
			else if(ret==0)
			{
				printf( "code should not come to here\n" );
			}
			else
			{
				printf("recv from %d : %s",socketfd,user_data[socketfd].buf);
				/*�ȰѶ�ʱ����ʱ������ɾ�� ������µĶ�ʱ��*/
				wh_timer.del_timer(user_data[socketfd].timer);
				user_data[socketfd].timer=wh_timer.add_timer(CLIENT_TIMEOUT,&user_data[socketfd]);
				if(user_data[socketfd].timer!=NULL)
				{
					user_data[socketfd].timer->cb_func=timer_func;
				}
			}
		 }
	  }
	  if(timer_active)
	  {
		printf("TICK\n");
		wh_timer.tick();
		timer_active=false;
		
	  }
  }
  printf("close fds\n");
  close(listenfd);
  close(pipefd[0]);
  close(pipefd[1]);
  return 0;

}
