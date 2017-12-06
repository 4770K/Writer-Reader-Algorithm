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


//读者优先——读者线程
//P: 读者线程信息
void RP_ReaderThread(void *p)
{

}



int main()
{
	return 0;
}