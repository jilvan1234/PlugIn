#pragma once
#include "ObjectProtected.h"
#include "Driver.h"


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING pRegPath)
{
    pDriverObj->DriverUnload = DriverUnLoad;

    /*ULONG iCount = 0;
    NTSTATUS ntStatus;
    ntStatus = InitDeviceAnSybolicLinkName(pDriverObj);
    if (!NT_SUCCESS(ntStatus))
    {
        return ntStatus;
    }

    ntStatus = InitDisPatchFunction(pDriverObj);
    if (!NT_SUCCESS(ntStatus))
    {
        return ntStatus;
    }*/
   
    Pass(pDriverObj);
 
   // AllocateOptMem();
   // UnAllocateOptMem();
    //SetProTerminate(TRUE);
   // SetProImageName("calc");
    InstallObjectProtecteHOOk();
	return STATUS_SUCCESS;
}


