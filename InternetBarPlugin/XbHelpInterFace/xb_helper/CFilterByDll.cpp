#include "stdafx.h"
#include "CFilterByDll.h"

CFilterByDll::CFilterByDll()
{
    InitFilterDllArray(); //拦截的DLL名
    InitFilterSectionName();//根据特征拦截的段名称.
}


CFilterByDll::~CFilterByDll()
{
}




map<CBinString, CBinString>  CFilterByDll::GetPassDllArray() const
{
    return this->m_PassDllArray;
}

map<string, string> CFilterByDll::GetFilterSectionNameArray() const
{
    return this->m_FilterSectionName;
}
BOOL CFilterByDll::AddPassDllName(CBinString DllName)
{
    m_PassDllArray.insert(pair<CBinString, CBinString>(DllName, DllName));
    return TRUE;
}

BOOL CFilterByDll::isFilter(CBinString DllName)
{

    BOOL bRet = FALSE;

    bRet = isVmpSectionName(DllName);       //节名称过滤.

    return bRet;
}

BOOL CFilterByDll::isVmpSectionName(CBinString DllName)
{
    BOOL bRet = FALSE;

    /*
    1.读取文件
    2.解析区段
    3.遍历节表
    4.判断节名称是不是.vmp0 || .vmp1
    */
    PIMAGE_DOS_HEADER pDosHead = nullptr;
    PIMAGE_NT_HEADERS pNtHeader = nullptr;
    PIMAGE_FILE_HEADER pFileHeader = nullptr;
    PIMAGE_OPTIONAL_HEADER pOptHeader = nullptr;
    PIMAGE_SECTION_HEADER pSectionHead = nullptr;
    DWORD dwSectionCount = 0;
    char *szBuffer = nullptr;
    string DebugStringA = " ";
    wstring DebugStringW = TEXT(" ");

    DWORD dwReadBytes = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    hFile = CreateFile(DllName.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);
    if (INVALID_HANDLE_VALUE == hFile)
    {
        CloseHandle(hFile);
        return FALSE;
    }

    //读取文件.
    szBuffer = new char[sizeof(IMAGE_DOS_HEADER)]();
    bRet = ReadFile(hFile, szBuffer, sizeof(IMAGE_DOS_HEADER), &dwReadBytes, nullptr);
    if (!bRet)
    {
        CloseHandle(hFile);
        return FALSE;
    }
    //成功读取IMAGE_DOS_HEAD 头大小. //移动文件指针到 doshead->elfnew位置.读取NT位置大小.
    pDosHead = reinterpret_cast<PIMAGE_DOS_HEADER>(szBuffer);
    if (pDosHead->e_lfanew == 0)
    {
        delete[] szBuffer;
        CloseHandle(hFile);
        return FALSE;
    }

    //print Dos->e_lfanew;
    SetFilePointer(hFile, pDosHead->e_lfanew, 0, FILE_BEGIN); //移动到PE头.
    delete[] szBuffer;
  
    szBuffer = new char[0x1000]();
    if (nullptr == szBuffer)
    {
        delete[] szBuffer;
       CloseHandle(hFile);
        return FALSE;
    }
    bRet = ReadFile(hFile, szBuffer, 0x1000, &dwReadBytes, nullptr);
    if (!bRet)
    {
        delete[] szBuffer;
        CloseHandle(hFile);
        return FALSE;
    }
    ////print PE signer
    pNtHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(szBuffer);
    pFileHeader = reinterpret_cast<PIMAGE_FILE_HEADER>(&pNtHeader->FileHeader);
    pOptHeader = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(&pNtHeader->OptionalHeader);

  
    if (nullptr == pNtHeader || nullptr == pFileHeader || nullptr == pOptHeader)
    {
        delete[] szBuffer;
        CloseHandle(hFile);
        return FALSE;
    }
  
    ////获取区段个数.解析区段.
    dwSectionCount = pFileHeader->NumberOfSections;


    //遍历节区域
    pSectionHead = reinterpret_cast<PIMAGE_SECTION_HEADER>(&pNtHeader[1]);
   

  
    string CurrtenString;
    map<string, string> LocalFilterSectionName = GetFilterSectionNameArray();
    map<string, string>::iterator it ;
    for (int i = 0; i < dwSectionCount; i++)
    {
        CurrtenString = (char *)pSectionHead->Name;


        
        it = LocalFilterSectionName.find(CurrtenString);
        if (it != LocalFilterSectionName.end())
        {

            if (INVALID_HANDLE_VALUE != hFile)
                CloseHandle(hFile);
            if (nullptr != szBuffer)
                delete[] szBuffer;

            
            return TRUE;  //在Map表中找到了拦截的DLL
        }

      /*  if (CurrtenString.find(ComParString1) != CurrtenString.npos || CurrtenString.find(ComParString2) != CurrtenString.npos)
        {*/
            //找到了.返回TRUE.并且输出打印.
           
         
       // }

        pSectionHead = reinterpret_cast<PIMAGE_SECTION_HEADER>(((CHAR *)pSectionHead + sizeof(IMAGE_SECTION_HEADER)));
       
    }

    if (INVALID_HANDLE_VALUE != hFile)
        CloseHandle(hFile);
    if (nullptr != szBuffer)
        delete[] szBuffer;

    return FALSE;
}

BOOL CFilterByDll::InitFilterDllArray()
{
    m_PassDllArray.insert(pair<CBinString, CBinString>(TEXT("L3CODECA.ACM"), TEXT("L3CODECA.ACM")));

//节名称的过滤的MAP表

    

    return TRUE;
}

BOOL CFilterByDll::InitFilterSectionName()
{
    m_FilterSectionName.insert(pair<string, string>(".vmp0", ".vmp0"));
    m_FilterSectionName.insert(pair<string, string>(".vmp1", ".vmp1"));
    return TRUE;
}


