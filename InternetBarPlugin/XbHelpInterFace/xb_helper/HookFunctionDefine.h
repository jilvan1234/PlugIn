#pragma once

#include "../../../publicstruct.h"


typedef HMODULE(WINAPI *PFNloadLibraryExw)(LPCWSTR lpLibFileName, HANDLE hFile, DWORD  dwFlags);  //HOOK LoadLibraryExW                        
extern "C" PFNloadLibraryExw pfnLoadLibraryExWTarget;//HOOK ��Ŀ�ĺ���
HMODULE WINAPI  MyLoadLibraryExWCallBack(LPCWSTR lpLibFileName, HANDLE hFile, DWORD  dwFlags);    //�����ܺ���


#ifdef _WIN64
#define  MIN_HOOK

#else

#define DEOURS_HOOK  //���ƿ������

#endif



#ifdef MIN_HOOK

#endif

#ifdef DEOURS_HOOK

#endif
