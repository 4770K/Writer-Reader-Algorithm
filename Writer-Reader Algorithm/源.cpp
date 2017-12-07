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
	DWORD m_delay;	//延迟时间
	DWORD m_persist;	//写文件持续时间
	int m_serial;	//线程序号
	//从参数中获得信息
	m_serial = ((ThreadInfo*)(p))->serial;
	m_delay = (DWORD)(((ThreadInfo*)(p))->delay *INTE_PER_SEC);
	m_persist = (DWORD)(((ThreadInfo*)(p))->persist *INTE_PER_SEC);
	Sleep(m_delay);
	printf("写进程 %d 发出请求 *** \n", m_serial);
	//等待资源
	EnterCriticalSection(&RP_Write);
	//写文件
	printf("写进程 %d 开始写文件。\n", m_serial);
	Sleep(m_persist);
	//退出进程
	printf("写进程 %d 结束写文件。\n", m_serial);
	//释放资源
	LeaveCriticalSection(&RP_Write);
}


void ReaderPriority(char *file)
{
	DWORD n_thread = 0;	//线程数目
	DWORD thread_ID;	//线程 ID
	DWORD wait_for_all;	//等待所有线程结束
	//互斥对象
	HANDLE h_Mutex;
	h_Mutex = CreateMutex(NULL, FALSE, "mutex_for_readcount");
	//线程对象的数组
	HANDLE h_Thread[MAX_THREAD_NUM];
	ThreadInfo thread_info[MAX_THREAD_NUM];
	readcount = 0;	//初始化 readcount
	InitializeCriticalSection(&RP_Write);	//初始化临界区

	printf("读者优先：\n\n");

	thread_info[0].serial = 1;
	thread_info[0].entity = 'W';
	thread_info[0].delay = 2;
	thread_info[0].persist = 3;
	thread_info[1].serial = 2;
	thread_info[1].entity = 'R';
	thread_info[1].delay = 3;
	thread_info[1].persist = 6;
	thread_info[2].serial = 3;
	thread_info[2].entity = 'R';
	thread_info[2].delay = 4;
	thread_info[2].persist = 2;
	thread_info[3].serial = 4;
	thread_info[3].entity = 'W';
	thread_info[3].delay = 6;
	thread_info[3].persist = 5;
	thread_info[4].serial = 5;
	thread_info[4].entity = 'R';
	thread_info[4].delay = 7;
	thread_info[4].persist = 2;
	n_thread = 5;

	//thread_info[0].serial = 1;
	//thread_info[0].entity = 'R';
	//thread_info[0].delay = 3;
	//thread_info[0].persist = 5;
	//thread_info[1].serial = 2;
	//thread_info[1].entity = 'W';
	//thread_info[1].delay = 4;
	//thread_info[1].persist = 5;
	//thread_info[2].serial = 3;
	//thread_info[2].entity = 'R';
	//thread_info[2].delay = 5;
	//thread_info[2].persist = 2;
	//thread_info[3].serial = 4;
	//thread_info[3].entity = 'R';
	//thread_info[3].delay = 6;
	//thread_info[3].persist = 5;
	//thread_info[4].serial = 5;
	//thread_info[4].entity = 'W';
	//thread_info[4].delay = 5.1;
	//thread_info[4].persist = 3;
	//n_thread = 5;
	//ifstream inFile;
	//inFile.open(file, ios::_Noreplace);
	//if (inFile.rdstate() == ios::failbit)
	//{
	//	printf("打开文件\"%s\"失败！请将\"%s\"放在程序目录下。\n", file, file);
	//	return;
	//}
	//printf("读者优先：\n\n");
	//while (inFile)
	//{
	//	//读入每一个读者、写者信息
	//	inFile >> thread_info[n_thread].serial;
	//	inFile >> thread_info[n_thread].entity;
	//	inFile >> thread_info[n_thread].delay;
	//	inFile >> thread_info[n_thread++].persist;
	//	//inFile.get();
	//}

	for (int i = 0; i < (int)(n_thread); i++)
	{
		if (thread_info[i].entity == READER || thread_info[i].entity == 'r')
		{
			h_Thread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(RP_ReaderThread), &thread_info[i], 0, &thread_ID);	//创建读者进程
		}
		else
		{
			h_Thread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(RP_WriterThread), &thread_info[i], 0, &thread_ID);	//创建写者进程
		}
	}

	//等待所有的线程结束
	wait_for_all = WaitForMultipleObjects(n_thread, h_Thread, TRUE, -1);
	printf("所有的读写线程结束操作。\n");
}


int main(int argc, char *argv[])
{
	ReaderPriority("input.txt");
	printf("\n按回车键结束");
	getchar();

	return 0;
}