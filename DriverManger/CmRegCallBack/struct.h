#pragma once
//#include <ntdef.h>

/*
������ring3����ͨѶ��ͷ�ļ�
*/


typedef struct _INIT_ROOTKEY
{

    wchar_t *  KeyPathName;                    //Ҫ��ʼ����KEYname

}INIT_ROOTKEY, *PINIT_ROOTKEY;

typedef struct _SET_KEYPATH_VALUENAME_VALUEDATA
{
    wchar_t *KeyPathName;                   //Ҫ���õ�Key·��
    wchar_t *ValueName;                     //Ҫ���õ�Value������
    wchar_t *ValueData;                     //Ҫ���õ�Value����
    ULONG   ValueDataSize;                 //Ҫ���õ����ݴ�С.
    ULONG   ControlCode;                   //ͨ�� �����������������. 1 ����RootKEY_Shell_open_command l
}SET_KEYPATH_VALUENAME_VALUEDATA, *PSET_KEYPATH_VALUENAME_VALUEDATA;

#define IOCTRL_BASE 0x800
#define MYIOCTRL_CODE(i)\
    CTL_CODE(FILE_DEVICE_UNKNOWN,IOCTRL_BASE+i,METHOD_BUFFERED,FILE_ANY_ACCESS)
//�����豸���� �豸��������ֵ  ����R3��R0��ͨѶ��ʽ.��ָ��Device.  ���ǵ�Ȩ��.

#define CTL_INITROOTKEY  MYIOCTRL_CODE(1) //��ʼ��ROOT_KEY �е���Ϣ.����ROOT Key��·�� xxxx\ you create key name
#define CTL_SETKEYVALUE  MYIOCTRL_CODE(2) //����Key Value ��ֵ
#define CTL_INIT_CMCREATEKEY  MYIOCTRL_CODE(3) //��ʼ�� CmCreateKey�ĺ�����ַ��ֵ
#define CM_REMOVE_CALLBACK   MYIOCTRL_CODE(4)  //ɾ��ע���ص�



