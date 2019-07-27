#include "CTerminateProcess.h"



void __cdecl MyOutputDebugStrig(const _TCHAR* pszFormat, ...)
{
	_TCHAR buf[1024] = { 0 };
	// ZeroMemory( buf, 1024*sizeof(TCHAR ) );
	swprintf_s(buf, 1024, _T("�߳�ID = [%lu]"), GetCurrentThreadId());
	va_list arglist;
	va_start(arglist, pszFormat);
	int nBuffSize = _tcslen(buf);
	vswprintf_s(&buf[nBuffSize], 1024 - nBuffSize, pszFormat, arglist);
	va_end(arglist);
	nBuffSize = _tcslen(buf);
	_tcscat_s(buf, 1024 - nBuffSize, _T("\n"));
	OutputDebugString(buf);
}

CTerminateProcess::CTerminateProcess()
{

	m_SpecifyAProcessName = NULL;
	m_SpecifyAModuleName  = NULL;

	m_SpecifyAProcessName = new map<STRING,STRING>;
	
	m_SpecifyAModuleName = new multimap<STRING, STRING>;

	m_PfnNtSuspendProcess = NULL;
	m_PfnNtResumeProcess = NULL;

}

CTerminateProcess::~CTerminateProcess()
{
	if (m_SpecifyAProcessName != NULL)
		delete m_SpecifyAProcessName;

	if (m_SpecifyAModuleName != NULL)
		delete m_SpecifyAModuleName;
}



void CTerminateProcess::WriteLogString(AWCSTR lpParam, DWORD dwCode, LPVOID AdditionalParameter)
{

	TCHAR lpBuffer[1024]{ 0 };
	wsprintf(lpBuffer, lpParam, dwCode);
	OutputDebugString(lpBuffer);
	//GetCurrentDirectory(1024, lpBuffer);
	STRING WriteLogName = TEXT("ErrorLog.txt");

	PLOGLIST PlogList = new LOGLIST;
	PlogList->NowData = GetNowTime();
	DWORD dwSize = (wcslen(lpParam) + 2) * sizeof(TCHAR);
	PlogList->pszWriteLog = new TCHAR[dwSize];
#ifdef UNICODE
	wsprintf(PlogList->pszWriteLog, TEXT("%ls ERRCODE = %d"), lpParam, dwCode);
	//wcsncpy_s(PlogList->pszWriteLog, dwSize, lpParam, wcslen(lpParam) + 2);
	FILE* pFile = NULL;
	_wfopen_s(&pFile, WriteLogName.c_str(), TEXT("ab+"));
	if (NULL == pFile)
	{
		return;
	}
	fwrite(PlogList->NowData.tmp0, wcslen(PlogList->NowData.tmp0) + sizeof(TCHAR), 1, pFile);
	fwrite(PlogList->NowData.tmp1, wcslen(PlogList->NowData.tmp1) + sizeof(TCHAR), 1, pFile);
	fwrite(PlogList->NowData.tmp2, wcslen(PlogList->NowData.tmp2) + sizeof(TCHAR), 1, pFile);  //д��ʱ��
	fwrite(PlogList->pszWriteLog, wcslen(PlogList->pszWriteLog) + sizeof(TCHAR), 1, pFile);
	TCHAR szBuf[] = TEXT("\r\n");
	fwrite(szBuf, wcslen(szBuf) + sizeof(TCHAR), 2, pFile);
	fclose(pFile);

	if (PlogList->pszWriteLog != NULL)
		delete PlogList->pszWriteLog;
	if (PlogList != NULL)
		delete PlogList;
	
#else

	sprintf(PlogList->pszWriteLog, TEXT("%ls ERRCODE = %d"), lpParam, dwCode);
	FILE* pFile = NULL;
	fopen_s(&pFile, WriteLogName.c_str(), TEXT("a+"));

	if (NULL == pFile)
	{
		return;
	}

	//fprintf_s(pFile, lpParam); //д�뵽�ļ�
	
	fwrite(PlogList->NowData.tmp0, strlen(PlogList->NowData.tmp0) + sizeof(TCHAR), 1, pFile);
	fwrite(PlogList->NowData.tmp1, strlen(PlogList->NowData.tmp1) + sizeof(TCHAR), 1, pFile);
	fwrite(PlogList->NowData.tmp2, strlen(PlogList->NowData.tmp2) + sizeof(TCHAR), 1, pFile);  //д��ʱ��
	fwrite(PlogList->pszWriteLog, strlen(PlogList->pszWriteLog) + sizeof(TCHAR), 1, pFile);
	TCHAR szBuf[] = TEXT("\r\n");
	fwrite(szBuf, strlen(szBuf) + sizeof(TCHAR), 1, pFile);

	fclose(pFile);

	if (PlogList->pszWriteLog != NULL)
		delete PlogList->pszWriteLog;
	if (PlogList != NULL)
		delete PlogList;

#endif // UNICODE

	
	
	return;

}



NOWDATE CTerminateProcess::GetNowTime()
{
	time_t timep;
	time(&timep);
	NOWDATE date;

#ifdef UNICODE
	wcsftime(date.tmp0, sizeof(date.tmp0)/sizeof(date.tmp0[0]) * sizeof(TCHAR), TEXT("%Y-%m-%d"), localtime(&timep));
	wcsftime(date.tmp1, sizeof(date.tmp0) / sizeof(date.tmp0[0]) * sizeof(TCHAR), TEXT("%H:%M:%S"), localtime(&timep));
	struct timeb tb;
	ftime(&tb);
	wsprintf(date.tmp2, TEXT("%d"), tb.millitm);

#else
	strftime(date.tmp0, sizeof(date.tmp0), "%Y-%m-%d", localtime(&timep));
	strftime(date.tmp1, sizeof(date.tmp1), "%H:%M:%S", localtime(&timep));

	struct timeb tb;
	ftime(&tb);
	sprintf(date.tmp2, "%d", tb.millitm);
#endif
	
	return date;
}

bool CTerminateProcess::AdjustPrivileges()
{
	
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES tp;
	TOKEN_PRIVILEGES oldtp;
	DWORD dwSize = sizeof(TOKEN_PRIVILEGES);
	LUID luid;

	OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);


	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
		CloseHandle(hToken);
		//OutputDebugString(TEXT("����Ȩ��ʧ��,LookupPrivilegeValue"));
		WriteLogString(TEXT("����Ȩ��ʧ��,LookupPrivilegeValue"),GetLastError(),NULL);
		return false;
	}
	ZeroMemory(&tp, sizeof(tp));
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	/* Adjust Token Privileges */
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize)) {
		CloseHandle(hToken);
		OutputDebugString(TEXT("����Ȩ��ʧ�� AdjustTokenPrivileges"));
		WriteLogString(TEXT("����Ȩ��ʧ�� AdjustTokenPrivileges"), GetLastError(),NULL);
		return false;
	}
	// close handles
	CloseHandle(hToken);
	return true;
	
}

BOOL CTerminateProcess::ZwTerminateProcess(DWORD dwPid, DWORD dwExitcode)
{
	if (dwPid == 0)
		return FALSE;

	//NtTerminateProcess



	HANDLE hProcess = NULL;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (NULL == hProcess)
		return FALSE;


	m_PfnNtTerminateProcess = (NtTerminateProcess)GetCallProc(TEXT("ntdll.dll"), "NtTerminateProcess", NULL);
	if (NULL == m_PfnNtTerminateProcess)
		return FALSE;

	m_PfnNtTerminateProcess(hProcess,dwExitcode);
	CloseHandle(hProcess);
	return TRUE;
}

BOOL CTerminateProcess::GetProcessPidByName( STRING pszName, DWORD& pid, LPVOID AdditionalParameter)
{

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return FALSE;
	}
	PROCESSENTRY32 pi;
	pi.dwSize = sizeof(PROCESSENTRY32); //��һ��ʹ�ñ����ʼ����Ա
	BOOL bRet = Process32First(hSnapshot, &pi);

	while (bRet)
	{

		
		if (pszName.compare(pi.szExeFile) == 0)
		{
			pid = pi.th32ProcessID;
			CloseHandle(hSnapshot);
			return TRUE;
		}

		bRet = Process32Next(hSnapshot, &pi);
	}
	CloseHandle(hSnapshot);
	return FALSE;
}

BOOL CTerminateProcess::InitSpecifyAProcessName(LPVOID AdditionalParameter)
{
	//services.exe conhost.exe
	if (NULL == m_SpecifyAProcessName)
		return FALSE;

	m_SpecifyAProcessName->insert((pair<STRING, STRING>(TEXT("services.exe"),TEXT("services.exe"))));
	m_SpecifyAProcessName->insert((pair<STRING, STRING>(TEXT("conhost.exe"), TEXT("conhost.exe"))));

	return TRUE;
}

BOOL CTerminateProcess::AddSpecifyAProcessName(STRING map_Key, STRING map_value, LPVOID AdditionalParameter)
{
	if (NULL == m_SpecifyAProcessName)
		return FALSE;

	m_SpecifyAProcessName->insert((pair<STRING, STRING>(map_Key, map_value)));
	return TRUE;
}

BOOL CTerminateProcess::InitSpecifyAModuleName(LPVOID AdditionalParameter)
{

	if (NULL == m_SpecifyAModuleName)
		return FALSE;

	m_SpecifyAModuleName->insert((pair<STRING, STRING>(TEXT(" "), TEXT(" "))));
	

	return TRUE;


}

BOOL CTerminateProcess::AddSpecifyAModuleName(STRING MuiteMap_Key, STRING MuiteMap_Value, LPVOID AdditionalParameter)
{
	if (NULL == m_SpecifyAModuleName)
		return FALSE;

	m_SpecifyAModuleName->insert((pair<STRING, STRING>(MuiteMap_Key, MuiteMap_Value)));
	return TRUE;
}


BOOL CTerminateProcess::DetectionStartBySpecifyAName(OUT vector<DWORD>& dwPid, LPVOID AdditionalParameter)
{

	/*
	1.���ȱ�������ָ���Ľ���.
	2.�����������зֱ����������ģ��
	3.�ж�ģ�������Ƿ��� jt_IELock.dll
	    �еĻ������������
		û�еĻ�������������.Ѱ����ͬ��.
		�����û��.���ȡDLL����
		�еĻ����н���
		û�еĻ�����.����ȫ���̱�������ģʽ.

	4.����ָ���Ľ���û���ҵ�
		�������н���.���ұ���������ģ��.
		�ҵ����Ե�ģ��
	
	*/

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		//MyOutputDebugStrig(TEXT("DetectionStartBySpecifyAName CreateToolhelp32Snapshot Error Code = %d"), GetLastError());
		//WriteLogString(TEXT("DetectionStartBySpecifyAName CreateToolhelp32Snapshot Error Code"), GetLastError(),NULL);
		return FALSE;
	}
	PROCESSENTRY32 pi;
	pi.dwSize = sizeof(PROCESSENTRY32); //��һ��ʹ�ñ����ʼ����Ա
	BOOL bRet = Process32First(hSnapshot, &pi);

	map<STRING, STRING>::iterator it;

	DWORD dwCount = 0;
	while (bRet)
	{

		
			
			it = m_SpecifyAProcessName->find(pi.szExeFile);//����key����value�ĵ�����
			
			if (it != m_SpecifyAProcessName->end())        //�ҵ���.
			{
				dwCount++;
				//MyOutputDebugStrig(TEXT("DetectionStartBySpecifyAName �ҵ��� Valueֵ = %ls pid = %d"), it->second.c_str(), pi.th32ProcessID);
				BOOL RetValue = FALSE;
				//�ҵ�����֮��,�жϽ������Ƿ���ָ����DLL.�����ҵ�ģ��ĸ���.
				
				RetValue =  DoesDLLExist(pi.th32ProcessID,NULL);
				if (FALSE != RetValue)
				{
					dwPid.push_back(pi.th32ProcessID); //����һ��pid.

				}

				
				RetValue = DoesDLLExist(pi.th32ProcessID, NULL); //��������ȥ����
				if (FALSE != RetValue)
				{
					dwPid.push_back(pi.th32ProcessID);
				}
				else if (FALSE == RetValue)
				{
					RetValue = DoesDLLExistAnFindStrBySubStrName(pi.th32ProcessID, 2, 11, TEXT("_IElock.dll"));
					if (FALSE != RetValue)
						dwPid.push_back(pi.th32ProcessID);
				}

				bRet = Process32Next(hSnapshot, &pi);
			}
			
		
				
			}
	
		bRet = Process32Next(hSnapshot, &pi);

	if (dwCount <= 0)
	{
		CloseHandle(hSnapshot);
		return FALSE;
	}
	CloseHandle(hSnapshot);
	return dwCount;

}

//���ݽ���ID��������ģ��.�ж�DLL�Ƿ����.


BOOL CTerminateProcess::DoesDLLExistAnFindStrBySubStrName(DWORD dwPid,
	DWORD Begin,
	DWORD end,
	STRING ArgFindStrSubStrName)
{

	DWORD dwCount = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);//�����ȡ�˽���PID,����ʹ�ü���.
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		//MyOutputDebugStrig( = %d"),GetLastError());
		//WriteLogString(TEXT("DoesDLLExist CreateToolhelp32Snapshot Error Code = "),GetLastError(),NULL);
		return FALSE;
	}
	MODULEENTRY32 mi;
	mi.dwSize = sizeof(MODULEENTRY32); //��һ��ʹ�ñ����ʼ����Ա
	BOOL  bRet = Module32First(hSnapshot, &mi);
	multimap<STRING, STRING>::iterator it;
	while (bRet)
	{
		//MyOutputDebugStrig(TEXT("DoesDLLExis Module  = %ls ����pid = %d"), mi.szModule, dwPid);
		//����ģ��Map��.Ѱ���Ƿ�����ͬ��ģ��. �ҵ�֮����з����ҵ��ĸ���.
		STRING FindStr = mi.szModule;
		
		STRING SubStr = FindStr.substr(Begin, end);
		if (ArgFindStrSubStrName.compare(SubStr) == 0)
		{
			TCHAR szTempBuffer[1024] = { 0 };
			wsprintf(szTempBuffer,TEXT("DoesDLLExistAnFindStrBySubStrName Find Name is: %s"), FindStr.substr(Begin, end).c_str());
			OutputDebugString(szTempBuffer);
			dwCount++;
			
		}

		bRet = Module32Next(hSnapshot, &mi);
	}
	if (dwCount <= 0)
	{
		CloseHandle(hSnapshot);
		return FALSE;
	}
	CloseHandle(hSnapshot);
	return dwCount;
}
BOOL CTerminateProcess::DoesDLLExist(DWORD dwPid,LPVOID AdditionalParameter)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);//�����ȡ�˽���PID,����ʹ�ü���.
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		//MyOutputDebugStrig( = %d"),GetLastError());
		//WriteLogString(TEXT("DoesDLLExist CreateToolhelp32Snapshot Error Code = "),GetLastError(),NULL);
		return FALSE;
	}
	MODULEENTRY32 mi;
	mi.dwSize = sizeof(MODULEENTRY32); //��һ��ʹ�ñ����ʼ����Ա
	BOOL  bRet = Module32First(hSnapshot, &mi);
	DWORD dwCount = 0;
	multimap<STRING, STRING>::iterator it;
	
	
	while (bRet)
	{
		//MyOutputDebugStrig(TEXT("DoesDLLExis Module  = %ls ����pid = %d"), mi.szModule, dwPid);
		//����ģ��Map��.Ѱ���Ƿ�����ͬ��ģ��. �ҵ�֮����з����ҵ��ĸ���.
		
		it = m_SpecifyAModuleName->find(mi.szModule);
		
		if (it != m_SpecifyAModuleName->end()) //�����ҵ���
		{
			MyOutputDebugStrig(TEXT("Module  = %ls DoesDLLExist �ҵ��� Valueֵ = %ls ����pid = %d"), mi.szModule, it->second.c_str(),dwPid);
			//OutdwPid.push_back(dwPid); //�ҵ���PID���б���.
			// TCHAR szTempBuffer[1024] = { 0 };
			//TCHAR szTempBuffer[1024] = { 0 };
			//wsprintf(szTempBuffer, TEXT("FindName is : %s"), *it->second.c_str());
			//OutputDebugString(szTempBuffer);
			dwCount++; //����ѭ��.
		}

		bRet = Module32Next(hSnapshot, &mi);
	}
	if (dwCount <= 0)
	{
		//�����ض�ͷ���ַ�������Ѱ��
		CloseHandle(hSnapshot);
		return FALSE;
		
	}
	CloseHandle(hSnapshot);
	return dwCount;   //���ز��ҵ��ĸ���.
}

//����ָ���Ľ���,��������ģ��.��ȡ��ģ������. 1.�ڴ��ȡ. 2.�ļ���ȡ.
BOOL CTerminateProcess::ProcessDLLFeatureDetection(DWORD dwPid, LPVOID AdditionalParameter)
{

	return FALSE;
}

BOOL CTerminateProcess::DetectionStartAllProcessName(OUT vector<DWORD>& dwPid, LPVOID AdditionalParameter)
{


	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		//MyOutputDebugStrig(TEXT("DetectionStartAllProcessName CreateToolhelp32Snapshot Error Code = %d"), GetLastError());
		//WriteLogString(TEXT("DetectionStartAllProcessName CreateToolhelp32Snapshot Error Code = %d"), GetLastError(),NULL);
		return FALSE;
	}
	PROCESSENTRY32 pi;
	pi.dwSize = sizeof(PROCESSENTRY32); //��һ��ʹ�ñ����ʼ����Ա
	BOOL bRet = Process32First(hSnapshot, &pi);

	map<STRING, STRING>::iterator it;

	
	DWORD dwCount = 0;
	while (bRet)
	{
		//�����ض��Ľ�����

		BOOL RetValue = FALSE;
		
		RetValue = DoesDLLExist(pi.th32ProcessID,NULL); //��������ȥ����
		if (FALSE != RetValue)
		{
			dwPid.push_back(pi.th32ProcessID);
		}
		
		/*
		STRING str1 = pi.szExeFile;
		STRING str2 = TEXT("TestLoadDLL.exe");
			RetValue = DoesDLLExistAnFindStrBySubStrName(pi.th32ProcessID, 2, 11, TEXT("_IElock.dll"));
			if (FALSE != RetValue)
				dwPid.push_back(pi.th32ProcessID);
		
		*/
		bRet = Process32Next(hSnapshot, &pi);
	}
	CloseHandle(hSnapshot);
	if (dwPid.empty())
	{
		return FALSE;
	}

	return TRUE;
}



LPVOID CTerminateProcess::GetCallProc(const TCHAR* SpecifyDllName, LPCSTR SpecifyProcName, LPVOID AdditionalParameter)const
{

	HMODULE h_Module = LoadLibrary(SpecifyDllName);
	if (NULL == h_Module)
		return NULL;


	return GetProcAddress(h_Module, SpecifyProcName);
}

BOOL CTerminateProcess::EnableSuspendAnResumeProcess(DWORD dwPid,BOOL IsSupendProcess, LPVOID AdditionalParameter)
{
	if (NULL == dwPid)
	{
		return FALSE;
	}
	AdjustPrivileges();//����Ȩ��

	HANDLE hProcess = NULL;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (NULL == hProcess)
		return FALSE;

	if (IsSupendProcess == TRUE)
	{
		m_PfnNtSuspendProcess = (NtSuspendProcess)GetCallProc(TEXT("ntdll.dll"), "NtSuspendProcess",NULL);
		if (NULL == m_PfnNtSuspendProcess)
			return FALSE;
		m_PfnNtSuspendProcess(hProcess);
		CloseHandle(hProcess);
		return TRUE;
	}
	else
	{
		m_PfnNtResumeProcess = (NtResumeProcess)GetCallProc(TEXT("ntdll.dll"), "NtResumeProcess", NULL);
		if (NULL == m_PfnNtResumeProcess)
			return FALSE;
		m_PfnNtResumeProcess(hProcess);
		CloseHandle(hProcess);
		return TRUE;
	}
	return FALSE;
}

