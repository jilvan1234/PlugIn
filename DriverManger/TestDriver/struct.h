#pragma once




typedef struct _KILLPROCESS
{
    unsigned int pid;

}KILLPROCESS,*PKILLPROCESS;


#define IOCTRL_BASE 0x800
#define MYIOCTRL_CODE(i)\
    CTL_CODE(FILE_DEVICE_UNKNOWN,IOCTRL_BASE+i,METHOD_BUFFERED,FILE_ANY_ACCESS)
//驱动设备类型 设备控制码数值  定义R3跟R0的通讯方式.是指定Device.  我们的权限.

#define CTL_PAKILLPROCESS  MYIOCTRL_CODE(1) //强杀
#define CTL_ZWKILLPROCESS  MYIOCTRL_CODE(2) //正常结束