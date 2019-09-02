#pragma once

#include "../../publicstruct.h"

/*
δ����API�Ĺ�����.
1.ZwQueryinformationProcess
2.ZwQueryObject
3.ZwQuerySystemInformation
*/


class CNativeApiManger
{
public:
    CNativeApiManger();
    ~CNativeApiManger();

public:     //�����Ľӿں���.  //NAT ΪNative��ǰ׺
    PVOID NatMmGetProcessRoutine(CBinString ModelName, string FunctionNamek);
    PVOID NatGetZwQueryInformationProcess() const;
    PVOID NatGetZwQueryObject() const;
    PVOID NatGetZwQuerySystemInformation()const;
 
public:
    //������
    virtual USHORT NatHndGetHandleTypeWithHandle(HANDLE handle);      //��ȡ�������

    virtual BOOL   NatHndDuplicateHandle(                           //�������
        _In_ HANDLE hSourceProcessHandle,
        _In_ HANDLE hSourceHandle,
        _In_ HANDLE hTargetProcessHandle,
        _Outptr_ LPHANDLE lpTargetHandle,
        _In_ DWORD dwDesiredAccess = 0,
        _In_ BOOL bInheritHandle = FALSE,
        _In_ DWORD dwOptions = DUPLICATE_SAME_ACCESS
    );

    virtual BOOL NatHndCloseSpecilProcessOccFileHandle(DWORD dwPid, string OccFileName);

    virtual PVOID NatHndGetSystemHandleInfo();//��ȡȫ�־����Ϣ

private:
    /*
    1.�ṩ��������.��ȡNativeAPi�ĺ�����ַ.
    */

    PVOID m_ZwQueryInformationProcess;       //������Ϣ��������ָ��
    PVOID             m_ZwQueryObject;                   //�����Ϣ��������ָ��
    PVOID  m_ZwQuerySystemInformation;        //ϵͳ��Ϣ��������ָ��.

    PfnZwQueryInformationProcess ZwQueryInformationProcess;
    PfnZwQueryObject    ZwQueryObject;
    PfnZwQuerySystemInformation ZwQuerySystemInformation;


};








//ZwQueryinfromation��Ϣ




