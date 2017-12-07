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


//�������ȡ������������߳�
//P: �����߳���Ϣ
void RP_ReaderThread(void *p)
{
	//�������
	HANDLE h_Mutex;
	h_Mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "mutex_for_readcount");
	DWORD wait_for_mutex;	//�ȴ������������Ȩ
	DWORD m_delay;	//�ӳ�ʱ��
	DWORD m_persist;	//���ļ�����ʱ��
	int m_serial;	//�߳����к�
	//�Ӳ����л����Ϣ
	m_serial = ((ThreadInfo*)(p))->serial;
	m_delay = (DWORD)(((ThreadInfo*)(p))->delay *INTE_PER_SEC);
	m_persist = (DWORD)(((ThreadInfo*)(p))->persist *INTE_PER_SEC);
	Sleep(m_delay);	//�ӳٵȴ�
	printf("���߳� %d ����������\n", m_serial);
	//�ȴ������źţ���֤�� ReadCount �ķ��ʣ��޸Ļ���
	wait_for_mutex = WaitForSingleObject(h_Mutex, -1);
	//������������
	readcount++;
	if (readcount==1)
	{
		//��һ�����ߣ��ȴ���Դ
		EnterCriticalSection(&RP_Write);
	}
	ReleaseMutex(h_Mutex);	//�ͷŻ����ź�
	//���ļ�
	printf("���߳� %d ��ʼ���ļ���\n", m_serial);
	Sleep(m_persist);
	//�˳��߳�
	printf("���߳� %d �������ļ���\n", m_serial);
	//�ȴ������źţ���֤�� ReadCount �ķ��ʣ��޸Ļ���
	wait_for_mutex = WaitForSingleObject(h_Mutex, -1);
	//������������
	readcount--;
	if (readcount==0)
	{
		LeaveCriticalSection(&RP_Write);	//������ж��߶��꣬����д��
	}
	ReleaseMutex(h_Mutex);	//�ͷ��ź���
}


//�������ȡ�������д�߽���
//P: д����Ϣ
void RP_WriterThread(void *p)
{
	DWORD m_delay;	//�ӳ�ʱ��
	DWORD m_persist;	//д�ļ�����ʱ��
	int m_serial;	//�߳����
	//�Ӳ����л����Ϣ
	m_serial = ((ThreadInfo*)(p))->serial;
	m_delay = (DWORD)(((ThreadInfo*)(p))->delay *INTE_PER_SEC);
	m_persist = (DWORD)(((ThreadInfo*)(p))->persist *INTE_PER_SEC);
	Sleep(m_delay);
	printf("д���� %d �������� *** \n", m_serial);
	//�ȴ���Դ
	EnterCriticalSection(&RP_Write);
	//д�ļ�
	printf("д���� %d ��ʼд�ļ���\n", m_serial);
	Sleep(m_persist);
	//�˳�����
	printf("д���� %d ����д�ļ���\n", m_serial);
	//�ͷ���Դ
	LeaveCriticalSection(&RP_Write);
}


void ReaderPriority(char *file)
{
	DWORD n_thread = 0;	//�߳���Ŀ
	DWORD thread_ID;	//�߳� ID
	DWORD wait_for_all;	//�ȴ������߳̽���
	//�������
	HANDLE h_Mutex;
	h_Mutex = CreateMutex(NULL, FALSE, "mutex_for_readcount");
	//�̶߳��������
	HANDLE h_Thread[MAX_THREAD_NUM];
	ThreadInfo thread_info[MAX_THREAD_NUM];
	readcount = 0;	//��ʼ�� readcount
	InitializeCriticalSection(&RP_Write);	//��ʼ���ٽ���

	printf("�������ȣ�\n\n");

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
	//	printf("���ļ�\"%s\"ʧ�ܣ��뽫\"%s\"���ڳ���Ŀ¼�¡�\n", file, file);
	//	return;
	//}
	//printf("�������ȣ�\n\n");
	//while (inFile)
	//{
	//	//����ÿһ�����ߡ�д����Ϣ
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
			h_Thread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(RP_ReaderThread), &thread_info[i], 0, &thread_ID);	//�������߽���
		}
		else
		{
			h_Thread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(RP_WriterThread), &thread_info[i], 0, &thread_ID);	//����д�߽���
		}
	}

	//�ȴ����е��߳̽���
	wait_for_all = WaitForMultipleObjects(n_thread, h_Thread, TRUE, -1);
	printf("���еĶ�д�߳̽���������\n");
}


int main(int argc, char *argv[])
{
	ReaderPriority("input.txt");
	printf("\n���س�������");
	getchar();

	return 0;
}