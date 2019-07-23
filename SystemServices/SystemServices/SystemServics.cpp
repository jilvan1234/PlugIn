
#pragma warning (disable:4996)
#include <userenv.h>
#include <Windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <string>
#include <vector>
#include "IGetFileName.h"
#include "CRetStartFileName.h"
using namespace std;

#pragma comment(lib,"Userenv.lib")
SERVICE_STATUS_HANDLE g_RegRetServiceStatusHandle;
SERVICE_STATUS        g_ServiceStatus; //服务的状态
BOOL g_ContrlCodeExcue;   //控制我们的代码是否执行
#define SERVICE_NAME "SystemServices"
#define LOAG_FILE_NAME "SystemServicesLog.txt"
#define TEXT_CREATE_PROCESS_NAME_SY TEXT("C:\\Users\\Administrator\\AppData\\Local\\Temp\\PRO86.exe") //启动的程序路径.
#define PROCESS_NAME TEXT("SystemRun.exe")

BOOL GetAppendNmae(wstring &wGetName, wstring AppendName); //Get Moudle Name Last - 1 Path Cat In Path
//服务控制函数

DWORD WINAPI ServiceHandle(DWORD    dwControl, DWORD    dwEventType, LPVOID   lpEventData, LPVOID   lpContext)
{
	//服务的控制状态.管理
	switch (dwControl)
	{
		//回调函数会传入dwControl让我们控制服务.我们只需要控制即可.更改状态并且设置即可.
	case SERVICE_CONTROL_PAUSE:
		//暂停
		g_ServiceStatus.dwCurrentState = SERVICE_PAUSED;
		break;
	case SERVICE_CONTROL_CONTINUE:
		g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;

		break;
	case SERVICE_CONTROL_STOP:
		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwCheckPoint = 0;
		g_ServiceStatus.dwWaitHint = 0;
		//SetServiceStatus(g_RegRetServiceStatusHandle, &g_ServiceStatus);
		g_ContrlCodeExcue = FALSE;
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	default:
		break;
	}
	SetServiceStatus(g_RegRetServiceStatusHandle, &g_ServiceStatus);
	return 0;
}

BOOL GetEachLevelDir(PWSTR GetPathName, vector<wstring> &wSavePath) //传入一个路径,获取每一级的目录.并且用数组进行存储.
{
	TCHAR *pCurPoint = GetPathName;//使用指针遍历

	TCHAR  *Begin = GetPathName;
	DWORD dwEnd = 0;
	DWORD dwCount = 0;
	wstring wstrTemp = TEXT("");
	while (pCurPoint)
	{

		//否则进行循环
		//wcscmp((TCHAR *)*pCurPoint,TEXT("\\")) == 0
		if (*pCurPoint == TEXT('\\'))//以反斜杠为分割.
		{

			wSavePath.push_back(wstrTemp);
			wstrTemp.clear();
			pCurPoint++; //控制指针去相加.
			dwEnd = 0;

		}

		if (*pCurPoint == 0x0)//结尾是0则跳出循环.记录下路径.
		{
			return TRUE;
			break;
		}
		wstring::iterator it = wstrTemp.begin() + dwEnd;
		wstrTemp.insert(it, *pCurPoint);  //插入字符
		dwEnd++;
		pCurPoint++;
		dwCount++; //获取判断长度进行退出.
	}
	return FALSE;
}

BOOL   GetTokenByName(HANDLE   &hToken, LPWSTR   lpName)
{
	if (!lpName)
	{
		return   FALSE;
	}
	HANDLE                   hProcessSnap = NULL;
	BOOL                       bRet = FALSE;
	PROCESSENTRY32   pe32 = { 0 };

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return   (FALSE);

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			if (!wcscmp(_wcsupr(pe32.szExeFile), _wcsupr(lpName)))
			{
				HANDLE   hProcess =
					OpenProcess(PROCESS_QUERY_INFORMATION,
					FALSE, pe32.th32ProcessID);
				bRet = OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
				CloseHandle(hProcessSnap);
				return   (bRet);
			}
		} while (Process32Next(hProcessSnap, &pe32));
		bRet = TRUE;
	}
	else
		bRet = FALSE;

	CloseHandle(hProcessSnap);
	return   (bRet);
}

//也可以创建进程，但是创建出来的是根据你得当前EXPLORER的权限创建的。 创建出来是管理员权限。
BOOL   RunProcess(LPWSTR   lpImage)
{
	if (!lpImage)
	{
		return   FALSE;
	}
	HANDLE   hToken;
	//TEXT("EXPLORER.EXE")
	if (!GetTokenByName(hToken, TEXT("EXPLORER.EXE")))
	{
		return   FALSE;
	}
	STARTUPINFO   si;
	PROCESS_INFORMATION   pi;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = (LPWSTR)TEXT("winsta0\\default");

	BOOL   bResult =
		CreateProcessAsUser(hToken, lpImage, NULL, NULL, NULL,
		FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	CloseHandle(hToken);
	if (bResult)
	{
		OutputDebugString(TEXT("CreateProcessAsUser   ok!\r\n"));
	}
	else
	{
		OutputDebugString(TEXT("CreateProcessAsUser   false!\r\n"));
	}
	return   bResult;
} 

BOOL GetAppendNmae(wstring &wGetName, wstring AppendName)//获取启动进程的启动的名字
{

	TCHAR FilePath[MAX_PATH] = { 0 };
	GetModuleFileName(GetModuleHandle(0), FilePath, MAX_PATH);
	vector<wstring> wSaveDirLeavePath;
	if (!GetEachLevelDir(FilePath, wSaveDirLeavePath))
	{
		return FALSE;
	}
	//cat 
	wstring wCatPath = TEXT("");
	for (int i = 0; i < wSaveDirLeavePath.size(); i++)
	{
		if (i == (wSaveDirLeavePath.size() - 1))
		{
			break;
		}
		wCatPath.append(wSaveDirLeavePath[i]);
		wCatPath.append(TEXT("\\"));
	}
	wCatPath.append(AppendName);
	wGetName = wCatPath;

	return TRUE;
}

void WriteLogString(LPWSTR lpParam, DWORD dwCode)
{
	TCHAR lpBuffer[1024]  {0};
	wsprintf(lpBuffer,lpParam, dwCode);
	OutputDebugString(lpBuffer);
	//GetCurrentDirectory(1024, lpBuffer);
	wstring WriteLogName;
	if (!GetAppendNmae(WriteLogName, TEXT(LOAG_FILE_NAME)))
	{
		return;
	}
	FILE *pFile = NULL;
	_wfopen_s(&pFile, WriteLogName.c_str(), TEXT("a+"));
	if (NULL == pFile)
	{
		return;
	}
	fwprintf_s(pFile, lpParam); //写入到文件
	fclose(pFile);
	return;
}

void RunRemoteControl() //在服务中创建桌面进程.
{
	HANDLE ProcessHandle = NULL;
	HANDLE CurrentToken = NULL;
	HANDLE TokenDup = NULL;
	wstring wOpenProcessName;
    IGetFileName *pGetFileName = new CRetStartFileName();
	/*if (!GetAppendNmae(wOpenProcessName, PROCESS_NAME))
	{
		int d = GetLastError();
		WriteLogString(reinterpret_cast<LPWSTR>("RunRemoteControl OpenProcessToken failed.Last Error is:%d"), d);
		return ;
	}*/
	ProcessHandle = GetCurrentProcess();
	if (!OpenProcessToken(ProcessHandle, TOKEN_ALL_ACCESS, &CurrentToken))
	{
		int d = GetLastError();
		WriteLogString(reinterpret_cast<LPWSTR>(L"RunRemoteControl OpenProcessToken failed.Last Error is:%d"), d);
		return;
	}
	if (!DuplicateTokenEx(CurrentToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &TokenDup))
	{
		int dd = GetLastError();
		WriteLogString(reinterpret_cast<LPWSTR>("RunRemoteControl DuplicateTokenEx failed.Last error is:%d"), dd);
		return;
	}
	DWORD dwSessionID = WTSGetActiveConsoleSessionId();
	WriteLogString(reinterpret_cast<LPWSTR>(L" WTSGetActiveConsoleSessionId:%d"), dwSessionID);
	if (!SetTokenInformation(TokenDup, TokenSessionId, &dwSessionID, sizeof(DWORD)))
	{
		int ddd = GetLastError();
		WriteLogString(reinterpret_cast<LPWSTR>(L"RunRemoteControl SetTokenInformation failed.Last error is:%d"), ddd);
		return;
	}
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop =(LPWSTR)L"WinSta0\\Default";

	LPVOID pEnv = NULL;
	DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT;
	if (!CreateEnvironmentBlock(&pEnv, TokenDup, FALSE))
	{
		int error1 = GetLastError();
		WriteLogString(reinterpret_cast<LPWSTR>(L"RunRemoteControl CreateEnvironmentBlock failed.Last error is:%d"), error1);
		return;
	}
	TCHAR szPath[1024] =  TEXT_CREATE_PROCESS_NAME_SY;
	//wcsncpy_s(szPath, wOpenProcessName.c_str(), wOpenProcessName.length());
	//OutputDebugString(szPath);
    //pGetFileName->GetStartFileName().c_str()
    wcsncpy_s(szPath, pGetFileName->GetStartFileName().c_str(), pGetFileName->GetStartFileName().length());
    OutputDebugString(TEXT("文件路径 = \r\n"));
    OutputDebugString(pGetFileName->GetStartFileName().c_str());
	if (!CreateProcessAsUser(TokenDup, pGetFileName->GetStartFileName().c_str(), NULL, NULL, NULL, FALSE, dwCreationFlags, pEnv, NULL, &si, &pi))
	{
		int error2 = GetLastError();
        delete pGetFileName;
		WriteLogString(reinterpret_cast<LPWSTR>(L"RunRemoteControl CreateProcessAsUser failed.Last error is:%d"), error2);
		return;
	}
    delete pGetFileName;
	return	;
}

VOID WINAPI SerMain(DWORD   dwNumServicesArgs,LPWSTR* lpServiceArgVectors)
{
	//在服务Main里面.我们需要注册服务.
	/*
	RegisterServiceCtrlHandler 
	注册一个函数来处理服务的请求控制.
	*/

	g_ServiceStatus.dwServiceType = SERVICE_WIN32;    //服务的类型是Win32服务
	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING; ; //服务的状态是正在巡行
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP ;// 服务接手的控制代码喊处理函数中的处理. (Reg注册的函数地址); 当前的意思就是这个服务可以停止.并且会传入Reg注册的ServiceHandle处理.
	g_ServiceStatus.dwWin32ExitCode = 0;  //服务用语报告启动或停止时候发生的错误代码.返回特定的错误代码就设置.
	g_ServiceStatus.dwServiceSpecificExitCode = 0;//服务特定的错误代码
	g_ServiceStatus.dwCheckPoint = 0;             //跟进度条一样,服务在启动的时候会递增这个值
	g_ServiceStatus.dwWaitHint = 0;               //挂起启动,停止,暂停.或继续操作所需要的估计时间. 

	BOOL bRet = FALSE;
	g_RegRetServiceStatusHandle = RegisterServiceCtrlHandlerEx(TEXT(SERVICE_NAME), ServiceHandle,NULL);//返回服务状态.
	if (g_RegRetServiceStatusHandle == (SERVICE_STATUS_HANDLE)0)
	{
		return;
	}
	//否则,调用SetServiceStatus进行更新服务的状态信息 结构体是最新服务状态结构信息.
	
	g_ServiceStatus.dwCurrentState = SERVICE_RUNNING; //Service Status Setting Start.
	 bRet = SetServiceStatus(g_RegRetServiceStatusHandle,&g_ServiceStatus);

	if (!bRet)
	{
		return ;
	}
	//编写自己的代码.
	g_ContrlCodeExcue = TRUE;
	
	
	
	RunRemoteControl();
    
	
	while (g_ContrlCodeExcue)
	{
		Sleep(2000);
	}
	return;
}

int main()
{
	/*

	定义服务列表结构体
	参数1.指定的服务名字.
	参数2.服务的入后Main函数地址.
	*/
	////安装服务.

	//SERVICE_TABLE_ENTRY ServiceTable[2] = { NULL };
	//ServiceTable[0].lpServiceName = (LPWSTR)TEXT(SERVICE_NAME); //指定我们的服务名字
	//ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)SerMain;//指向服务的函数地址
	////LPSERVICE_MAIN_FUNCTIONW

	//ServiceTable[1].lpServiceName = NULL;
	//ServiceTable[1].lpServiceProc = NULL;
	//StartServiceCtrlDispatcher(ServiceTable); //开始派发.

	
		 SERVICE_TABLE_ENTRY ServiceTable[2] = { NULL };
		ServiceTable[0].lpServiceName = (LPWSTR)TEXT(SERVICE_NAME); //指定我们的服务名字
		ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)SerMain;//指向服务的函数地址
		//LPSERVICE_MAIN_FUNCTIONW

		ServiceTable[1].lpServiceName = NULL;
		ServiceTable[1].lpServiceProc = NULL;
		StartServiceCtrlDispatcher(ServiceTable); //开始派发.
	

	return 0;
}