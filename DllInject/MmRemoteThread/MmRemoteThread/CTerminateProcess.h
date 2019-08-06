
#pragma once

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <set>
#include <TlHelp32.h>
#include <string.h>
#include <tchar.h>
#include <map>
#include <sys/types.h> 
#include <sys/timeb.h>
#include <time.h>

using namespace std;

void __cdecl MyOutputDebugStrig(const _TCHAR* pszFormat, ...);

//д����־��Ҫ��¼����Ϣ.


typedef struct _NowDate
{
	TCHAR tmp0[128]; //������
	TCHAR tmp1[128]; //ʱ����
	TCHAR tmp2[128];  //����
}NOWDATE, * PNOWDATE;


typedef struct _LogList
{
	//ʱ��
	//д�������
	NOWDATE NowData;
	TCHAR * pszWriteLog; 

}LOGLIST, *PLOGLIST;

class CTerminateProcess
{

#ifdef UNICODE
#define STRING       wstring
#define AWCSTR        LPCWSTR
#else
#define MYISTRING       string
#define AWCSTR       LPCSTR
#endif

public:

	CTerminateProcess();
	virtual ~CTerminateProcess();




	virtual NOWDATE GetNowTime();


	virtual void  WriteLogString(AWCSTR lpParam, DWORD dwCode,LPVOID AdditionalParameter);


	
	virtual bool AdjustPrivileges();
		
	virtual BOOL ZwTerminateProcess(DWORD dwPid, DWORD dwExitcode);

	//��������.��ȡ����
	virtual BOOL GetProcessPidByName(MYISTRING pszName,DWORD &pid,LPVOID AdditionalParameter);

	// ��ʼ����Ҫ���ҽ��̵�map��
	virtual BOOL InitSpecifyAProcessName(LPVOID AdditionalParameter);
	//�����Ҫ���ҽ��̵�map��
	virtual BOOL AddSpecifyAProcessName(MYISTRING map_Key,MYISTRING map_value,LPVOID AdditionalParameter);


	virtual BOOL InitSpecifyAModuleName(LPVOID AdditionalParameter);
	//�����Ҫ���ҽ��̵�map��
	virtual BOOL AddSpecifyAModuleName(MYISTRING MuiteMap_Key, MYISTRING MuiteMap_Value, LPVOID AdditionalParameter);


	//ͨ��ָ�����ֽ��б���ģ��.���������PID
	virtual BOOL DetectionStartBySpecifyAName(OUT vector<DWORD> &dwPid,LPVOID AdditionalParameter);
	
	//���ݽ���ID�Լ��ض���DLL����.���ж�ȡ.
	virtual BOOL DoesDLLExist(DWORD dwPid,LPVOID AdditionalParameter);
	//���ݽ���ID.�Լ��������Ľ�ȡ�ַ������в���.
	virtual BOOL DoesDLLExistAnFindStrBySubStrName(DWORD dwPid,
		DWORD Begin,
		DWORD end,
		MYISTRING ArgFindStrSubStrName);

	//����DLL�������.�������Ľ���ID�µ�����DLL.�ж��Ƿ���
	virtual BOOL ProcessDLLFeatureDetection(DWORD dwPid, LPVOID AdditionalParameter);

	//������н�����.��������ȡ����. DoesDLLExist  ProcessDLLFeatureDetection ���ʹ��.
	virtual BOOL DetectionStartAllProcessName(OUT vector<DWORD>& dwPid, LPVOID AdditionalParameter); //�������н�����

	//����DLL �Լ���Ҫ��ȡ������.���з���.
	virtual LPVOID GetCallProc(const TCHAR* SpecifyDllName, LPCSTR SpecifyProcName, LPVOID AdditionalParameter) const;

	//�Ƿ���ͣ�Լ���������. ��ͣ����TRUE ���� ����FALSE �����Ƿ�ɹ�
	virtual BOOL EnableSuspendAnResumeProcess(DWORD dwPid,BOOL IsSupendProcess,LPVOID AdditionalParameter);

protected:
	map<MYISTRING, MYISTRING> *m_SpecifyAProcessName;
	multimap<MYISTRING, MYISTRING> *m_SpecifyAModuleName; //ģ�����������ظ�.

	typedef DWORD(WINAPI* NtSuspendProcess)(HANDLE ProcessHandle);
	typedef DWORD(WINAPI* NtResumeProcess)(HANDLE hProcess);
	typedef DWORD(WINAPI* NtTerminateProcess)(HANDLE, DWORD);
	NtSuspendProcess   m_PfnNtSuspendProcess;
	NtResumeProcess    m_PfnNtResumeProcess;
	NtTerminateProcess m_PfnNtTerminateProcess;
};

