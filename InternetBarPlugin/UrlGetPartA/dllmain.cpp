// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#include "../../publicstruct.h"
#include "../../HOOK/MinHook/MinHookInclude/MinHook.h"
#include "../../HOOK/DetoursInclude/detours.h"
#include <winhttp.h>
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

#ifdef _WIN64
//../../../../HOOK/X64Detours.lib\detours.lib;

#pragma comment(lib,"../../HOOK/MinHook/MinHookLib/libMinHook.x64.lib")

#else
#pragma comment(lib,"../../HOOK/MinHook/MinHookLib/libMinHook.x86.lib")
#pragma comment(lib,"../../HOOK/X86Detours.lib/detours.lib")
#endif // _WIN64

/*
作用:
HOOK UrlGetPartA 查看查询信息.获得URL
*/

using PfnUrlGetPartA = DWORD(WINAPI *)(
    PCSTR pszIn,
    PSTR  pszOut,
    DWORD *pcchOut,
    DWORD dwPart,
    DWORD dwFlags
    );
using PfnGetUrl = DWORD(WINAPI *)(LPVOID lpParam);

DWORD GetHookAddress()
{
    //获取sesafe.dll + 0xFF9B405D的地址
    HMODULE hModule = GetModuleHandle(TEXT("sesafe.dll"));
    if (NULL == hModule)
        return 0;
    DWORD dwAddress = (DWORD)hModule + 0x4405D;
    return dwAddress;
}
PfnGetUrl  pMyTargetGetUrl = reinterpret_cast<PfnGetUrl>(GetHookAddress());  //Hook TargetUrl的值.

PfnUrlGetPartA MyTargetUrlGetPartA =
reinterpret_cast<PfnUrlGetPartA>(
DetourFindFunction("KernelBase.dll","UrlGetPartA"));

DWORD __stdcall MyUrlGetPartA(
    __in LPCSTR pszIn, 
    __out_ecount(*pcchOut) LPSTR pszOut, 
    __inout LPDWORD pcchOut, 
    __in DWORD dwPart, 
    __in DWORD dwFlags)
{

    //打印返回地址.
    DWORD dwAddress = 0;
    __asm
    {
        mov eax, [ebp + 4];
        mov dwAddress,eax
    }
    char szBuffer[0x100] = { 0 };
    sprintf_s(szBuffer,0x100, "返回地址 = %0x", dwAddress);
    OutputDebugStringA(szBuffer);
    OutputDebugStringA("函数来了");
    OutputDebugStringA(pszIn);//查询信息
    return MyTargetUrlGetPartA(pszIn, pszOut, pcchOut, dwPart, dwFlags); //调用原本的
}

typedef struct _Url
{
    DWORD Unkown1; 
    DWORD Unkown2; 
    DWORD Unkown3; 
    DWORD Unkown4; 

    DWORD Unkown5; 
    DWORD Unkown6; 
    DWORD Unkown7;   //取内容 + 0x2C = 字符串首地址
    DWORD pUrlAddress;

    DWORD Unkown9; 
    DWORD Unkown10; 
    DWORD Unkown11;
    DWORD Unkown12;

    DWORD Unkown13;
    DWORD Unkown14;
    DWORD Unkown15;
    DWORD Unkown16;

    DWORD Unkown17;
    PVOID PurAscii; //指向URL 存储URL的地址
    
}URL,*PURL;

LPVOID lpTargGetUrl = NULL;

DWORD WINAPI MyUrl(LPVOID lpParam)
{
    //HOOK线程回调函数
    if (lpParam == 0)
        return 0;
    OutputDebugStringA("拦截URL测试");
    PURL pUrl = reinterpret_cast<PURL>(lpParam);
    //输出PURADDRESS的地址中的内容. 内容则是URL首地址.
    char szUrlBuffer[0x200] = { 0 };
    if ((char *)pUrl->PurAscii == 0)
        return 0;
   // DWORD Address1 = *(DWORD *)((DWORD)&pUrl->pUrlAddress + 0X1c) + 0x2c;
    //char SearchUrl[] = "www.hao123.com";
    //memcpy((char*)pUrl->PurAscii, SearchUrl, sizeof(SearchUrl) / sizeof(SearchUrl[0]));
    sprintf_s(szUrlBuffer, 0x200, "%s \r\n", (char *)pUrl->PurAscii);
    OutputDebugStringA(szUrlBuffer);
    return 0;
}
DWORD  __declspec(naked)  MyGetUrl(LPVOID lpParam)
{

   
    __asm
    {
        pushad
        pushfd
        push lpParam
        call MyUrl
        popfd
        popad
        jmp lpTargGetUrl;
        
    }
}
BOOL  BeginHook()
{
    if (MH_Initialize() != MH_OK)
    {
        OutputDebugString(TEXT("HOOK 失败,初始化失败\r\n"));
        return FALSE;
    }

    // Create a hook for MessageBoxW, in disabled state.

    if (MH_CreateHook(&UrlGetPartA, &MyGetUrl,        //创建HOOK

        reinterpret_cast<void**>(&pMyTargetGetUrl)) != MH_OK)
    {

        OutputDebugString(TEXT("创建HOOK失败,初始化失败\r\n"));
        return FALSE;
    }

    // Enable the hook for MessageBoxW.

    if (MH_EnableHook(&UrlGetPartA) != MH_OK)            //启动HOOK
    {


        return FALSE;
    }
    

    return TRUE;
}
void EndHood()
{

}

void DetoursBeginHook()
{
  
       /* DetourRestoreAfterWith();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(LPVOID&)pMyTargetGetUrl, MyGetUrl);
        DetourTransactionCommit();*/

    //自定义HOOK编写

    /*
    1.获取HOOK的地址
    2.修改内存保护属性
    3.申请TarGet内存空间
    4.保存HOOK的原指令的值写入到申请的内存空间.
    5.写入HOOK代码.跳转到你现在的函数位置.
    6.你现在的函数跳转到Target函数位置.
    8.Target函数位置.跳转到原指令位置下方.
    */

    if (pMyTargetGetUrl == 0)
        return;
    DWORD dwOldPro;
    if (!VirtualProtect(pMyTargetGetUrl, 0x1000, PAGE_EXECUTE_READWRITE, &dwOldPro))
    {
        return;
    }

    //保存指令
    char OldOptCode[7] = { 0 };
    memcpy(OldOptCode, pMyTargetGetUrl, 7);
   

    //前7条指令写为 E9 MyGetUrl  90 90  JMP MyGetUrl nop nop
    char Jmp[7] = { 0xE9,0X0,0X0,0X0,0X0,0X90,0X90 };
    
    DWORD JmpGetUrlOptCode = (DWORD) (MyGetUrl) - (DWORD)(pMyTargetGetUrl) - 5;
    *(DWORD *)((char *)Jmp + 1) = JmpGetUrlOptCode;
    memcpy(pMyTargetGetUrl, Jmp, 7);



    //申请Targ写入远程空间.
    lpTargGetUrl = VirtualAlloc(0, 0X1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (lpTargGetUrl == NULL)
        return;
    //old指令写入
    memcpy(lpTargGetUrl, OldOptCode, 7);
    //tar指令后面写入E9 + 4个偏移.

    char JmpOffsetBuffer[5] = { 0xE9,0X0,0X0,0X0,0X0 };
    *(DWORD *)(((char *)JmpOffsetBuffer + 1)) =
        (DWORD)(((char *)pMyTargetGetUrl + 7)) - (DWORD)((char *)lpTargGetUrl+7) - 5;
    memcpy((char *)lpTargGetUrl + 7, JmpOffsetBuffer, 5); //Target写入跳转指令

    
    if (!VirtualProtect(pMyTargetGetUrl, 0x1000, dwOldPro, &dwOldPro))
    {
        return;
    }
}

void DetoursUnHook()
{
    //DetourTransactionBegin();
    //DetourUpdateThread(GetCurrentThread());
    //DetourDetach((void **)&pMyTargetGetUrl, MyGetUrl);//撤销拦截函数
    //DetourTransactionCommit();//

    //自定义HOOK编写.


}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
       
        DetoursBeginHook();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        DetoursUnHook();
        break;
    }
    return TRUE;
    
}

