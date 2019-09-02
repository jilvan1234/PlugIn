#include "HookFunctionDefine.h"
#include "CFilterByDll.h"




#ifdef MIN_HOOK

PFNloadLibraryExw pfnLoadLibraryExWTarget = NULL;   //����LoadLibraryExW
#endif
#ifdef DEOURS_HOOK



PFNloadLibraryExw pfnLoadLibraryExWTarget =
reinterpret_cast<PFNloadLibraryExw>(DetourFindFunction("KernelBase.dll", "LoadLibraryExW"));
#endif


HMODULE WINAPI MyLoadLibraryExWCallBack(
    LPCWSTR lpLibFileName,
    HANDLE hFile,
    DWORD  dwFlags
)
{

    CFilterByDll PassDll;

    map<CBinString, CBinString> PassDllArray = PassDll.GetPassDllArray();

    CBinString strPath = lpLibFileName;
    CBinString Path = strPath.substr(strPath.find_last_of(TEXT("\\")) + 1);

    ::transform(Path.begin(), Path.end(), Path.begin(), ::toupper);


    map<CBinString, CBinString>::iterator it;
    it = PassDllArray.find(Path);
    if (it != PassDllArray.end())
    {
        return NULL;  //��Map�����ҵ������ص�DLL
    }

    //�����Ǹ������ֲ���.�����Ǹ�����������.
    if (TRUE == PassDll.isFilter(lpLibFileName))
    {
        return NULL; //�ṩ�ӿں���.IsFileter. ���������ƥ��.�͹���.�����򲻹���.
    }
    return pfnLoadLibraryExWTarget(lpLibFileName, hFile, dwFlags);
}




