
#include "../../ClassManger/CNativeApi/CNativeApiManger.h"
#include "../../ClassManger/CNativeApi/CNativeApiManger.cpp"

#include "CProcessOpt.h"
#include "CRemoteThread.h"
#include "CShellCode.h"
#include <windows.h>


int main()
{
    CRemoteThread cRts;
    char szShellCode[] = { 0 };
    szShellCode[0] = 0xC3;
    cRts.RtsCreateSusPendProcessInject(
        TEXT("F:\\公司文档\\公司代码\\PlugIn\\TestCode\\Ring3TestCode\\x64\\Release\\calc.exe"),
        NULL, 
        szShellCode, 
        1);
}
