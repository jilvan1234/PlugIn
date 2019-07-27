// EnumSysHandleTableToolsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <Tlhelp32.h>
#include "EnumSysHandleTableTools.h"
#include "EnumSysHandleTableToolsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//全局变量
NTQUERYSYSTEMINFORMATION NtQuerySystemInformation;
NTQUERYINFORMATIONFILE   NtQueryInformationFile;
NTQUERYOBJECT			 NtQueryObject;


// CEnumSysHandleTableToolsDlg 对话框
CEnumSysHandleTableToolsDlg::CEnumSysHandleTableToolsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEnumSysHandleTableToolsDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEnumSysHandleTableToolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_PidCombox);
	DDX_Control(pDX, IDC_LIST1, m_HandleResult);
	DDX_Control(pDX, IDC_LIST2, m_FileInfo);
}

BEGIN_MESSAGE_MAP(CEnumSysHandleTableToolsDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CEnumSysHandleTableToolsDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CEnumSysHandleTableToolsDlg::OnCbnSelchangeCombo1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST2, &CEnumSysHandleTableToolsDlg::OnNMRClickList2)
	ON_COMMAND(IDM_UNLOCKFILE, &CEnumSysHandleTableToolsDlg::OnUnlockfile)
END_MESSAGE_MAP()


// CEnumSysHandleTableToolsDlg 消息处理程序

BOOL CEnumSysHandleTableToolsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	this->hHeap = NULL;
	this->pBuffer = NULL;
	HINSTANCE ntdll_dll = GetModuleHandle((LPCWSTR)L"ntdll.dll");   
	if ( ntdll_dll!=NULL )   
	{   
		NtQuerySystemInformation = (NTQUERYSYSTEMINFORMATION)GetProcAddress(ntdll_dll, "NtQuerySystemInformation");
		NtQueryInformationFile = (NTQUERYINFORMATIONFILE)GetProcAddress(ntdll_dll,"NtQueryInformationFile");
		NtQueryObject = (NTQUERYOBJECT)GetProcAddress(ntdll_dll,"NtQueryObject");
	}
	//加载菜单资源
	if(!m_Menu1.LoadMenu(IDR_MENU1)){
		AfxMessageBox(_T("加载菜单资源失败!"));
	}
	pm_Menu1 = m_Menu1.GetSubMenu(0);
	//初始化句柄信息结果列表空间
	this->CurProcessId = 0;
	m_HandleResult.InsertColumn(0,_T("句柄对象类型"),LVCFMT_LEFT,120);
	m_HandleResult.InsertColumn(1,_T("句柄值"),LVCFMT_LEFT,100);
	m_HandleResult.InsertColumn(2,_T("对象指针值"),LVCFMT_LEFT,100);
	m_HandleResult.InsertColumn(3,_T("访问权限"),LVCFMT_LEFT,100);
	m_HandleResult.InsertColumn(4,_T("标记"),LVCFMT_LEFT,100);
	m_FileInfo.InsertColumn(0,_T("对象类型"),LVCFMT_LEFT,120);
	m_FileInfo.InsertColumn(1,_T("句柄值"),LVCFMT_LEFT,100);
	m_FileInfo.InsertColumn(2,_T("对象名称"),LVCFMT_LEFT,500);
	m_FileInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP);
	m_HandleResult.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_HEADERDRAGDROP);
	//提升进程权限
	if(!UpProcessPriority()){
		AfxMessageBox(TEXT("提升权限失败"));
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CEnumSysHandleTableToolsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CEnumSysHandleTableToolsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

BOOL CEnumSysHandleTableToolsDlg :: UpProcessPriority()
{
	HANDLE hTokenHandle = NULL;
	TOKEN_PRIVILEGES TokenPrivileges;
	BOOL result = FALSE;
	//打开进程权限令牌
	result = ::OpenProcessToken(::GetCurrentProcess(),TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES,&hTokenHandle);
	if(result){
		result = ::LookupPrivilegeValueW(NULL,SE_DEBUG_NAME,&TokenPrivileges.Privileges[0].Luid);
		if(result){
			TokenPrivileges.PrivilegeCount = 1;
			TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			//提升进程权限
			result = ::AdjustTokenPrivileges(hTokenHandle,FALSE,&TokenPrivileges,0,NULL,0);
		}
	}
	if(hTokenHandle){
		::CloseHandle(hTokenHandle);
	}
	return result;
}

//当用户拖动最小化窗口时系统调用此函数取得光标
HCURSOR CEnumSysHandleTableToolsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CEnumSysHandleTableToolsDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	PSYSTEM_HANDLE_INFORMATION pInfo = NULL;
	CString Buffer;
	HANDLE hModule = NULL;
	MODULEENTRY32 ModuleEntry32;
	INT LastPid = 4;
	ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
	if(pBuffer != NULL && hHeap){
		HeapFree(hHeap,HEAP_GENERATE_EXCEPTIONS,pBuffer);
	}
	pBuffer = GetInfoTable(SystemHandleInformation);
	if(pBuffer != NULL)
	{
		m_PidCombox.ResetContent();
		pInfo = (PSYSTEM_HANDLE_INFORMATION)pBuffer;
		for(unsigned long i = 0; i <pInfo->Count;i++)
		{
			if((INT)pInfo->Handle[i].uIdProcess != LastPid){
				hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pInfo->Handle[i].uIdProcess);
				if(hModule != INVALID_HANDLE_VALUE){
					Module32First(hModule,&ModuleEntry32);
				}
				Buffer.Format(_T("%08d*%s"),pInfo->Handle[i].uIdProcess,ModuleEntry32.szExePath);
				m_PidCombox.AddString(Buffer);
				ZeroMemory(&ModuleEntry32,sizeof(MODULEENTRY32));
				ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
				LastPid = pInfo->Handle[i].uIdProcess;
			}
		}
	}
	return ;
}

PVOID CEnumSysHandleTableToolsDlg::GetInfoTable(SYSTEM_INFORMATION_CLASS InfoClass)
{
	ULONG pInfo;
	ULONG ReturnBytes = 0;
	NTSTATUS status;
	if(hHeap == NULL){
		hHeap = GetProcessHeap();
	}
	if(hHeap){
		pInfo = (ULONG)HeapAlloc(hHeap,HEAP_GENERATE_EXCEPTIONS,0x1000);
		//第一次调用获取所需空间大小
		status = NtQuerySystemInformation(InfoClass,(PVOID)pInfo,0x1000,&ReturnBytes);
		if(STATUS_INFO_LENGTH_MISMATCH == status)
		{
			HeapFree(hHeap,HEAP_GENERATE_EXCEPTIONS,(PVOID)pInfo);
			while(status != STATUS_SUCCESS)//多次调用
			{
				pInfo = (ULONG)HeapAlloc(hHeap,HEAP_GENERATE_EXCEPTIONS,ReturnBytes);
				//继续调用看当前提交空间是否足够(可能是系统句柄表大小会变)
				status = NtQuerySystemInformation(InfoClass,(PVOID)pInfo,ReturnBytes,&ReturnBytes);
				if(status == STATUS_INFO_LENGTH_MISMATCH)
				{
					HeapFree(hHeap,HEAP_GENERATE_EXCEPTIONS,(PVOID)pInfo);
				}
			}
		}
		if(status == STATUS_SUCCESS)
		{
			return (PVOID)pInfo;
		}
	}
	return NULL;
}

VOID CEnumSysHandleTableToolsDlg::GetSpecialHandleInfo(PSYSTEM_HANDLE pInfo,HANDLE hFile,BOOL DupFileSuccess)
{
	CString Buffer;
	WCHAR DeviceName[MAX_PATH];
	WCHAR Drive[3] = L"A:";
	BOOL FileObOrNot = FALSE;
	UINT nRow_HandleInfo = NULL,nRow_FileInfo = NULL;
	ULONG Length = 0;
	if(pInfo){
		switch(pInfo->ObjectType){
			case OB_TYPE_ADAPTER:
				Buffer.Format(_T("Adapter"));
				break;
			case OB_TYPE_CALLBACK:
				Buffer.Format(_T("CallBack"));
				break;
			case OB_TYPE_CONTROLLER:
				Buffer.Format(_T("Controler"));
				break;
			case OB_TYPE_UNKNOWN:
				Buffer.Format(_T("Unknown"));
				break;
			case OB_TYPE_TYPE:
				Buffer.Format(_T("Type"));
				break;
			case OB_TYPE_DIRECTORY:
				Buffer.Format(_T("Directory"));
				break;
			case OB_TYPE_SYMBOLIC_LINK:
				Buffer.Format(_T("Symbolic Link"));
				break;
			case OB_TYPE_TOKEN:
				Buffer.Format(_T("Token"));
				break;
			case OB_TYPE_PROCESS:
				Buffer.Format(_T("Process"));
				break;
			case OB_TYPE_THREAD:
				Buffer.Format(_T("Thread"));
				break;
			case OB_TYPE_JOB:
				Buffer.Format(_T("Job"));
				break;
			case OB_TYPE_DEBUG_OBJECT:
				Buffer.Format(_T("DebugObj"));
				break;
			case OB_TYPE_EVENT:
				Buffer.Format(_T("Event"));
				break;
			case OB_TYPE_EVENT_PAIR:
				Buffer.Format(_T("Event pair"));
				break;
			case OB_TYPE_MUTANT:
				Buffer.Format(_T("Mutant"));
				break;   
			case OB_TYPE_SEMAPHORE:
				Buffer.Format(_T("Semaphore"));
				break;
			case OB_TYPE_TIMER:
				Buffer.Format(_T("Timer"));
				break;
			case OB_TYPE_PROFILE:
				Buffer.Format(_T("ProFile"));
				break;
			case OB_TYPE_KEYED_EVENT:
				Buffer.Format(_T("Keyed_Event"));
				break;
			case OB_TYPE_WINDOWS_STATION:
				Buffer.Format(_T("Windows Station"));
				break;
			case OB_TYPE_DESKTOP:
				Buffer.Format(_T("DeskTop"));
				break;
			case OB_TYPE_SECTION:
				Buffer.Format(_T("Section"));
				break;
			case OB_TYPE_KEY:
				Buffer.Format(_T("Key"));
				break;
			case OB_TYPE_PORT:
				Buffer.Format(_T("Port"));
				break;
			case OB_TYPE_WAITABLE_PORT:
				Buffer.Format(_T("Waitable Port"));
				break;
			case OB_TYPE_DEVICE:
				Buffer.Format(_T("Device"));
				break;
			case OB_TYPE_DRIVER:
				Buffer.Format(_T("Dirver"));
				break;
			case OB_TYPE_IOCOMPLETION:
				Buffer.Format(_T("IoCompletion"));
				break;
			case OB_TYPE_FILE:
				Buffer.Format(_T("File"));
				FileObOrNot = TRUE;
				break;
			case OB_TYPE_WMIGUID:
				Buffer.Format(_T("Wmiguid"));
				break;
		}
		if(!FileObOrNot){
			nRow_HandleInfo = m_HandleResult.InsertItem(0,Buffer);
			Buffer.Format(_T("0x%08x"),pInfo->Handle);
			m_HandleResult.SetItemText(nRow_HandleInfo,1,Buffer);
			Buffer.Format(_T("0x%08x"),pInfo->pObject);
			m_HandleResult.SetItemText(nRow_HandleInfo,2,Buffer);
			Buffer.Format(_T("0x%08x"),pInfo->GrantedAccess);
			m_HandleResult.SetItemText(nRow_HandleInfo,3,Buffer);
			Buffer.Format(_T("0x%08x"),pInfo->Flags);
			m_HandleResult.SetItemText(nRow_HandleInfo,4,Buffer);
		}else if(FileObOrNot && DupFileSuccess){
			nRow_FileInfo = m_FileInfo.InsertItem(0,Buffer);
			Buffer.Format(_T("%08d"),pInfo->Handle);
			m_FileInfo.SetItemText(nRow_FileInfo,1,Buffer);
			Buffer.Empty();
			if(hHeap){
				PINTHREAD_PARAM pInParam= (PINTHREAD_PARAM)HeapAlloc(hHeap,HEAP_GENERATE_EXCEPTIONS,sizeof(INTHREAD_PARAM));
				if(pInParam){
					ZeroMemory(pInParam,sizeof(INTHREAD_PARAM));
					GetFileName(hFile,pInParam);
					Buffer = pInParam->pObNameInfo->Buffer;
					for(ULONG i = 0; i<26 ;i++){
						Drive[0] += (WCHAR)i;
						if(::QueryDosDeviceW(Drive,DeviceName,MAX_PATH) && -1 != Buffer.Find(DeviceName)){
							Buffer.Replace(DeviceName,Drive);
							break;
						}
						Drive[0] = 'A';
					}
					HeapFree(hHeap,HEAP_GENERATE_EXCEPTIONS,pInParam);
				}
			}
			m_FileInfo.SetItemText(nRow_FileInfo,2,Buffer);
			CloseHandle(hFile);
		}
		Buffer.Empty();
	}
}

//查询文件名线程函数
DWORD WINAPI QueryObjectNameInfoThread(LPVOID lpParam)
{
	PINTHREAD_PARAM pInParam = (PINTHREAD_PARAM)lpParam;
	ULONG Length = 0;
	NTSTATUS status;
	OBJECT_NAME_INFORMATION ObNameInfo,*pObNameInfo = NULL;
	if( STATUS_BUFFER_OVERFLOW== NtQueryObject(pInParam->hFile,OB_TYPE_TYPE,&ObNameInfo,sizeof(OBJECT_NAME_INFORMATION),&Length)){
		pObNameInfo = (POBJECT_NAME_INFORMATION)HeapAlloc(pInParam->hHeap,HEAP_GENERATE_EXCEPTIONS,Length);
		pInParam->pObNameInfo = pObNameInfo;
		status = NtQueryObject(pInParam->hFile,OB_TYPE_TYPE,pInParam->pObNameInfo,Length,&Length);
	}
	return 0;
}
//解析文件对象名
VOID CEnumSysHandleTableToolsDlg::GetFileName(HANDLE hFile,PINTHREAD_PARAM pInParam)
{
	HANDLE hThread;
	if(hHeap){
		pInParam->hFile = hFile;
		pInParam->hHeap = hHeap;
		pInParam->pObNameInfo = NULL;
		hThread = CreateThread(NULL,0,QueryObjectNameInfoThread,pInParam,0,NULL);
		if(WaitForSingleObject(hThread,INFINITE) == WAIT_TIMEOUT){
			TerminateThread(hThread,0);
		}
		CloseHandle(hThread);
	}
}

void CEnumSysHandleTableToolsDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	PSYSTEM_HANDLE_INFORMATION pInfo = NULL;
	CString Buffer;
	ULONG SpecialPid = 0;
	HANDLE hProcess = NULL,hFile = NULL;
	BOOL DupFileSuccess = FALSE;
 	m_HandleResult.DeleteAllItems();//清空列表
	m_FileInfo.DeleteAllItems();//清空列表
 	m_PidCombox.GetLBText(m_PidCombox.GetCurSel(),Buffer);
	Buffer.Replace('*','\0');
	SpecialPid = _ttoi(Buffer);
	CurProcessId = SpecialPid;
	pInfo = (PSYSTEM_HANDLE_INFORMATION)pBuffer;
	for(unsigned long i = 0; i <pInfo->Count;i++){
		if(pInfo->Handle[i].uIdProcess == SpecialPid){
			if(pInfo->Handle[i].ObjectType == OB_TYPE_FILE){
				hProcess = OpenProcess(PROCESS_DUP_HANDLE,NULL,pInfo->Handle[i].uIdProcess);
				if(hProcess){
					//拷贝文件句柄
					if(DuplicateHandle(hProcess,(HANDLE)pInfo->Handle[i].Handle,GetCurrentProcess(),&hFile,0,FALSE,DUPLICATE_SAME_ACCESS)){
						DupFileSuccess = TRUE;
					}
					CloseHandle(hProcess);
				}
			}
			GetSpecialHandleInfo(&pInfo->Handle[i],hFile,DupFileSuccess);
		}
	}
	return ;
}

void CEnumSysHandleTableToolsDlg::OnNMRClickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE *>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CPoint pot;
	GetCursorPos(&pot);//获取鼠标当前位置
	pm_Menu1->TrackPopupMenu(TPM_LEFTALIGN,pot.x,pot.y,this);//在鼠标位置弹出菜单
	*pResult = 0;
}

void CEnumSysHandleTableToolsDlg::OnUnlockfile()
{
	// TODO: 在此添加命令处理程序代码
	CString Buffer;
	HANDLE RemoteHandle = NULL;
	int nRow = m_FileInfo.GetSelectionMark();
	if(nRow != -1){
		Buffer = m_FileInfo.GetItemText(nRow,1);
		RemoteHandle = (HANDLE)_ttoi(Buffer);
		if(CloseRemoteProcessHandle(RemoteHandle)){
			m_FileInfo.DeleteItem(nRow);
			AfxMessageBox(_T("解锁文件成功!"));
		}else{
			AfxMessageBox(_T("解锁文件失败!"));
		}
	}
}

BOOL CEnumSysHandleTableToolsDlg::CloseRemoteProcessHandle(HANDLE hRemoteHandle)
{
	BOOL result = FALSE;
	HANDLE hRemoteThread = NULL;
	HMODULE hKernel32Module = NULL;
	HANDLE hTargetProcess = NULL;
	hTargetProcess = OpenProcess(PROCESS_CREATE_THREAD|PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ,FALSE,CurProcessId);
	hKernel32Module = LoadLibrary(_T("kernel32.dll"));
	if(hTargetProcess && hKernel32Module){
		hRemoteThread = CreateRemoteThread(hTargetProcess,0,0,(LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32Module,"CloseHandle"),hRemoteHandle,0,NULL);
		if(hRemoteThread == NULL){
			result = FALSE;
			goto Error;
		}
		if(WaitForSingleObject(hRemoteThread,2000) == WAIT_OBJECT_0){
			result = TRUE;
		}else{
			result = FALSE;
		}
	}
Error:
	if(hRemoteThread){
		CloseHandle(hRemoteThread);
	}
	if(hTargetProcess){
		CloseHandle(hTargetProcess);
	}
	if(hKernel32Module){
		FreeLibrary(hKernel32Module);
	}
	return result;
}
