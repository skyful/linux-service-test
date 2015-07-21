#ifndef _MEMERYPOOL1_H
#define _MEMERYPOOL1_H
struct block{
	char* head;
	block* next;
	int   nSize;//一个内存块大小
}
struct list{//一个链表中存放相同大小的内存块
	block* free;//空闲内存块链表
	block* used;//使用的内存块链表
	list* next;//
	int   nSize;//内存链表大小
}

struct pool{
	list* list_head;
	list* list_next;
	int nSize;//内存池总大小
}
/*
* 申请内存
* nSize申请内存大小
* 返回值：内存指针
*/
block* allocMem(int nSize);

/*
* 释放内存
* ptr内存块指针
*/
void freeMem(blockr* ptr);
#endif
