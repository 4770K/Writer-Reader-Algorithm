#include "windows.h"
#include <conio.h>
#include <stdlib.h>
#include <fstream>	//#include <fstream.h>
#include <io.h>
#include <string.h>
#include <stdio.h>
using namespace std;

#define READER 'R'	//����
#define WRITER 'W'	//д��
#define INTE_PER_SEC 1000	//ÿ��ʱ���жϵ���Ŀ
#define MAX_THREAD_NUM 63	//����߳���
#define MAX_FILE_NUM 32	//����ļ���Ŀ��
#define MAX_STR_LEN 32	//�ַ����ĳ���
int readcount = 0;
int writecount = 0;
CRITICAL_SECTION RP_Write;	//�ٽ���Դ
CRITICAL_SECTION cs_Write;
CRITICAL_SECTION cs_Read;


struct ThreadInfo
{
	int serial;	//�߳����
	int entity;	//�߳�����ж��Ƕ��߻���д�ߣ�
	double delay;	//�߳��ӳ�ʱ��
	double persist;	//�̶߳�д����ʱ��
};


//�������ȡ��������߳�
//P: �����߳���Ϣ
void RP_ReaderThread(void *p)
{

}



int main()
{
	return 0;
}