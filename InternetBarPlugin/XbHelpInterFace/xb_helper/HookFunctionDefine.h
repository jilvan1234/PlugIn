#pragma once

#include "../../../publicstruct.h"


typedef HMODULE(WINAPI *PFNloadLibraryExw)(LPCWSTR lpLibFileName, HANDLE hFile, DWORD  dwFlags);  //HOOK LoadLibraryExW                        
extern "C" PFNloadLibraryExw pfnLoadLibraryExWTarget;//HOOK 的目的函数
HMODULE WINAPI  MyLoadLibraryExWCallBack(LPCWSTR lpLibFileName, HANDLE hFile, DWORD  dwFlags);    //主功能函数


#ifdef _WIN64
#define  MIN_HOOK

#else

#define DEOURS_HOOK  //控制开关语句

#endif



#ifdef MIN_HOOK

#endif

#ifdef DEOURS_HOOK

#endif
