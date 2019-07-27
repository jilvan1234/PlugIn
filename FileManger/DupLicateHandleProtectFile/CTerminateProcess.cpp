#include "CTerminateProcess.h"



void __cdecl MyOutputDebugStrig(const _TCHAR* pszFormat, ...)
{
	_TCHAR buf[1024] = { 0 };
	// ZeroMemory( buf, 1024*sizeof(TCHAR ) );
	swprintf_s(buf, 1024, _T("线程ID = [%lu]"), GetCurrentThreadId());
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
	fwrite(PlogList->NowData.tmp2, wcslen(PlogList->NowData.tmp2) + sizeof(TCHAR), 1, pFile);  //写入时间
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

	//fprintf_s(pFile, lpParam); //写入到文件
	
	fwrite(PlogList->NowData.tmp0, strlen(PlogList->NowData.tmp0) + sizeof(TCHAR), 1, pFile);
	fwrite(PlogList->NowData.tmp1, strlen(PlogList->NowData.tmp1) + sizeof(TCHAR), 1, pFile);
	fwrite(PlogList->NowData.tmp2, strlen(PlogList->NowData.tmp2) + sizeof(TCHAR), 1, pFile);  //写入时间
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
		//OutputDebugString(TEXT("提升权限失败,LookupPrivilegeValue"));
		WriteLogString(TEXT("提升权限失败,LookupPrivilegeValue"),GetLastError(),NULL);
		return false;
	}
	ZeroMemory(&tp, sizeof(tp));
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	/* Adjust Token Privileges */
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize)) {
		CloseHandle(hToken);
		OutputDebugString(TEXT("提升权限失败 AdjustTokenPrivileges"));
		WriteLogString(TEXT("提升权限失败 AdjustTokenPrivileges"), GetLastError(),NULL);
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
	pi.dwSize = sizeof(PROCESSENTRY32); //第一次使用必须初始化成员
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
	1.首先遍历两个指定的进程.
	2.从两个进程中分别遍历出所有模块
	3.判断模块名字是否有 jt_IELock.dll
	    有的话结束这个进程
		没有的话继续遍历进程.寻找相同的.
		如果还没有.则读取DLL特征
		有的话进行结束
		没有的话结束.启用全进程遍历搜索模式.

	4.两个指定的进程没有找到
		遍历所有进程.并且遍历出所有模块.
		找到可以的模块
	
	*/

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		//MyOutputDebugStrig(TEXT("DetectionStartBySpecifyAName CreateToolhelp32Snapshot Error Code = %d"), GetLastError());
		//WriteLogString(TEXT("DetectionStartBySpecifyAName CreateToolhelp32Snapshot Error Code"), GetLastError(),NULL);
		return FALSE;
	}
	PROCESSENTRY32 pi;
	pi.dwSize = sizeof(PROCESSENTRY32); //第一次使用必须初始化成员
	BOOL bRet = Process32First(hSnapshot, &pi);

	map<STRING, STRING>::iterator it;

	DWORD dwCount = 0;
	while (bRet)
	{

		
			
			it = m_SpecifyAProcessName->find(pi.szExeFile);//传入key返回value的迭代器
			
			if (it != m_SpecifyAProcessName->end())        //找到了.
			{
				dwCount++;
				//MyOutputDebugStrig(TEXT("DetectionStartBySpecifyAName 找到的 Value值 = %ls pid = %d"), it->second.c_str(), pi.th32ProcessID);
				BOOL RetValue = FALSE;
				//找到进程之后,判断进程中是否有指定的DLL.返回找到模块的个数.
				
				RetValue =  DoesDLLExist(pi.th32ProcessID,NULL);
				if (FALSE != RetValue)
				{
					dwPid.push_back(pi.th32ProcessID); //保存一下pid.

				}

				
				RetValue = DoesDLLExist(pi.th32ProcessID, NULL); //根据名字去查找
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

//根据进程ID遍历进程模块.判断DLL是否存在.


BOOL CTerminateProcess::DoesDLLExistAnFindStrBySubStrName(DWORD dwPid,
	DWORD Begin,
	DWORD end,
	STRING ArgFindStrSubStrName)
{

	DWORD dwCount = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);//上面获取了进程PID,下面使用即可.
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		//MyOutputDebugStrig( = %d"),GetLastError());
		//WriteLogString(TEXT("DoesDLLExist CreateToolhelp32Snapshot Error Code = "),GetLastError(),NULL);
		return FALSE;
	}
	MODULEENTRY32 mi;
	mi.dwSize = sizeof(MODULEENTRY32); //第一次使用必须初始化成员
	BOOL  bRet = Module32First(hSnapshot, &mi);
	multimap<STRING, STRING>::iterator it;
	while (bRet)
	{
		//MyOutputDebugStrig(TEXT("DoesDLLExis Module  = %ls 进程pid = %d"), mi.szModule, dwPid);
		//遍历模块Map表.寻找是否有相同的模块. 找到之后进行返回找到的个数.
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
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);//上面获取了进程PID,下面使用即可.
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		//MyOutputDebugStrig( = %d"),GetLastError());
		//WriteLogString(TEXT("DoesDLLExist CreateToolhelp32Snapshot Error Code = "),GetLastError(),NULL);
		return FALSE;
	}
	MODULEENTRY32 mi;
	mi.dwSize = sizeof(MODULEENTRY32); //第一次使用必须初始化成员
	BOOL  bRet = Module32First(hSnapshot, &mi);
	DWORD dwCount = 0;
	multimap<STRING, STRING>::iterator it;
	
	
	while (bRet)
	{
		//MyOutputDebugStrig(TEXT("DoesDLLExis Module  = %ls 进程pid = %d"), mi.szModule, dwPid);
		//遍历模块Map表.寻找是否有相同的模块. 找到之后进行返回找到的个数.
		
		it = m_SpecifyAModuleName->find(mi.szModule);
		
		if (it != m_SpecifyAModuleName->end()) //代表找到了
		{
			MyOutputDebugStrig(TEXT("Module  = %ls DoesDLLExist 找到的 Value值 = %ls 进程pid = %d"), mi.szModule, it->second.c_str(),dwPid);
			//OutdwPid.push_back(dwPid); //找到的PID进行保存.
			// TCHAR szTempBuffer[1024] = { 0 };
			//TCHAR szTempBuffer[1024] = { 0 };
			//wsprintf(szTempBuffer, TEXT("FindName is : %s"), *it->second.c_str());
			//OutputDebugString(szTempBuffer);
			dwCount++; //继续循环.
		}

		bRet = Module32Next(hSnapshot, &mi);
	}
	if (dwCount <= 0)
	{
		//继续截断头部字符串进行寻找
		CloseHandle(hSnapshot);
		return FALSE;
		
	}
	CloseHandle(hSnapshot);
	return dwCount;   //返回查找到的个数.
}

//根据指定的进程,遍历所有模块.读取其模块特征. 1.内存读取. 2.文件读取.
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
	pi.dwSize = sizeof(PROCESSENTRY32); //第一次使用必须初始化成员
	BOOL bRet = Process32First(hSnapshot, &pi);

	map<STRING, STRING>::iterator it;

	
	DWORD dwCount = 0;
	while (bRet)
	{
		//查找特定的进程名

		BOOL RetValue = FALSE;
		
		RetValue = DoesDLLExist(pi.th32ProcessID,NULL); //根据名字去查找
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
	AdjustPrivileges();//提升权限

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

