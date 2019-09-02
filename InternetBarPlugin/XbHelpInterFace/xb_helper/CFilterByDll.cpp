#include "stdafx.h"
#include "CFilterByDll.h"

CFilterByDll::CFilterByDll()
{
    InitFilterDllArray(); //���ص�DLL��
    InitFilterSectionName();//�����������صĶ�����.
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

    bRet = isVmpSectionName(DllName);       //�����ƹ���.

    return bRet;
}

BOOL CFilterByDll::isVmpSectionName(CBinString DllName)
{
    BOOL bRet = FALSE;

    /*
    1.��ȡ�ļ�
    2.��������
    3.�����ڱ�
    4.�жϽ������ǲ���.vmp0 || .vmp1
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

    //��ȡ�ļ�.
    szBuffer = new char[sizeof(IMAGE_DOS_HEADER)]();
    bRet = ReadFile(hFile, szBuffer, sizeof(IMAGE_DOS_HEADER), &dwReadBytes, nullptr);
    if (!bRet)
    {
        CloseHandle(hFile);
        return FALSE;
    }
    //�ɹ���ȡIMAGE_DOS_HEAD ͷ��С. //�ƶ��ļ�ָ�뵽 doshead->elfnewλ��.��ȡNTλ�ô�С.
    pDosHead = reinterpret_cast<PIMAGE_DOS_HEADER>(szBuffer);
    if (pDosHead->e_lfanew == 0)
    {
        delete[] szBuffer;
        CloseHandle(hFile);
        return FALSE;
    }

    //print Dos->e_lfanew;
    SetFilePointer(hFile, pDosHead->e_lfanew, 0, FILE_BEGIN); //�ƶ���PEͷ.
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
  
    ////��ȡ���θ���.��������.
    dwSectionCount = pFileHeader->NumberOfSections;


    //����������
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

            
            return TRUE;  //��Map�����ҵ������ص�DLL
        }

      /*  if (CurrtenString.find(ComParString1) != CurrtenString.npos || CurrtenString.find(ComParString2) != CurrtenString.npos)
        {*/
            //�ҵ���.����TRUE.���������ӡ.
           
         
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

//�����ƵĹ��˵�MAP��

    

    return TRUE;
}

BOOL CFilterByDll::InitFilterSectionName()
{
    m_FilterSectionName.insert(pair<string, string>(".vmp0", ".vmp0"));
    m_FilterSectionName.insert(pair<string, string>(".vmp1", ".vmp1"));
    return TRUE;
}


