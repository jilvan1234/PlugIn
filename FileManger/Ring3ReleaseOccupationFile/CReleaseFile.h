#pragma once
#include "NtQuerySystemInformation.h"
#include "define.h"
#include <string>
using namespace std;

typedef NTSTATUS(WINAPI *PfnNtQuerySystemInformation)(IN SYSTEM_INFORMATION_CLASS, IN OUT PVOID, IN ULONG, OUT PULONG OPTIONAL);

class CReleaseFile
{
public:
    CReleaseFile();
    ~CReleaseFile();


    PSYSTEM_PROCESSES GetProcessInfo();
    PSYSTEM_HANDLE_INFORMATION  GetHandleBasicInfo();
    BOOL RemoteFileByHandle(STRING fileName, HANDLE hProcess, HANDLE Handle);
    BOOL CloseHandleForFile(STRING fileName);


private:
    PfnNtQuerySystemInformation NtQuerySystemInformation;
    PfnNtQuerySystemInformation GetNtQuerySystemInfomation();
};

