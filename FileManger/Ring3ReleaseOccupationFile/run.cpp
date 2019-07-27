#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "CReleaseFile.h"


int main()
{
    CReleaseFile file;
    PSYSTEM_PROCESSES psp = file.GetProcessInfo();
    file.CloseHandleForFile(TEXT(""));
   // file.GetHandleCount();
    return 0;
}