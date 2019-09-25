// CreateSupendProcessInject.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

/*
挂起进程注入 测试程序
*/
#include "pch.h"
#include <iostream>



#include "../../ThreadManger/RemoteThread/CRemoteThread.h"
#include "../../ThreadManger/RemoteThread/CRemoteThread.cpp"




int main()
{
    CRemoteThread cRts;
    char szShellCode[] = { 0xC3 };
    cRts.RtsCreateSusPendProcessInject(TEXT("calc.exe"), NULL, szShellCode, 1);
}


