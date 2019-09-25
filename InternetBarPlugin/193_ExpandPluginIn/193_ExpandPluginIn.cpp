// 193_ExpandPluginIn.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "../../ClassManger/CNativeApi/CNativeApiManger.h"
#include "../../ProcessManger/ProcessIterator/CProcessOpt.h"


#ifdef _WIN64                   //64位下加载Lib

#ifdef _DEBUG
#pragma comment(lib,"../../PulicLib/x64/Debug/CNativeApi.lib")
#pragma comment(lib,"../../PulicLib/x64/Debug/ProcessIterator.lib")
#else
#pragma comment(lib,"../../PulicLib/x64/Release/CNativeApi.lib")
#pragma comment(lib,"../../PulicLib/x64/Release/ProcessIterator.lib")
#endif

#else

#ifdef _DEBUG

#pragma comment(lib,"../../PulicLib/Win32/Debug/CNativeApi.lib")
#pragma comment(lib,"../../PulicLib/Win32/Debug/ProcessIterator.lib")

#else

#pragma comment(lib,"../../PulicLib/Win32/Release/CNativeApi.lib")
#pragma comment(lib,"../../PulicLib/Win32/Release/ProcessIterator.lib")

#endif

#endif


#include "193_ExpandPluginIn.h"

/*
针对193问题的扩展处理.

1.变种. explore进程做代理

*/


BOOL ExplorerPlugin()
{

    /*
    针对ExplorerPlugin做的处理
    1.首先代理改为不断创建Explorer
    2.病毒Explorer会创建很多线程.可以以此为思路去做
    3.病毒Explorer会乱认爹. 所以可以以此为特征.
    解决方法:
        自我的解决方法为:
        1.遍历所有Explorer进程
        2.判断是否有父进程
        3.判断Explorer进程的线程相同入口是不是很多

        如果以上都成立.结束这个进程.
    */

    CProcessOpt opt;
    CNativeApiManger nat;

    PfnZwQuerySystemInformation ZwQuerySystemInformation =
        reinterpret_cast<PfnZwQuerySystemInformation> (nat.NatGetZwQuerySystemInformation());  //获取ZwQuerySystemInforMation地址.
    if (nullptr == ZwQuerySystemInformation)
        return FALSE;

    //获取进程
    //ZwQuerySystemInformation(ProcessThreadStackAllocation,)


    return 0;
}
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    ExplorerPlugin();
}

