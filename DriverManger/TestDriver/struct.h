#pragma once




typedef struct _KILLPROCESS
{
    unsigned int pid;

}KILLPROCESS,*PKILLPROCESS;


#define IOCTRL_BASE 0x800
#define MYIOCTRL_CODE(i)\
    CTL_CODE(FILE_DEVICE_UNKNOWN,IOCTRL_BASE+i,METHOD_BUFFERED,FILE_ANY_ACCESS)
//�����豸���� �豸��������ֵ  ����R3��R0��ͨѶ��ʽ.��ָ��Device.  ���ǵ�Ȩ��.

#define CTL_PAKILLPROCESS  MYIOCTRL_CODE(1) //ǿɱ
#define CTL_ZWKILLPROCESS  MYIOCTRL_CODE(2) //��������