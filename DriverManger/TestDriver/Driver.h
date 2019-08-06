#pragma once

#include <ntddk.h>
#include <ntstrsafe.h>

#define SYMBOLIC_LINK_NAME L"\\DosDevices\\InterceptLink"
#define DEVICE_NAME        L"\\device\\InterceptDevice"

DRIVER_UNLOAD DriverUnLoad;
NTSTATUS InitDeviceAnSybolicLinkName(PDRIVER_OBJECT pDriverObj);
NTSTATUS InitDisPatchFunction(PDRIVER_OBJECT pDriverObj);

NTSTATUS DisPatchControl(PDEVICE_OBJECT pDriverObj, PIRP pIrp);
NTSTATUS DisPatchCommand(PDEVICE_OBJECT pDriverObj, PIRP pIrp);
NTSTATUS DisPatchRead(PDEVICE_OBJECT pDriverObj, PIRP pIrp);
NTSTATUS DisPatchWrite(PDEVICE_OBJECT pDriverObj, PIRP pIrp);
NTSTATUS DisPatchClose(PDEVICE_OBJECT pDriverObj, PIRP pIrp);
NTSTATUS DisPatchCreate(PDEVICE_OBJECT pDriverObj, PIRP pIrp);

void PassKill(HANDLE hPid);
void ZwKillProcess(PVOID pUserBuffer);