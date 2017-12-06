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

}



int main()
{
	return 0;
}