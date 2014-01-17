#ifndef __WHEEL_TIMER__
#define __WHEEL_TIMER__
#include <time.h>
#include <netinet/io.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
class tw_timer;
struct client_data
{
	sockaddr_in address;
	int         sockfd;
	char	    buf[BUFFER_SIZE];
	tw_timer*   timer;
}
class tw_timer
{
public:
	tw_timer(int rot,int ts)
	:next(NULL),prev(NULL),rotation(rot),time_slot(ts){}
public:
	int rotation;/*��ʱ����ʱ�����϶���Ȧ����Ч*/
	int time_slot;/*��ʱ����ʱ�����ϵĵڼ�����*/
	void (*cb_func)(client_data*);
	client_data* user_data;
	tw_timer* next;
	tw_timer* prev;
}

class wheel_timer
{
public:
	wheel_timer():cur_slot(0)
	{
		for(int i=0;i<N;i++)
		{
			slots[i]=NULL;
		}
	}
	~wheel_timer()
	{
		for(int i=0;i<N;i++)
		{
			tw_timer* tmp=slots[i];
			while(tmp!=NULL)
			{
				slots[i]=tmp->next;
				delete tmp;
				tmp=slots[i];
				
			}
		}
	}
	
	/************************************
	* ���ݳ�ʱʱ������һ����ʱ�������뵽ʱ���ֵĺ���λ��
	************************************/
	tw_timer* add_timer(int timeout,client_data* user_data)
	{
		if(timeout<0)
			return NULL;
		else if(timeout<TI)/*�����ʱʱ��С�ڲۼ������ʱ���ڵ�һ������Ч*/
			ticks=1;
		else
			ticks=timeout/TI;
		int rotation=ticks/N;/*��ʱ����ʱ����ת����Ȧ����Ч*/
		int ts=(cur_slot+ticks%N)%N;/*��ʱ����ʱ���ֵڼ�����*/	
		tw_timer* timer= new tw_timer(rotation,ts);
		timer->user_data=user_data;
		if(slots[ts]==NULL)/*�����ʱ�����û���κζ�ʱ��*/
		{
			slots[ts]=timer;
			slots[ts]->next=NULL;
			slots[ts]->prev=NULL;
		}
		else
		{
			timer->next=slots[ts];
			timer->prev=NULL;
			slots[ts]=timer;
		}
		return timer;
			
	}
	void del_timer(tw_timer* timer)
	{
		if(timer==NULL)
			return;
		int ts=timer->time_slot;
		if(timer==slots[ts])
		{/*���������ͷ*/
			slots[ts]=slots[ts]->next;
			if(slots[ts])
			{
				slots[ts]->prev=NULL;
			}
			delete timer;
		}
		else
		{
			timer->prev->next=timer->next;
			if(timer->next)
			{
				timer->next->prev=timer->prev;
			}
			delete timer;
		}
	}
	void tick()
	{
		tw_timer* tmp=slots[cur_slot];
		while(tmp)
		{
			if(tmp->rotation>0)
			{/*�����ʱ������ѯȦ��>0��ʾ��ʱ��û�д���*/
				tmp->rotation--;
				tmp=tmp->next;
			}
			else
			{
				tmp->cb_func(tmp->user_data);
				tw_timer* tmp2=tmp->next;
				del_timer(tmp);
				tmp=tmp2;
			}
		}
		cur_slot=(++cur_slot) % N;
	}
private:
	static const N=60;/*ʱ������һ���ж��ٸ�ʱ���*/
	static onsst TI=1;/*ÿ��1Sʱ����ת��һ�� ���ۼ��*/
	tw_timer* slots[N];/*ÿ���۵�����ͷ�ڵ�*/
	int   cur_slot;
}
#endif
