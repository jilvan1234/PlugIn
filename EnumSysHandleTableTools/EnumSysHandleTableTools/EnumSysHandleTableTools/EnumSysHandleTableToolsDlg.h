// EnumSysHandleTableToolsDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

//宏定义
typedef LONG NTSTATUS;   
#define STATUS_SUCCESS                  ((NTSTATUS)0x00000000L)   
#define STATUS_UNSUCCESSFUL             ((NTSTATUS)0xC0000001L)   
#define STATUS_INFO_LENGTH_MISMATCH     ((NTSTATUS)0xC0000004L)
#define STATUS_BUFFER_OVERFLOW			((NTSTATUS)0x80000005L)

typedef struct _SYSTEM_HANDLE{
ULONG        uIdProcess;
UCHAR        ObjectType; 
UCHAR        Flags;
USHORT       Handle;
PVOID		 pObject;
ACCESS_MASK GrantedAccess;
}SYSTEM_HANDLE,*PSYSTEM_HANDLE;

//需要用到的头文件
typedef struct _SYSTEM_HANDLE_INFORMATION
{
	ULONG Count;
	SYSTEM_HANDLE Handle[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION; 

typedef struct _OBJECT_NAME_INFORMATION{
	WORD Length;
	WORD MaximumLength;
	LPWSTR Buffer;
}OBJECT_NAME_INFORMATION,*POBJECT_NAME_INFORMATION;

typedef struct _FILE_NAME_INFORMATION{
	ULONG FileNameLength;
	WCHAR FileName[1];
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

typedef struct _NM_INFO
{
	HANDLE   hFile;
	FILE_NAME_INFORMATION Info;
	WCHAR Name[MAX_PATH];
}NM_INFO, *PNM_INFO; 

typedef struct _INTHREAD_PARAM
{
	HANDLE hFile;
	HANDLE hHeap;
	POBJECT_NAME_INFORMATION pObNameInfo;
}INTHREAD_PARAM,*PINTHREAD_PARAM;

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _IO_STATUS_BLOCK{
	union {
		NTSTATUS Status;
		PVOID Pointer;
	};
	ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;


typedef enum _SYSTEM_INFORMATION_CLASS   
{   
	SystemBasicInformation,                 //  0 Y N   
	SystemProcessorInformation,             //  1 Y N   
	SystemPerformanceInformation,           //  2 Y N   
	SystemTimeOfDayInformation,             //  3 Y N   
	SystemNotImplemented1,                  //  4 Y N   
	SystemProcessesAndThreadsInformation,   //  5 Y N   
	SystemCallCounts,                       //  6 Y N   
	SystemConfigurationInformation,         //  7 Y N   
	SystemProcessorTimes,                   //  8 Y N   
	SystemGlobalFlag,                       //  9 Y Y   
	SystemNotImplemented2,                  // 10 Y N   
	SystemModuleInformation,                // 11 Y N   
	SystemLockInformation,                  // 12 Y N   
	SystemNotImplemented3,                  // 13 Y N   
	SystemNotImplemented4,                  // 14 Y N   
	SystemNotImplemented5,                  // 15 Y N   
	SystemHandleInformation,                // 16 Y N   
	SystemObjectInformation,                // 17 Y N   
	SystemPagefileInformation,              // 18 Y N   
	SystemInstructionEmulationCounts,       // 19 Y N   
	SystemInvalidInfoClass1,                // 20   
	SystemCacheInformation,                 // 21 Y Y   
	SystemPoolTagInformation,               // 22 Y N   
	SystemProcessorStatistics,              // 23 Y N   
	SystemDpcInformation,                   // 24 Y Y   
	SystemNotImplemented6,                  // 25 Y N   
	SystemLoadImage,                        // 26 N Y   
	SystemUnloadImage,                      // 27 N Y   
	SystemTimeAdjustment,                   // 28 Y Y   
	SystemNotImplemented7,                  // 29 Y N   
	SystemNotImplemented8,                  // 30 Y N   
	SystemNotImplemented9,                  // 31 Y N   
	SystemCrashDumpInformation,             // 32 Y N   
	SystemExceptionInformation,             // 33 Y N   
	SystemCrashDumpStateInformation,        // 34 Y Y/N   
	SystemKernelDebuggerInformation,        // 35 Y N   
	SystemContextSwitchInformation,         // 36 Y N   
	SystemRegistryQuotaInformation,         // 37 Y Y   
	SystemLoadAndCallImage,                 // 38 N Y   
	SystemPrioritySeparation,               // 39 N Y   
	SystemNotImplemented10,                 // 40 Y N   
	SystemNotImplemented11,                 // 41 Y N   
	SystemInvalidInfoClass2,                // 42   
	SystemInvalidInfoClass3,                // 43   
	SystemTimeZoneInformation,              // 44 Y N   
	SystemLookasideInformation,             // 45 Y N   
	SystemSetTimeSlipEvent,                 // 46 N Y   
	SystemCreateSession,                    // 47 N Y   
	SystemDeleteSession,                    // 48 N Y   
	SystemInvalidInfoClass4,                // 49   
	SystemRangeStartInformation,            // 50 Y N   
	SystemVerifierInformation,              // 51 Y Y   
	SystemAddVerifier,                      // 52 N Y   
	SystemSessionProcessesInformation       // 53 Y N   

} SYSTEM_INFORMATION_CLASS;  

typedef enum _FILE_INFORMATION_CLASS {
	// end_wdm
	FileDirectoryInformation         = 1,
	FileFullDirectoryInformation,   // 2
	FileBothDirectoryInformation,   // 3
	FileBasicInformation,           // 4   wdm
	FileStandardInformation,        // 5   wdm
	FileInternalInformation,        // 6
	FileEaInformation,              // 7
	FileAccessInformation,          // 8
	FileNameInformation,            // 9
	FileRenameInformation,          // 10
	FileLinkInformation,            // 11
	FileNamesInformation,           // 12
	FileDispositionInformation,     // 13
	FilePositionInformation,        // 14 wdm
	FileFullEaInformation,          // 15
	FileModeInformation,            // 16
	FileAlignmentInformation,       // 17
	FileAllInformation,             // 18
	FileAllocationInformation,      // 19
	FileEndOfFileInformation,       // 20 wdm
	FileAlternateNameInformation,   // 21
	FileStreamInformation,          // 22
	FilePipeInformation,            // 23
	FilePipeLocalInformation,       // 24
	FilePipeRemoteInformation,      // 25
	FileMailslotQueryInformation,   // 26
	FileMailslotSetInformation,     // 27
	FileCompressionInformation,     // 28
	FileObjectIdInformation,        // 29
	FileCompletionInformation,      // 30
	FileMoveClusterInformation,     // 31
	FileQuotaInformation,           // 32
	FileReparsePointInformation,    // 33
	FileNetworkOpenInformation,     // 34
	FileAttributeTagInformation,    // 35
	FileTrackingInformation,        // 36
	FileIdBothDirectoryInformation, // 37
	FileIdFullDirectoryInformation, // 38
	FileValidDataLengthInformation, // 39
	FileShortNameInformation,       // 40
	FileMaximumInformation
	// begin_wdm
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

typedef enum _OBJECT_INFORMATION_CLASS {
	ObjectBasicInformation, // 0 Y N
	ObjectNameInformation, // 1 Y N
	ObjectTypeInformation, // 2 Y N
	ObjectAllTypesInformation, // 3 Y N
	ObjectHandleInformation // 4 Y Y
} OBJECT_INFORMATION_CLASS;

typedef enum _SYSTEM_HANDLE_TYPE
{
	OB_TYPE_UNKNOWN=0,   //0
	OB_TYPE_TYPE,    // 1,fixed
	OB_TYPE_DIRECTORY,   // 2,fixed
	OB_TYPE_SYMBOLIC_LINK, // 3,fixed
	OB_TYPE_TOKEN,    // 4,fixed
	OB_TYPE_PROCESS,   // 5,fixed
	OB_TYPE_THREAD,    // 6,fixed
	OB_TYPE_JOB,    // 7,fixed
	OB_TYPE_DEBUG_OBJECT, // 8,fixed
	OB_TYPE_EVENT,    // 9,fixed
	OB_TYPE_EVENT_PAIR,   //10,fixed
	OB_TYPE_MUTANT,    //11,fixed
	OB_TYPE_CALLBACK,   //12,fixed
	OB_TYPE_SEMAPHORE,   //13,fixed
	OB_TYPE_TIMER,    //14,fixed
	OB_TYPE_PROFILE,   //15,fixed
	OB_TYPE_KEYED_EVENT, //16,fixed
	OB_TYPE_WINDOWS_STATION,//17,fixed
	OB_TYPE_DESKTOP,   //18,fixed
	OB_TYPE_SECTION,   //19,fixed
	OB_TYPE_KEY,    //20,fixed
	OB_TYPE_PORT,    //21,fixed 
	OB_TYPE_WAITABLE_PORT, //22,fixed
	OB_TYPE_ADAPTER,   //23,fixed
	OB_TYPE_CONTROLLER,   //24,fixed
	OB_TYPE_DEVICE,    //25,fixed
	OB_TYPE_DRIVER,    //26,fixed
	OB_TYPE_IOCOMPLETION, //27,fixed
	OB_TYPE_FILE,    //28,fixed
	OB_TYPE_WMIGUID    //29,fixed
}SYSTEM_HANDLE_TYPE;

//系统函数定义
typedef NTSTATUS (WINAPI *NTQUERYSYSTEMINFORMATION)(IN SYSTEM_INFORMATION_CLASS, IN OUT PVOID, IN ULONG, OUT PULONG OPTIONAL);
typedef NTSTATUS (WINAPI *NTQUERYINFORMATIONFILE)(IN HANDLE FileHandle,OUT PIO_STATUS_BLOCK IoStatusBlock,OUT PVOID FileInformation,IN DWORD Length,IN FILE_INFORMATION_CLASS FileInformationClass);
typedef NTSTATUS (WINAPI *NTQUERYOBJECT)(HANDLE ObjectHandle,ULONG ObjectInformationClass,PVOID ObjectInformation,ULONG ObjectInformationLength,PULONG ReturnLength);

//线程函数定义
DWORD WINAPI QueryObjectNameInfoThread(LPVOID lpParam);

// CEnumSysHandleTableToolsDlg 对话框
class CEnumSysHandleTableToolsDlg : public CDialog
{
// 构造
public:
	CEnumSysHandleTableToolsDlg(CWnd* pParent = NULL);	// 标准构造函数
	// 对话框数据
	enum { IDD = IDD_ENUMSYSHANDLETABLETOOLS_DIALOG };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
// 实现
protected:
	HICON m_hIcon;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	// //进程ID控制对话框
	CComboBox m_PidCombox;
	// //句柄信息查询结果
	CListCtrl m_HandleResult;
	//获取句柄表函数
	PVOID GetInfoTable(SYSTEM_INFORMATION_CLASS InfoClass);
	//获取特定句柄项信息函数
	VOID  GetSpecialHandleInfo(PSYSTEM_HANDLE pInfo,HANDLE hFile,BOOL DupFileSuccess);
	//提升进程权限函数
	BOOL UpProcessPriority();
	//获取文件名函数
	VOID GetFileName(HANDLE hFile,PINTHREAD_PARAM pObjectNameInfo);
	//结束远程进程句柄
	BOOL CloseRemoteProcessHandle(HANDLE RemoteHandle);
private:
	HANDLE hHeap;
	DWORD CurProcessId;
	PVOID pBuffer;
public:
	afx_msg void OnCbnSelchangeCombo1();
	// 当前进程文件引起信息
	CListCtrl m_FileInfo;
	CMenu m_Menu1,*pm_Menu1;
	afx_msg void OnNMRClickList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUnlockfile();
};
