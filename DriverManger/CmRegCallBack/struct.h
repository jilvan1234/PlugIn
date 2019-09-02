#pragma once
//#include <ntdef.h>

/*
驱动跟ring3进行通讯的头文件
*/


typedef struct _INIT_ROOTKEY
{

    wchar_t *  KeyPathName;                    //要初始化的KEYname

}INIT_ROOTKEY, *PINIT_ROOTKEY;

typedef struct _SET_KEYPATH_VALUENAME_VALUEDATA
{
    wchar_t *KeyPathName;                   //要设置的Key路径
    wchar_t *ValueName;                     //要设置的Value的名字
    wchar_t *ValueData;                     //要设置的Value数据
    ULONG   ValueDataSize;                 //要设置的数据大小.
    ULONG   ControlCode;                   //通过 控制码区分如何设置. 1 设置RootKEY_Shell_open_command l
}SET_KEYPATH_VALUENAME_VALUEDATA, *PSET_KEYPATH_VALUENAME_VALUEDATA;

#define IOCTRL_BASE 0x800
#define MYIOCTRL_CODE(i)\
    CTL_CODE(FILE_DEVICE_UNKNOWN,IOCTRL_BASE+i,METHOD_BUFFERED,FILE_ANY_ACCESS)
//驱动设备类型 设备控制码数值  定义R3跟R0的通讯方式.是指定Device.  我们的权限.

#define CTL_INITROOTKEY  MYIOCTRL_CODE(1) //初始化ROOT_KEY 中的信息.传入ROOT Key的路径 xxxx\ you create key name
#define CTL_SETKEYVALUE  MYIOCTRL_CODE(2) //设置Key Value 的值
#define CTL_INIT_CMCREATEKEY  MYIOCTRL_CODE(3) //初始化 CmCreateKey的函数地址的值
#define CM_REMOVE_CALLBACK   MYIOCTRL_CODE(4)  //删除注册表回调



