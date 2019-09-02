
#pragma once
#include <windows.h>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
using namespace std;

#ifdef UNICODE
#define CBinString wstring
#else
#define CBinString string
#endif

#ifdef _WIN64

#define UBinSize DWORD64
#else
#define UBinSize DWORD

#endif

#include "HOOK/DetoursInclude/detours.h"
#include "HOOK/DetoursInclude/detver.h"


/*


#include "ClassManger/CNativeApi/CNativeApiManger.h"
#include "ProcessManger/ProcessIterator/CProcessOpt.h"
#include "ThreadManger/RemoteThread/CRemoteThread.h"
#include "ThreadManger/ShellCode/ShellCode/CShellCode.h"
#include "FileManger/CFileManger/CFileManger.h"
#include "RegManger/RegManger/CRegManger.h"



#ifdef _WIN64
#pragma comment(lib,"../../../PulicLib/x64/Release/CNativeApi.lib")
#pragma comment(lib,"../../../PulicLib/x64/Release/ProcessIterator.lib")
#pragma comment(lib,"../../../PulicLib/x64/Release/RegManger.lib")
#pragma comment(lib,"../../../PulicLib/x64/Release/CFileManger.lib")
#pragma comment(lib,"../../../PulicLib/x64/Release/RemoteThread.lib")
#else
#pragma comment(lib,"../../../PulicLib/Win32/Release/CNativeApi.lib")
#pragma comment(lib,"../../../PulicLib/Win32/Release/ProcessIterator.lib")
#pragma comment(lib,"../../../PulicLib/Win32/Release/RegManger.lib")
#pragma comment(lib,"../../../PulicLib/Win32/Release/CFileManger.lib")
#pragma comment(lib,"../../../PulicLib/Win32/Release/RemoteThread.lib")
#endif

*/

typedef LONG NTSTATUS;

#define STATUS_SUCCESS                  ((NTSTATUS)0x00000000L)   
#define STATUS_UNSUCCESSFUL             ((NTSTATUS)0xC0000001L)   
#define STATUS_NOT_IMPLEMENTED          ((NTSTATUS)0xC0000002L)   
#define STATUS_INVALID_INFO_CLASS       ((NTSTATUS)0xC0000003L)   
#define STATUS_INFO_LENGTH_MISMATCH     ((NTSTATUS)0xC0000004L)   

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)




//ZwQuerySystemInformation 需要用到的信息
typedef enum _SYSTEM_INFORMATION_CLASS
{
    SystemBasicInformation,                    //  0 Y N   
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


//ZwQuerySystemInformation 遍历系统所有句柄的句柄信息

//句柄的详细信息
typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
    USHORT UniqueProcessId;
    USHORT CreatorBackTraceIndex;
    UCHAR ObjectTypeIndex;
    UCHAR HandleAttributes;
    USHORT HandleValue;
    PVOID Object;
    ULONG GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

//全局句柄记录的个数
typedef struct _SYSTEM_HANDLE_INFORMATION
{
    ULONG NumberOfHandles;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

//********
typedef struct _LSA_UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;

} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING, UNICODE_STRING, *PUNICODE_STRING;

typedef struct _CLIENT_ID
{
    HANDLE UniqueProcess;
    HANDLE UniqueThread;

} CLIENT_ID;

typedef enum _THREAD_STATE
{
    StateInitialized,
    StateReady,
    StateRunning,
    StateStandby,
    StateTerminated,
    StateWait,
    StateTransition,
    StateUnknown

} THREAD_STATE;

typedef enum _KWAIT_REASON
{
    Executive,
    FreePage,
    PageIn,
    PoolAllocation,
    DelayExecution,
    Suspended,
    UserRequest,
    WrExecutive,
    WrFreePage,
    WrPageIn,
    WrPoolAllocation,
    WrDelayExecution,
    WrSuspended,
    WrUserRequest,
    WrEventPair,
    WrQueue,
    WrLpcReceive,
    WrLpcReply,
    WrVirtualMemory,
    WrPageOut,
    WrRendezvous,
    Spare2,
    Spare3,
    Spare4,
    Spare5,
    Spare6,
    WrKernel

} KWAIT_REASON;

/*typedef struct _IO_COUNTERS
{
    LARGE_INTEGER ReadOperationCount;   //I/O读操作数目
    LARGE_INTEGER WriteOperationCount;  //I/O写操作数目
    LARGE_INTEGER OtherOperationCount;  //I/O其他操作数目
    LARGE_INTEGER ReadTransferCount;    //I/O读数据数目
    LARGE_INTEGER WriteTransferCount;   //I/O写数据数目
    LARGE_INTEGER OtherTransferCount;   //I/O其他操作数据数目

} IO_COUNTERS, *PIO_COUNTERS;
  */
typedef struct _VM_COUNTERS
{
    ULONG PeakVirtualSize;              //虚拟存储峰值大小   
    ULONG VirtualSize;                  //虚拟存储大小   
    ULONG PageFaultCount;               //页故障数目   
    ULONG PeakWorkingSetSize;           //工作集峰值大小   
    ULONG WorkingSetSize;               //工作集大小   
    ULONG QuotaPeakPagedPoolUsage;      //分页池使用配额峰值   
    ULONG QuotaPagedPoolUsage;          //分页池使用配额   
    ULONG QuotaPeakNonPagedPoolUsage;   //非分页池使用配额峰值   
    ULONG QuotaNonPagedPoolUsage;       //非分页池使用配额   
    ULONG PagefileUsage;                //页文件使用情况   
    ULONG PeakPagefileUsage;            //页文件使用峰值   

} VM_COUNTERS, *PVM_COUNTERS;

typedef LONG KPRIORITY;

typedef struct _SYSTEM_THREADS
{
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    ULONG WaitTime;
    PVOID StartAddress;
    CLIENT_ID ClientId;
    KPRIORITY Priority;
    KPRIORITY BasePriority;
    ULONG ContextSwitchCount;
    THREAD_STATE State;
    KWAIT_REASON WaitReason;

} SYSTEM_THREADS, *PSYSTEM_THREADS;

typedef struct _SYSTEM_PROCESSES
{
    ULONG NextEntryDelta;
    ULONG ThreadCount;
    ULONG Reserved1[6];
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ProcessName;
    KPRIORITY BasePriority;
    ULONG ProcessId;
    ULONG InheritedFromProcessId;
    ULONG HandleCount;
    ULONG Reserved2[2];
    VM_COUNTERS  VmCounters;
    IO_COUNTERS IoCounters;
    SYSTEM_THREADS Threads[1];

} SYSTEM_PROCESSES, *PSYSTEM_PROCESSES;

typedef struct _SYSTEM_BASIC_INFORMATION
{
    BYTE Reserved1[24];
    PVOID Reserved2[4];
    CCHAR NumberOfProcessors;

} SYSTEM_BASIC_INFORMATION;

typedef struct tagSYSTEM_MODULE_INFORMATION {
    ULONG Reserved[2];
    PVOID Base;
    ULONG Size;
    ULONG Flags;
    USHORT Index;
    USHORT Unknown;
    USHORT LoadCount;
    USHORT ModuleNameOffset;
    CHAR ImageName[256];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;



//ZwQueryInformationProcess 遍历进程信息.

typedef enum _PROCESSINFOCLASS {
    ProcessBasicInformation = 0,
    ProcessQuotaLimits = 1,
    ProcessIoCounters = 2,
    ProcessVmCounters = 3,
    ProcessTimes = 4,
    ProcessBasePriority = 5,
    ProcessRaisePriority = 6,
    ProcessDebugPort = 7,
    ProcessExceptionPort = 8,
    ProcessAccessToken = 9,
    ProcessLdtInformation = 10,
    ProcessLdtSize = 11,
    ProcessDefaultHardErrorMode = 12,
    ProcessIoPortHandlers = 13,   // Note: this is kernel mode only
    ProcessPooledUsageAndLimits = 14,
    ProcessWorkingSetWatch = 15,
    ProcessUserModeIOPL = 16,
    ProcessEnableAlignmentFaultFixup = 17,
    ProcessPriorityClass = 18,
    ProcessWx86Information = 19,
    ProcessHandleCount = 20,
    ProcessAffinityMask = 21,
    ProcessPriorityBoost = 22,
    ProcessDeviceMap = 23,
    ProcessSessionInformation = 24,
    ProcessForegroundInformation = 25,
    ProcessWow64Information = 26,
    ProcessImageFileName = 27,
    ProcessLUIDDeviceMapsEnabled = 28,
    ProcessBreakOnTermination = 29,
    ProcessDebugObjectHandle = 30,
    ProcessDebugFlags = 31,
    ProcessHandleTracing = 32,
    ProcessIoPriority = 33,
    ProcessExecuteFlags = 34,
    ProcessTlsInformation = 35,
    ProcessCookie = 36,
    ProcessImageInformation = 37,
    ProcessCycleTime = 38,
    ProcessPagePriority = 39,
    ProcessInstrumentationCallback = 40,
    ProcessThreadStackAllocation = 41,
    ProcessWorkingSetWatchEx = 42,
    ProcessImageFileNameWin32 = 43,
    ProcessImageFileMapping = 44,
    ProcessAffinityUpdateMode = 45,
    ProcessMemoryAllocationMode = 46,
    ProcessGroupInformation = 47,
    ProcessTokenVirtualizationEnabled = 48,
    ProcessOwnerInformation = 49,
    ProcessWindowInformation = 50,
    ProcessHandleInformation = 51,
    ProcessMitigationPolicy = 52,
    ProcessDynamicFunctionTableInformation = 53,
    ProcessHandleCheckingMode = 54,
    ProcessKeepAliveCount = 55,
    ProcessRevokeFileHandles = 56,
    ProcessWorkingSetControl = 57,
    ProcessHandleTable = 58,
    ProcessCheckStackExtentsMode = 59,
    ProcessCommandLineInformation = 60,
    ProcessProtectionInformation = 61,
    ProcessMemoryExhaustion = 62,
    ProcessFaultInformation = 63,
    ProcessTelemetryIdInformation = 64,
    ProcessCommitReleaseInformation = 65,
    ProcessReserved1Information = 66,
    ProcessReserved2Information = 67,
    ProcessSubsystemProcess = 68,
    ProcessInPrivate = 70,
    ProcessRaiseUMExceptionOnInvalidHandleClose = 71,
    ProcessSubsystemInformation = 75,
    ProcessWin32kSyscallFilterInformation = 79,
    ProcessEnergyTrackingState = 82,
    MaxProcessInfoClass                             // MaxProcessInfoClass should always be the last enum
} PROCESSINFOCLASS;








//遍历系统信息





//NtQueryObject




typedef enum _OBJECT_INFORMATION_CLASS {
    ObjectBasicInformation = 0,  //如果为1表示获取名字信息
    ObjectFileInformation = 1,
    ObjectTypeInformation = 2    //表示获取类型信息
} OBJECT_INFORMATION_CLASS;
//返回长度

// NtQueryObject类型为0的信息结构体
typedef struct _PUBLIC_OBJECT_BASIC_INFORMATION {
    ULONG       Attributes;
    ACCESS_MASK GrantedAccess;
    ULONG       HandleCount;
    ULONG       PointerCount;
    ULONG       Reserved[10];
} PUBLIC_OBJECT_BASIC_INFORMATION, *PPUBLIC_OBJECT_BASIC_INFORMATION;
//查询句柄信息. 为1的结构体
typedef struct _OBJECT_NAME_INformATION {
    UNICODE_STRING Name;
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;





//为2的结构体

typedef enum _RFILE_INFORMATION_CLASS {
    FileDirectoryInformation1 = 1,
    FileFullDirectoryInformation,
    FileBothDirectoryInformation,
    FileBasicInformation,
    FileStandardInformation,
    FileInternalInformation,
    FileEaInformation,
    FileAccessInformation,
    FileNameInformation,
    FileRenameInformation,
    FileLinkInformation,
    FileNamesInformation,
    FileDispositionInformation,
    FilePositionInformation,
    FileFullEaInformation,
    FileModeInformation,
    FileAlignmentInformation,
    FileAllInformation,
    FileAllocationInformation,
    FileEndOfFileInformation,
    FileAlternateNameInformation,
    FileStreamInformation,
    FilePipeInformation,
    FilePipeLocalInformation,
    FilePipeRemoteInformation,
    FileMailslotQueryInformation,
    FileMailslotSetInformation,
    FileCompressionInformation,
    FileObjectIdInformation,
    FileCompletionInformation,
    FileMoveClusterInformation,
    FileQuotaInformation,
    FileReparsePointInformation,
    FileNetworkOpenInformation,
    FileAttributeTagInformation,
    FileTrackingInformation,
    FileIdBothDirectoryInformation,
    FileIdFullDirectoryInformation,
    FileValidDataLengthInformation,
    FileShortNameInformation,
    FileIoCompletionNotificationInformation,
    FileIoStatusBlockRangeInformation,
    FileIoPriorityHintInformation,
    FileSfioReserveInformation,
    FileSfioVolumeInformation,
    FileHardLinkInformation,
    FileProcessIdsUsingFileInformation,
    FileNormalizedNameInformation,
    FileNetworkPhysicalNameInformation,
    FileIdGlobalTxDirectoryInformation,
    FileIsRemoteDeviceInformation,
    FileUnusedInformation,
    FileNumaNodeInformation,
    FileStandardLinkInformation,
    FileRemoteProtocolInformation,
    FileRenameInformationBypassAccessCheck,
    FileLinkInformationBypassAccessCheck,
    FileVolumeNameInformation,
    FileIdInformation,
    FileIdExtdDirectoryInformation,
    FileReplaceCompletionInformation,
    FileHardLinkFullIdInformation,
    FileIdExtdBothDirectoryInformation,
    FileMaximumInformation
} RFILE_INFORMATION_CLASS, *PRFILE_INFORMATION_CLASS;


typedef _Enum_is_bitflag_ enum _POOL_TYPE {
    NonPagedPool,
    NonPagedPoolExecute = NonPagedPool,
    PagedPool,
    NonPagedPoolMustSucceed = NonPagedPool + 2,
    DontUseThisType,
    NonPagedPoolCacheAligned = NonPagedPool + 4,
    PagedPoolCacheAligned,
    NonPagedPoolCacheAlignedMustS = NonPagedPool + 6,
    MaxPoolType,

    //
    // Define base types for NonPaged (versus Paged) pool, for use in cracking
    // the underlying pool type.
    //

    NonPagedPoolBase = 0,
    NonPagedPoolBaseMustSucceed = NonPagedPoolBase + 2,
    NonPagedPoolBaseCacheAligned = NonPagedPoolBase + 4,
    NonPagedPoolBaseCacheAlignedMustS = NonPagedPoolBase + 6,

    //
    // Note these per session types are carefully chosen so that the appropriate
    // masking still applies as well as MaxPoolType above.
    //

    NonPagedPoolSession = 32,
    PagedPoolSession = NonPagedPoolSession + 1,
    NonPagedPoolMustSucceedSession = PagedPoolSession + 1,
    DontUseThisTypeSession = NonPagedPoolMustSucceedSession + 1,
    NonPagedPoolCacheAlignedSession = DontUseThisTypeSession + 1,
    PagedPoolCacheAlignedSession = NonPagedPoolCacheAlignedSession + 1,
    NonPagedPoolCacheAlignedMustSSession = PagedPoolCacheAlignedSession + 1,

    NonPagedPoolNx = 512,
    NonPagedPoolNxCacheAligned = NonPagedPoolNx + 4,
    NonPagedPoolSessionNx = NonPagedPoolNx + 32,

} _Enum_is_bitflag_ POOL_TYPE;


typedef struct _PUBLIC_OBJECT_TYPE_INFORMATION {
    UNICODE_STRING          TypeName;
    ULONG                   TotalNumberOfHandles;
    ULONG                   TotalNumberOfObjects;
    WCHAR                   Unused1[8];
    ULONG                   HighWaterNumberOfHandles;
    ULONG                   HighWaterNumberOfObjects;
    WCHAR                   Unused2[8];
    ACCESS_MASK             InvalidAttributes;
    GENERIC_MAPPING         GenericMapping;
    ACCESS_MASK             ValidAttributes;
    BOOLEAN                 SecurityRequired;
    BOOLEAN                 MaintainHandleCount;
    USHORT                  MaintainTypeList;
    POOL_TYPE               PoolType;
    ULONG                   DefaultPagedPoolCharge;
    ULONG                   DefaultNonPagedPoolCharge;
    BYTE                    Unknown2[16];
} PUBLIC_OBJECT_TYPE_INFORMATION, *PPUBLIC_OBJECT_TYPE_INFORMATION;





//遍历文件需要的信息

typedef struct _IO_STATUS_BLOCK {
#pragma warning(push)
#pragma warning(disable: 4201) // we'll always use the Microsoft compiler
    union {
        NTSTATUS Status;
        PVOID Pointer;
    } DUMMYUNIONNAME;
#pragma warning(pop)

    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;


typedef struct _FILE_NAME_INFORMATION {
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;





typedef NTSTATUS(WINAPI* PfnZwQueryInformationProcess)(
    _In_      HANDLE           ProcessHandle,
    _In_      PROCESSINFOCLASS ProcessInformationClass,         //根据类型信息获取不同的信息
    _Out_     PVOID            ProcessInformation,
    _In_      ULONG            ProcessInformationLength,
    _Out_opt_ PULONG           ReturnLength
    );

typedef NTSTATUS(WINAPI * PfnZwQueryObject)(
    _In_opt_ HANDLE Handle,                                 //句柄
    _In_ OBJECT_INFORMATION_CLASS ObjectInformationClass,   //要获取的类型信息
    PVOID ObjectInformation,                                //缓冲区           用户模式下使用 NtQueryObject
    _In_ ULONG ObjectInformationLength, _Out_opt_           //缓冲区大小
    PULONG ReturnLength);                                   //返回长度

typedef NTSTATUS(WINAPI * PfnZwQuerySystemInformation)(
    IN ULONG SysInfoClass,
    IN OUT PVOID SystemInfroMation,
    IN ULONG SystemInforMationLength,
    OUT PULONG RetLen);


typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    BYTE Reserved1[48];
    UNICODE_STRING ImageName;
    KPRIORITY BasePriority;
    HANDLE UniqueProcessId;
    PVOID Reserved2;
    ULONG HandleCount;
    ULONG SessionId;
    PVOID Reserved3;
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG Reserved4;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    PVOID Reserved5;
    SIZE_T QuotaPagedPoolUsage;
    PVOID Reserved6;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER Reserved7[6];
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

