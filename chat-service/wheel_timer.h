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
	int rotation;/*定时器在时间轮上多少圈后生效*/
	int time_slot;/*定时器在时间轮上的第几个槽*/
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
	* 根据超时时间添创建一个定时器并插入到时间轮的合适位置
	************************************/
	tw_timer* add_timer(int timeout,client_data* user_data)
	{
		if(timeout<0)
			return NULL;
		else if(timeout<TI)/*如果超时时间小于槽间隔，则定时器在第一个槽生效*/
			ticks=1;
		else
			ticks=timeout/TI;
		int rotation=ticks/N;/*定时器在时间轮转多少圈后生效*/
		int ts=(cur_slot+ticks%N)%N;/*定时器在时间轮第几个槽*/	
		tw_timer* timer= new tw_timer(rotation,ts);
		timer->user_data=user_data;
		if(slots[ts]==NULL)/*如果此时间槽中没有任何定时器*/
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
		{/*如果在链表头*/
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
			{/*如果定时器的轮询圈数>0表示定时器没有触发*/
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
	static const N=60;/*时间轮中一共有多少个时间槽*/
	static onsst TI=1;/*每隔1S时间轮转动一次 即槽间隔*/
	tw_timer* slots[N];/*每个槽的链表头节点*/
	int   cur_slot;
}
#endif
