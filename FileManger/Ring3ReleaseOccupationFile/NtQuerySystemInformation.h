#pragma once
#include <windows.h>


typedef LONG NTSTATUS;

#define STATUS_SUCCESS                  ((NTSTATUS)0x00000000L)   
#define STATUS_UNSUCCESSFUL             ((NTSTATUS)0xC0000001L)   
#define STATUS_NOT_IMPLEMENTED          ((NTSTATUS)0xC0000002L)   
#define STATUS_INVALID_INFO_CLASS       ((NTSTATUS)0xC0000003L)   
#define STATUS_INFO_LENGTH_MISMATCH     ((NTSTATUS)0xC0000004L)   

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
    SystemNotImplemented2,                  // 10 Y N   A
    SystemModuleInformation,                // 11 Y N   B
    SystemLockInformation,                  // 12 Y N   C
    SystemNotImplemented3,                  // 13 Y N   D
    SystemNotImplemented4,                  // 14 Y N   E
    SystemNotImplemented5,                  // 15 Y N   F
    SystemHandleInformation,                // 16 Y N   10
    SystemObjectInformation,                // 17 Y N   11
    SystemPagefileInformation,              // 18 Y N   12
    SystemInstructionEmulationCounts,       // 19 Y N   13
    SystemInvalidInfoClass1,                // 20       14
    SystemCacheInformation,                 // 21 Y Y   15
    SystemPoolTagInformation,               // 22 Y N   16
    SystemProcessorStatistics,              // 23 Y N   17
    SystemDpcInformation,                   // 24 Y Y   18
    SystemNotImplemented6,                  // 25 Y N   19
    SystemLoadImage,                        // 26 N Y   1A
    SystemUnloadImage,                      // 27 N Y   1B
    SystemTimeAdjustment,                   // 28 Y Y   1C
    SystemNotImplemented7,                  // 29 Y N   1D
    SystemNotImplemented8,                  // 30 Y N   1E
    SystemNotImplemented9,                  // 31 Y N   1F
    SystemCrashDumpInformation,             // 32 Y N   20
    SystemExceptionInformation,             // 33 Y N   21
    SystemCrashDumpStateInformation,        // 34 Y Y/N 22
    SystemKernelDebuggerInformation,        // 35 Y N   23
    SystemContextSwitchInformation,         // 36 Y N   24
    SystemRegistryQuotaInformation,         // 37 Y Y   25
    SystemLoadAndCallImage,                 // 38 N Y   26
    SystemPrioritySeparation,               // 39 N Y   27
    SystemNotImplemented10,                 // 40 Y N   28
    SystemNotImplemented11,                 // 41 Y N   29
    SystemInvalidInfoClass2,                // 42       2A
    SystemInvalidInfoClass3,                // 43       2B
    SystemTimeZoneInformation,              // 44 Y N   2C
    SystemLookasideInformation,             // 45 Y N   2D
    SystemSetTimeSlipEvent,                 // 46 N Y   2E
    SystemCreateSession,                    // 47 N Y   2F
    SystemDeleteSession,                    // 48 N Y   30
    SystemInvalidInfoClass4,                // 49       31
    SystemRangeStartInformation,            // 50 Y N   32
    SystemVerifierInformation,              // 51 Y Y   33
    SystemAddVerifier,                      // 52 N Y   34
    SystemSessionProcessesInformation       // 53 Y N   35

} SYSTEM_INFORMATION_CLASS;

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

//句柄相关.
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

typedef struct _SYSTEM_HANDLE_INFORMATION
{
    ULONG NumberOfHandles;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;
