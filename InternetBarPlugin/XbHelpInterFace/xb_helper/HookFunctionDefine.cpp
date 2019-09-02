#include "HookFunctionDefine.h"
#include "CFilterByDll.h"




#ifdef MIN_HOOK

PFNloadLibraryExw pfnLoadLibraryExWTarget = NULL;   //过滤LoadLibraryExW
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
        return NULL;  //在Map表中找到了拦截的DLL
    }

    //上面是根据名字查找.下面是根据特征查找.
    if (TRUE == PassDll.isFilter(lpLibFileName))
    {
        return NULL; //提供接口函数.IsFileter. 如果特征相匹配.就过滤.否则则不过滤.
    }
    return pfnLoadLibraryExWTarget(lpLibFileName, hFile, dwFlags);
}




