#pragma once

#include "../../publicstruct.h"

/*
未公开API的管理类.
1.ZwQueryinformationProcess
2.ZwQueryObject
3.ZwQuerySystemInformation
*/


class CNativeApiManger
{
public:
    CNativeApiManger();
    ~CNativeApiManger();

public:     //公共的接口函数.  //NAT 为Native的前缀
    PVOID NatMmGetProcessRoutine(CBinString ModelName, string FunctionNamek);
    PVOID NatGetZwQueryInformationProcess() const;
    PVOID NatGetZwQueryObject() const;
    PVOID NatGetZwQuerySystemInformation()const;
 
public:
    //句柄相关
    virtual USHORT NatHndGetHandleTypeWithHandle(HANDLE handle);      //获取句柄类型

    virtual BOOL   NatHndDuplicateHandle(                           //拷贝句柄
        _In_ HANDLE hSourceProcessHandle,
        _In_ HANDLE hSourceHandle,
        _In_ HANDLE hTargetProcessHandle,
        _Outptr_ LPHANDLE lpTargetHandle,
        _In_ DWORD dwDesiredAccess = 0,
        _In_ BOOL bInheritHandle = FALSE,
        _In_ DWORD dwOptions = DUPLICATE_SAME_ACCESS
    );

    virtual BOOL NatHndCloseSpecilProcessOccFileHandle(DWORD dwPid, string OccFileName);

    virtual PVOID NatHndGetSystemHandleInfo();//获取全局句柄信息

private:
    /*
    1.提供公开函数.获取NativeAPi的函数地址.
    */

    PVOID m_ZwQueryInformationProcess;       //进程信息遍历函数指针
    PVOID             m_ZwQueryObject;                   //句柄信息遍历函数指针
    PVOID  m_ZwQuerySystemInformation;        //系统信息遍历函数指针.

    PfnZwQueryInformationProcess ZwQueryInformationProcess;
    PfnZwQueryObject    ZwQueryObject;
    PfnZwQuerySystemInformation ZwQuerySystemInformation;


};








//ZwQueryinfromation信息




