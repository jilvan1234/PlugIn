#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "Hook.h"

typedef BOOL (*_cdecl PfnHook)();

int _tmain(int argc, _TCHAR *argv)
{
    TCHAR szPath[1024] = { NULL };
    GetCurrentDirectory(1024 * sizeof(TCHAR), szPath);
    ::lstrcat(szPath, TEXT("\\Hook.dll"));
    HMODULE hModule = ::LoadLibrary(szPath);
    if (NULL == hModule)
        return FALSE;
    PfnHook SetHook = reinterpret_cast<PfnHook>(GetProcAddress(hModule,TEXT("Hook")));
    if (NULL == SetHook)
        return FALSE;

    SetHook();

    system("pause");
    return 0;
}