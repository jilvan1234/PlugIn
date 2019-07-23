
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
SERVICE_STATUS        g_ServiceStatus; //�����״̬
BOOL g_ContrlCodeExcue;   //�������ǵĴ����Ƿ�ִ��
#define SERVICE_NAME "SystemServices"
#define LOAG_FILE_NAME "SystemServicesLog.txt"
#define TEXT_CREATE_PROCESS_NAME_SY TEXT("C:\\Users\\Administrator\\AppData\\Local\\Temp\\PRO86.exe") //�����ĳ���·��.
#define PROCESS_NAME TEXT("SystemRun.exe")

BOOL GetAppendNmae(wstring &wGetName, wstring AppendName); //Get Moudle Name Last - 1 Path Cat In Path
//������ƺ���

DWORD WINAPI ServiceHandle(DWORD    dwControl, DWORD    dwEventType, LPVOID   lpEventData, LPVOID   lpContext)
{
	//����Ŀ���״̬.����
	switch (dwControl)
	{
		//�ص������ᴫ��dwControl�����ǿ��Ʒ���.����ֻ��Ҫ���Ƽ���.����״̬�������ü���.
	case SERVICE_CONTROL_PAUSE:
		//��ͣ
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

BOOL GetEachLevelDir(PWSTR GetPathName, vector<wstring> &wSavePath) //����һ��·��,��ȡÿһ����Ŀ¼.������������д洢.
{
	TCHAR *pCurPoint = GetPathName;//ʹ��ָ�����

	TCHAR  *Begin = GetPathName;
	DWORD dwEnd = 0;
	DWORD dwCount = 0;
	wstring wstrTemp = TEXT("");
	while (pCurPoint)
	{

		//�������ѭ��
		//wcscmp((TCHAR *)*pCurPoint,TEXT("\\")) == 0
		if (*pCurPoint == TEXT('\\'))//�Է�б��Ϊ�ָ�.
		{

			wSavePath.push_back(wstrTemp);
			wstrTemp.clear();
			pCurPoint++; //����ָ��ȥ���.
			dwEnd = 0;

		}

		if (*pCurPoint == 0x0)//��β��0������ѭ��.��¼��·��.
		{
			return TRUE;
			break;
		}
		wstring::iterator it = wstrTemp.begin() + dwEnd;
		wstrTemp.insert(it, *pCurPoint);  //�����ַ�
		dwEnd++;
		pCurPoint++;
		dwCount++; //��ȡ�жϳ��Ƚ����˳�.
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

//Ҳ���Դ������̣����Ǵ����������Ǹ�����õ�ǰEXPLORER��Ȩ�޴����ġ� ���������ǹ���ԱȨ�ޡ�
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

BOOL GetAppendNmae(wstring &wGetName, wstring AppendName)//��ȡ�������̵�����������
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
	fwprintf_s(pFile, lpParam); //д�뵽�ļ�
	fclose(pFile);
	return;
}

void RunRemoteControl() //�ڷ����д����������.
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
    OutputDebugString(TEXT("�ļ�·�� = \r\n"));
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
	//�ڷ���Main����.������Ҫע�����.
	/*
	RegisterServiceCtrlHandler 
	ע��һ�����������������������.
	*/

	g_ServiceStatus.dwServiceType = SERVICE_WIN32;    //�����������Win32����
	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING; ; //�����״̬������Ѳ��
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP ;// ������ֵĿ��ƴ��뺰�������еĴ���. (Regע��ĺ�����ַ); ��ǰ����˼��������������ֹͣ.���һᴫ��Regע���ServiceHandle����.
	g_ServiceStatus.dwWin32ExitCode = 0;  //�������ﱨ��������ֹͣʱ�����Ĵ������.�����ض��Ĵ�����������.
	g_ServiceStatus.dwServiceSpecificExitCode = 0;//�����ض��Ĵ������
	g_ServiceStatus.dwCheckPoint = 0;             //��������һ��,������������ʱ���������ֵ
	g_ServiceStatus.dwWaitHint = 0;               //��������,ֹͣ,��ͣ.�������������Ҫ�Ĺ���ʱ��. 

	BOOL bRet = FALSE;
	g_RegRetServiceStatusHandle = RegisterServiceCtrlHandlerEx(TEXT(SERVICE_NAME), ServiceHandle,NULL);//���ط���״̬.
	if (g_RegRetServiceStatusHandle == (SERVICE_STATUS_HANDLE)0)
	{
		return;
	}
	//����,����SetServiceStatus���и��·����״̬��Ϣ �ṹ�������·���״̬�ṹ��Ϣ.
	
	g_ServiceStatus.dwCurrentState = SERVICE_RUNNING; //Service Status Setting Start.
	 bRet = SetServiceStatus(g_RegRetServiceStatusHandle,&g_ServiceStatus);

	if (!bRet)
	{
		return ;
	}
	//��д�Լ��Ĵ���.
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

	��������б�ṹ��
	����1.ָ���ķ�������.
	����2.��������Main������ַ.
	*/
	////��װ����.

	//SERVICE_TABLE_ENTRY ServiceTable[2] = { NULL };
	//ServiceTable[0].lpServiceName = (LPWSTR)TEXT(SERVICE_NAME); //ָ�����ǵķ�������
	//ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)SerMain;//ָ�����ĺ�����ַ
	////LPSERVICE_MAIN_FUNCTIONW

	//ServiceTable[1].lpServiceName = NULL;
	//ServiceTable[1].lpServiceProc = NULL;
	//StartServiceCtrlDispatcher(ServiceTable); //��ʼ�ɷ�.

	
		 SERVICE_TABLE_ENTRY ServiceTable[2] = { NULL };
		ServiceTable[0].lpServiceName = (LPWSTR)TEXT(SERVICE_NAME); //ָ�����ǵķ�������
		ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)SerMain;//ָ�����ĺ�����ַ
		//LPSERVICE_MAIN_FUNCTIONW

		ServiceTable[1].lpServiceName = NULL;
		ServiceTable[1].lpServiceProc = NULL;
		StartServiceCtrlDispatcher(ServiceTable); //��ʼ�ɷ�.
	

	return 0;
}