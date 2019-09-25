


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 头文件
#include <Windows.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "../../../publicstruct.h"

//HOOK自身的 GetFileVersionInfoSizeW 其第一个参数为路径.会加载自己的程序DLL.根据DLL名退出其
#ifdef _WIN64
//../../../../HOOK/X64Detours.lib\detours.lib;
#pragma comment(lib,"../../../HOOK/MinHook/MinHookLib/libMinHook.x64.lib")
#pragma comment(lib,"../../../HOOK/X64Detours.lib/detours.lib")
#else
#pragma comment(lib,"../../../HOOK/MinHook/MinHookLib/libMinHook.x86.lib")
#pragma comment(lib,"../../../HOOK/X86Detours.lib/detours.lib")
#endif // _WIN64

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
#pragma comment(linker, "/EXPORT:GetFileVersionInfoA=_AheadLib_GetFileVersionInfoA,@1")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoByHandle=_AheadLib_GetFileVersionInfoByHandle,@2")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoExW=_AheadLib_GetFileVersionInfoExW,@3")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeA=_AheadLib_GetFileVersionInfoSizeA,@4")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeExW=_AheadLib_GetFileVersionInfoSizeExW,@5")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoSizeW=_AheadLib_GetFileVersionInfoSizeW,@6")
#pragma comment(linker, "/EXPORT:GetFileVersionInfoW=_AheadLib_GetFileVersionInfoW,@7")
#pragma comment(linker, "/EXPORT:VerFindFileA=_AheadLib_VerFindFileA,@8")
#pragma comment(linker, "/EXPORT:VerFindFileW=_AheadLib_VerFindFileW,@9")
#pragma comment(linker, "/EXPORT:VerInstallFileA=_AheadLib_VerInstallFileA,@10")
#pragma comment(linker, "/EXPORT:VerInstallFileW=_AheadLib_VerInstallFileW,@11")
#pragma comment(linker, "/EXPORT:VerLanguageNameA=_AheadLib_VerLanguageNameA,@12")
#pragma comment(linker, "/EXPORT:VerLanguageNameW=_AheadLib_VerLanguageNameW,@13")
#pragma comment(linker, "/EXPORT:VerQueryValueA=_AheadLib_VerQueryValueA,@14")
#pragma comment(linker, "/EXPORT:VerQueryValueW=_AheadLib_VerQueryValueW,@15")
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 原函数地址指针
PVOID pfnGetFileVersionInfoA;
PVOID pfnGetFileVersionInfoByHandle;
PVOID pfnGetFileVersionInfoExW;
PVOID pfnGetFileVersionInfoSizeA;
PVOID pfnGetFileVersionInfoSizeExW;
PVOID pfnGetFileVersionInfoSizeW;
PVOID pfnGetFileVersionInfoW;
PVOID pfnVerFindFileA;
PVOID pfnVerFindFileW;
PVOID pfnVerInstallFileA;
PVOID pfnVerInstallFileW;
PVOID pfnVerLanguageNameA;
PVOID pfnVerLanguageNameW;
PVOID pfnVerQueryValueA;
PVOID pfnVerQueryValueW;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 宏定义
#define EXTERNC extern "C"
#define NAKED __declspec(naked)
#define EXPORT __declspec(dllexport)

#define ALCPP EXPORT NAKED
#define ALSTD EXTERNC EXPORT NAKED void __stdcall
#define ALCFAST EXTERNC EXPORT NAKED void __fastcall
#define ALCDECL EXTERNC NAKED void __cdecl
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace DetourseHook
{


    LONG DetourseHookStart();
    LONG DetourseHookEnd();

    typedef DWORD (APIENTRY *TypePfnGetFileVersionInfoSizeW)(
            __in        LPCWSTR lptstrFilename, /* Filename of version stamped file */
            __out_opt LPDWORD lpdwHandle       /* Information for use by GetFileVersionInfo */
        );
    TypePfnGetFileVersionInfoSizeW PfnGetFileVersionInfoSizeWTarger = NULL;



     DWORD APIENTRY MyGetFileVersionInfoSizeW(
        __in        LPCWSTR lptstrFilename, /* Filename of version stamped file */
        __out_opt LPDWORD lpdwHandle       /* Information for use by GetFileVersionInfo */
        )
    {
         CBinString str = lptstrFilename;
         if (str.find(TEXT("Packet.dll")) != str.npos)
         {
             //找到了。
             OutputDebugStringA("找到了");
             ExitProcess(0);
         }
         OutputDebugStringA("没找到");
         return PfnGetFileVersionInfoSizeWTarger(lptstrFilename,lpdwHandle);
    }


    LONG DetourseHookStart()
    {

    
            DetourRestoreAfterWith();
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourAttach(&(LPVOID&)PfnGetFileVersionInfoSizeWTarger, MyGetFileVersionInfoSizeW);

            return DetourTransactionCommit();
    }


    LONG DetourseHookEnd()
    {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach((void **)&PfnGetFileVersionInfoSizeWTarger, MyGetFileVersionInfoSizeW);//撤销拦截函数

        return DetourTransactionCommit();//

    }

      
}

 


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Hook 命名空间
namespace Hook
{
    HHOOK m_hHook;
    // HOOK 句柄


// HOOK 函数
    LRESULT CALLBACK HookProc(INT iCode, WPARAM wParam, LPARAM lParam)
    {
        if (iCode > 0)
        {
            ;
        }

        return CallNextHookEx(m_hHook, iCode, wParam, lParam);
    }

    // Hook
    inline BOOL WINAPI Hook(INT iHookId = WH_CALLWNDPROC)
    {
        m_hHook = SetWindowsHookEx(iHookId, HookProc, NULL, GetCurrentThreadId());
        return (m_hHook != NULL);
    }

    // Unhook
    inline VOID WINAPI Unhook()
    {
        if (m_hHook)
        {
            UnhookWindowsHookEx(m_hHook);
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AheadLib 命名空间
namespace AheadLib
{
    HMODULE m_hModule = NULL;	// 原始模块句柄
    DWORD m_dwReturn[15] = { 0 };	// 原始函数返回地址


    // 获取原始函数地址
    FARPROC WINAPI GetAddress(PCSTR pszProcName)
    {
        FARPROC fpAddress;
        CHAR szProcName[16];
        TCHAR tzTemp[MAX_PATH];

        fpAddress = GetProcAddress(m_hModule, pszProcName);
        if (fpAddress == NULL)
        {
            if (HIWORD(pszProcName) == 0)
            {
                wsprintfA(szProcName, "%d", pszProcName);
                pszProcName = szProcName;
            }

            wsprintf(tzTemp, TEXT("无法找到函数 %hs，程序无法正常运行。"), pszProcName);
            MessageBox(NULL, tzTemp, TEXT("AheadLib"), MB_ICONSTOP);
            ExitProcess(-2);
        }

        return fpAddress;
    }

    // 初始化原始函数地址指针
    inline VOID WINAPI InitializeAddresses()
    {
        pfnGetFileVersionInfoA = GetAddress("GetFileVersionInfoA");
        pfnGetFileVersionInfoByHandle = GetAddress("GetFileVersionInfoByHandle");
        pfnGetFileVersionInfoExW = GetAddress("GetFileVersionInfoExW");
        pfnGetFileVersionInfoSizeA = GetAddress("GetFileVersionInfoSizeA");
        pfnGetFileVersionInfoSizeExW = GetAddress("GetFileVersionInfoSizeExW");
        pfnGetFileVersionInfoSizeW = GetAddress("GetFileVersionInfoSizeW");
        pfnGetFileVersionInfoW = GetAddress("GetFileVersionInfoW");
        pfnVerFindFileA = GetAddress("VerFindFileA");
        pfnVerFindFileW = GetAddress("VerFindFileW");
        pfnVerInstallFileA = GetAddress("VerInstallFileA");
        pfnVerInstallFileW = GetAddress("VerInstallFileW");
        pfnVerLanguageNameA = GetAddress("VerLanguageNameA");
        pfnVerLanguageNameW = GetAddress("VerLanguageNameW");
        pfnVerQueryValueA = GetAddress("VerQueryValueA");
        pfnVerQueryValueW = GetAddress("VerQueryValueW");
    }

    // 加载原始模块
    inline BOOL WINAPI Load()
    {
        TCHAR tzPath[MAX_PATH];
        TCHAR tzTemp[MAX_PATH * 2];

        GetSystemDirectory(tzPath, MAX_PATH);
        lstrcat(tzPath, TEXT("\\rtuti1s.dll"));  //变成1 不是L 让其无法正常的运行.
        m_hModule = LoadLibrary(tzPath);
        if (m_hModule == NULL)
        {
            wsprintf(tzTemp, TEXT("无法加载 %s，程序无法正常运行。"), tzPath);
            MessageBox(NULL, tzTemp, TEXT("AheadLib"), MB_ICONSTOP);
        }
        else
        {
            InitializeAddresses();
        }

        return (m_hModule != NULL);
    }

    // 释放原始模块
    inline VOID WINAPI Free()
    {
        if (m_hModule)
        {
            FreeLibrary(m_hModule);
        }
    }
}
using namespace AheadLib;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 入口函数
BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, PVOID pvReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        
        return  Load(); 
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
       
        Free();
      
    }

    return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void FilterW()
{
    wchar_t *pSzNam = NULL;
    __asm
    {
        mov ecx, [ebp + 0xc];
        mov pSzNam, ecx
    }
   
    CBinString strPath = pSzNam;
    strPath =  strPath.substr(strPath.find_last_of(TEXT("\\"))+1);
    CBinString FiltterName1 = TEXT("WTPcap.dll");
    CBinString FiltterName2 = TEXT("NPF");
    ::transform(FiltterName1.begin(), FiltterName1.end(), FiltterName1.begin(), ::toupper);
    ::transform(strPath.begin(), strPath.end(), strPath.begin(), ::toupper);
    ::transform(FiltterName2.begin(), FiltterName2.end(), FiltterName2.begin(), ::toupper);

    OutputDebugStringW(pSzNam);
    OutputDebugStringW(strPath.c_str());
    if (strPath == FiltterName1)
    {
        ExitProcess(0);
    }

   

    return;
    
}

void  FilterA(void)
{
    
    char *pSzNam = NULL;
    __asm
    {
        mov ecx, [ebp + 0xc];
        mov pSzNam,ecx
    }
    OutputDebugStringA(pSzNam);
    
    return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_GetFileVersionInfoA(void)
{
    __asm JMP pfnGetFileVersionInfoA;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_GetFileVersionInfoByHandle(void)
{
    __asm JMP pfnGetFileVersionInfoByHandle;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_GetFileVersionInfoExW(void)
{
    __asm JMP pfnGetFileVersionInfoExW;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_GetFileVersionInfoSizeA(void)
{

 /*   __asm
    {
        call FilterA
    }*/

    __asm JMP pfnGetFileVersionInfoSizeA;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_GetFileVersionInfoSizeExW(void)
{

  
    __asm JMP pfnGetFileVersionInfoSizeExW;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




ALCDECL AheadLib_GetFileVersionInfoSizeW(void)
{
    
    __asm
    {
        call FilterW
    }
    __asm JMP pfnGetFileVersionInfoSizeW;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_GetFileVersionInfoW(void)
{
    __asm JMP pfnGetFileVersionInfoW;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_VerFindFileA(void)
{

    __asm JMP pfnVerFindFileA;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_VerFindFileW(void)
{
    __asm JMP pfnVerFindFileW;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_VerInstallFileA(void)
{
    __asm JMP pfnVerInstallFileA;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_VerInstallFileW(void)
{
    __asm JMP pfnVerInstallFileW;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_VerLanguageNameA(void)
{
    __asm JMP pfnVerLanguageNameA;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_VerLanguageNameW(void)
{
    __asm JMP pfnVerLanguageNameW;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_VerQueryValueA(void)
{
    __asm JMP pfnVerQueryValueA;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
ALCDECL AheadLib_VerQueryValueW(void)
{
    __asm JMP pfnVerQueryValueW;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

