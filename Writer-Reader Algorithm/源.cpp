#include "windows.h"
#include <conio.h>
#include <stdlib.h>
#include <fstream>	//#include <fstream.h>
#include <io.h>
#include <string.h>
#include <stdio.h>
using namespace std;

#define READER 'R'	//读者
#define WRITER 'W'	//写者
#define INTE_PER_SEC 1000	//每秒时钟中断的数目
#define MAX_THREAD_NUM 63	//最大线程数
#define MAX_FILE_NUM 32	//最大文件数目数
#define MAX_STR_LEN 32	//字符串的长度
int readcount = 0;
int writecount = 0;
CRITICAL_SECTION RP_Write;	//临界资源
CRITICAL_SECTION cs_Write;
CRITICAL_SECTION cs_Read;


struct ThreadInfo
{
	int serial;	//线程序号
	int entity;	//线程类别（判断是读者还是写者）
	double delay;	//线程延迟时间
	double persist;	//线程读写操作时间
};


//读者优先————读者线程
//P: 读者线程信息
void RP_ReaderThread(void *p)
{
	//互斥变量
	HANDLE h_Mutex;
	h_Mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "mutex_for_readcount");
	DWORD wait_for_mutex;	//等待互斥变量所有权
	DWORD m_delay;	//延迟时间
	DWORD m_persist;	//读文件持续时间
	int m_serial;	//线程序列号
	//从参数中获得信息
	m_serial = ((ThreadInfo*)(p))->serial;
	m_delay = (DWORD)(((ThreadInfo*)(p))->delay *INTE_PER_SEC);
	m_persist = (DWORD)(((ThreadInfo*)(p))->persist *INTE_PER_SEC);
	Sleep(m_delay);	//延迟等待
	printf("读线程 %d 发出读请求。\n", m_serial);
	//等待互斥信号，保证对 ReadCount 的访问，修改互斥
	wait_for_mutex = WaitForSingleObject(h_Mutex, -1);
	//读者数量增加
	readcount++;
	if (readcount==1)
	{
		//第一个读者，等待资源
		EnterCriticalSection(&RP_Write);
	}
	ReleaseMutex(h_Mutex);	//释放互斥信号
	//读文件
	printf("读线程 %d 开始读文件。\n", m_serial);
	Sleep(m_persist);
	//退出线程
	printf("读线程 %d 结束读文件。\n", m_serial);
	//等待互斥信号，保证对 ReadCount 的访问，修改互斥
	wait_for_mutex = WaitForSingleObject(h_Mutex, -1);
	//读者数量减少
	readcount--;
	if (readcount==0)
	{
		LeaveCriticalSection(&RP_Write);	//如果所有读者读完，唤醒写者
	}
	ReleaseMutex(h_Mutex);	//释放信号量
}


//读者优先————写者进程
//P: 写者信息
void RP_WriterThread(void *p)
{

}



int main()
{
	return 0;
}