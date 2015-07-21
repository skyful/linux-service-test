#ifndef _MEMERYPOOL1_H
#define _MEMERYPOOL1_H
struct block{
	char* head;
	block* next;
	int   nSize;//һ���ڴ���С
}
struct list{//һ�������д����ͬ��С���ڴ��
	block* free;//�����ڴ������
	block* used;//ʹ�õ��ڴ������
	list* next;//
	int   nSize;//�ڴ������С
}

struct pool{
	list* list_head;
	list* list_next;
	int nSize;//�ڴ���ܴ�С
}
/*
* �����ڴ�
* nSize�����ڴ��С
* ����ֵ���ڴ�ָ��
*/
block* allocMem(int nSize);

/*
* �ͷ��ڴ�
* ptr�ڴ��ָ��
*/
void freeMem(blockr* ptr);
#endif
