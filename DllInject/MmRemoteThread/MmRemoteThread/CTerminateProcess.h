
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

//写入日志需要记录的信息.


typedef struct _NowDate
{
	TCHAR tmp0[128]; //年月日
	TCHAR tmp1[128]; //时分秒
	TCHAR tmp2[128];  //毫秒
}NOWDATE, * PNOWDATE;


typedef struct _LogList
{
	//时间
	//写入的内容
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

	//遍历进程.获取进程
	virtual BOOL GetProcessPidByName(MYISTRING pszName,DWORD &pid,LPVOID AdditionalParameter);

	// 初始化你要查找进程的map表
	virtual BOOL InitSpecifyAProcessName(LPVOID AdditionalParameter);
	//添加你要查找进程的map表
	virtual BOOL AddSpecifyAProcessName(MYISTRING map_Key,MYISTRING map_value,LPVOID AdditionalParameter);


	virtual BOOL InitSpecifyAModuleName(LPVOID AdditionalParameter);
	//添加你要查找进程的map表
	virtual BOOL AddSpecifyAModuleName(MYISTRING MuiteMap_Key, MYISTRING MuiteMap_Value, LPVOID AdditionalParameter);


	//通过指定名字进行遍历模块.返回其进程PID
	virtual BOOL DetectionStartBySpecifyAName(OUT vector<DWORD> &dwPid,LPVOID AdditionalParameter);
	
	//根据进程ID以及特定的DLL名字.进行读取.
	virtual BOOL DoesDLLExist(DWORD dwPid,LPVOID AdditionalParameter);
	//根据进程ID.以及进程名的截取字符串进行查找.
	virtual BOOL DoesDLLExistAnFindStrBySubStrName(DWORD dwPid,
		DWORD Begin,
		DWORD end,
		MYISTRING ArgFindStrSubStrName);

	//进程DLL特征检测.检测给定的进程ID下的所有DLL.判断是否是
	virtual BOOL ProcessDLLFeatureDetection(DWORD dwPid, LPVOID AdditionalParameter);

	//检测所有进程名.从里面提取特征. DoesDLLExist  ProcessDLLFeatureDetection 配合使用.
	virtual BOOL DetectionStartAllProcessName(OUT vector<DWORD>& dwPid, LPVOID AdditionalParameter); //遍历所有进程名

	//传入DLL 以及你要获取的名字.进行返回.
	virtual LPVOID GetCallProc(const TCHAR* SpecifyDllName, LPCSTR SpecifyProcName, LPVOID AdditionalParameter) const;

	//是否暂停以及启动进程. 暂停传入TRUE 否则 传入FALSE 返回是否成功
	virtual BOOL EnableSuspendAnResumeProcess(DWORD dwPid,BOOL IsSupendProcess,LPVOID AdditionalParameter);

protected:
	map<MYISTRING, MYISTRING> *m_SpecifyAProcessName;
	multimap<MYISTRING, MYISTRING> *m_SpecifyAModuleName; //模块名不允许重复.

	typedef DWORD(WINAPI* NtSuspendProcess)(HANDLE ProcessHandle);
	typedef DWORD(WINAPI* NtResumeProcess)(HANDLE hProcess);
	typedef DWORD(WINAPI* NtTerminateProcess)(HANDLE, DWORD);
	NtSuspendProcess   m_PfnNtSuspendProcess;
	NtResumeProcess    m_PfnNtResumeProcess;
	NtTerminateProcess m_PfnNtTerminateProcess;
};

