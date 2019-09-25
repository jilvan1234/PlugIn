#include "pch.h"
#include "CPeManger.h"
#include <dbghelp.h>
#pragma comment (lib,"Dbghelp.lib")

CPeManger::CPeManger()
{
}


CPeManger::~CPeManger()
{
}

//��ʼ��Ȩ��������Ϣ Ȩ���������.
BOOL CPeManger::PeDesireInit(DWORD DesireAccess, DWORD & dwMapingDesireAccessType, DWORD & dwViewDesireAccessType, DWORD & dwFileDesireAccessType)
{
    switch (DesireAccess)
    {
    case PEOPT_READ:
        dwMapingDesireAccessType = PAGE_READONLY;
        dwViewDesireAccessType = FILE_MAP_READ;
        dwFileDesireAccessType = GENERIC_READ;
        return TRUE;
    case PEOPT_WRITE:
        dwMapingDesireAccessType = PAGE_READWRITE;
        dwViewDesireAccessType = FILE_MAP_WRITE;
        dwFileDesireAccessType = GENERIC_WRITE;
        return TRUE;

    case PEOPT_READ_WRITE:
        dwMapingDesireAccessType = PAGE_READWRITE;
        dwViewDesireAccessType = FILE_MAP_WRITE | FILE_MAP_READ;
        dwFileDesireAccessType = GENERIC_WRITE | GENERIC_WRITE;
        return TRUE;
    case PEOPT_READ_WRITE_EXCUTE:
        dwMapingDesireAccessType = PAGE_EXECUTE_READWRITE;
        dwViewDesireAccessType = FILE_MAP_ALL_ACCESS;
        dwFileDesireAccessType = GENERIC_ALL;
        return TRUE;
    default:
        return FALSE;
    }

    return FALSE;
}
DWORD CPeManger::PeDesireAccessGetDesiredAccessRead()
{
    return 0;
}
DWORD CPeManger::PeDesireAccessGetDesiredAccessWrite()
{
    return 1;
}
DWORD CPeManger::PeDesireAccessGetDesiredAccessReadWrite()
{
    return 2;
}
DWORD CPeManger::PeDesireAccessGetDesiredAccessReadWriteExcute()
{
    return 3;
}




/*
��ȡPEͷ��Ϣ
*/
PIMAGE_DOS_HEADER CPeManger::PeHederGetDosHeader(CBinString FileName, HANDLE hFile, DWORD DesireAccess)
{

    HANDLE hFileMaping = nullptr;
    LPVOID MapViewBuf = nullptr;
    PIMAGE_DOS_HEADER  pDosHead = nullptr;
    PVOID DosHeadBuffer = new IMAGE_DOS_HEADER;
    DWORD dwReadBytes = 0;
    DWORD dwRet = 0;
    LARGE_INTEGER la = { 0 };
    BOOL bRet = FALSE;
    HANDLE hLocalFile = INVALID_HANDLE_VALUE;

    DWORD dwMapingDesireAccessType = 0;
    DWORD dwViewDesireAccessType = 0;
    DWORD dwFileDesireAccessType = 0;

    bRet = PeDesireInit(DesireAccess, dwMapingDesireAccessType, dwViewDesireAccessType, dwFileDesireAccessType);
    if (!bRet)
        return nullptr;
    // �����ļ����ӳ����ߴ��ļ�.
    if (INVALID_HANDLE_VALUE != hFile)
    {
        //����FileName string ֱ��ͨ�����ȥ��.
       
        //��ȡ�ļ���С.ӳ���ļ���С��ô��.
       
        GetFileSizeEx(hFile, &la);
     
        hFileMaping =  CreateFileMapping(hFile, NULL, dwMapingDesireAccessType, 0, la.LowPart, NULL);
        if (nullptr != hFileMaping)

        {
            //�ɹ���ӳ���ļ�.
            MapViewBuf = MapViewOfFile(hFileMaping, dwViewDesireAccessType, 0, 0, 0); //ӳ��ȫ��.
            if (nullptr != MapViewBuf)
            {
                memcpy(DosHeadBuffer, MapViewBuf, sizeof(IMAGE_DOS_HEADER));
                pDosHead = reinterpret_cast<PIMAGE_DOS_HEADER>(DosHeadBuffer);
                if (nullptr != pDosHead && pDosHead->e_magic == 0x5A4D)
                {
                    UnmapViewOfFile(MapViewBuf);
                    CloseHandle(hFileMaping);  //�ͷ���Դ����DOSͷ.
                    return reinterpret_cast<PIMAGE_DOS_HEADER>(DosHeadBuffer);
                }

            }

        }
//����ӳ��ʧ��.�Զ��ļ��ķ�ʽ��

       
        dwRet = ReadFile(hFile, DosHeadBuffer, sizeof(IMAGE_DOS_HEADER), &dwReadBytes, nullptr);
        if (dwRet != 0)
        {
            //�ɹ�.����з���.
            pDosHead = reinterpret_cast<PIMAGE_DOS_HEADER>(DosHeadBuffer);
            if (nullptr != pDosHead && pDosHead->e_magic == 0x5A4D)
            {
                return reinterpret_cast<PIMAGE_DOS_HEADER>(DosHeadBuffer);
            }
        }

        return nullptr;
    }
    
    //FileName��Ϊ��.�����ӳ���ļ���.
    if (!FileName.empty())
    {
        hLocalFile = CreateFile(FileName.c_str(), dwFileDesireAccessType, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);  //����Ȩ�ޱ��뱣��һ��.
        if (INVALID_HANDLE_VALUE != hLocalFile)
        {
            //�����ļ�ӳ��.
            GetFileSizeEx(hLocalFile, &la);

            hFileMaping = CreateFileMapping(hLocalFile, NULL, dwMapingDesireAccessType, 0, la.LowPart, NULL);
            if (nullptr != hFileMaping)

            {
                //�ɹ���ӳ���ļ�.
                MapViewBuf = MapViewOfFile(hFileMaping,  dwViewDesireAccessType, 0, 0, 0); //ӳ��ȫ��.
                if (nullptr != MapViewBuf)
                {
                    memcpy(DosHeadBuffer, MapViewBuf, sizeof(IMAGE_DOS_HEADER));
                    pDosHead = reinterpret_cast<PIMAGE_DOS_HEADER>(DosHeadBuffer);
                    if (nullptr != pDosHead && pDosHead->e_magic == 0x5A4D)
                    {
                        UnmapViewOfFile(MapViewBuf);
                        CloseHandle(hFileMaping);  //�ͷ���Դ����DOSͷ.
                        CloseHandle(hLocalFile);
                        return reinterpret_cast<PIMAGE_DOS_HEADER>(DosHeadBuffer);
                    }

                }

            }


            dwRet = ReadFile(hLocalFile, DosHeadBuffer, sizeof(IMAGE_DOS_HEADER), &dwReadBytes, nullptr);
            if (dwRet != 0)
            {
                //�ɹ�.����з���.
                pDosHead = reinterpret_cast<PIMAGE_DOS_HEADER>(DosHeadBuffer);
                if (nullptr != pDosHead && pDosHead->e_magic == 0x5A4D)
                {
                    CloseHandle(hLocalFile);
                    return reinterpret_cast<PIMAGE_DOS_HEADER>(DosHeadBuffer);
                }
            }

        }

        //������ж��ļ���.

      

        return nullptr;

    }

    if (INVALID_HANDLE_VALUE != hLocalFile)
    {
        CloseHandle(hLocalFile);
    }
    return nullptr;
}

PIMAGE_NT_HEADERS CPeManger::PeHederGetNtHeader(CBinString FileName, HANDLE hFile, DWORD DesireAccess)
{
    PIMAGE_DOS_HEADER pDosHeader = nullptr;
    PIMAGE_NT_HEADERS  pNtHeder = nullptr;
    PVOID pNtHeaderBuf = new IMAGE_NT_HEADERS;

    HANDLE hFileMaping = nullptr;
    LPVOID MapViewBuf = nullptr;
    DWORD dwReadBytes = 0;
    DWORD dwRet = 0;
    LARGE_INTEGER la = { 0 };
    BOOL bRet = FALSE;
    HANDLE hLocalFile = INVALID_HANDLE_VALUE;


    pDosHeader = PeHederGetDosHeader(FileName, hFile, DesireAccess);
    if (nullptr == pDosHeader)
    {
        return nullptr;
    }

    //hFile ��Ϊ�� ӳ�� ���� ���ļ�����.


    DWORD dwMapingDesireAccessType = 0;
    DWORD dwViewDesireAccessType = 0;
    DWORD dwFileDesireAccessType = 0;

    bRet =  PeDesireInit(DesireAccess, dwMapingDesireAccessType, dwViewDesireAccessType, dwFileDesireAccessType);
    if (!bRet)
        return nullptr;



    if (INVALID_HANDLE_VALUE != hFile)
    {
        //����FileName string ֱ��ͨ�����ȥ��.

        //��ȡ�ļ���С.ӳ���ļ���С��ô��.

        GetFileSizeEx(hFile, &la);

        hFileMaping = CreateFileMapping(hFile, NULL, dwMapingDesireAccessType, 0, la.LowPart, NULL);
        if (nullptr != hFileMaping)

        {
            //�ɹ���ӳ���ļ�.
            MapViewBuf = MapViewOfFile(hFileMaping,  dwViewDesireAccessType, 0, 0, 0); //ӳ��ȫ��.
            if (nullptr != MapViewBuf)
            {
                memcpy(pNtHeaderBuf, (char *)MapViewBuf + pDosHeader->e_lfanew, sizeof(IMAGE_NT_HEADERS));
                pNtHeder = reinterpret_cast<PIMAGE_NT_HEADERS>(pNtHeaderBuf);
                if (nullptr != pNtHeder && pNtHeder->Signature == 0x4550)
                {
                    UnmapViewOfFile(MapViewBuf);
                    CloseHandle(hFileMaping);  //�ͷ���Դ����DOSͷ.
                    return reinterpret_cast<PIMAGE_NT_HEADERS>(pNtHeaderBuf);
                }

            }

        }
        //����ӳ��ʧ��.�Զ��ļ��ķ�ʽ��

        SetFilePointer(hFile, pDosHeader->e_lfanew, 0, FILE_BEGIN);  //�ƶ���NTͷλ�ö�ȡ.
        dwRet = ReadFile(hFile, pNtHeaderBuf, sizeof(IMAGE_NT_HEADERS), &dwReadBytes, nullptr);
        if (dwRet != 0)
        {
            //�ɹ�.����з���.
            pNtHeder = reinterpret_cast<PIMAGE_NT_HEADERS>(pNtHeaderBuf);
            if (nullptr != pNtHeder && pNtHeder->Signature == 0X4550)
            {
                return reinterpret_cast<PIMAGE_NT_HEADERS>(pNtHeaderBuf);
            }
        }

        return nullptr;
    }

    //FileName��Ϊ��.�����ӳ���ļ���.
    if (!FileName.empty())
    {
        hLocalFile = CreateFile(FileName.c_str(),  dwFileDesireAccessType, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);  //����Ȩ�ޱ��뱣��һ��.
        if (INVALID_HANDLE_VALUE != hLocalFile)
        {
            //�����ļ�ӳ��.
            GetFileSizeEx(hLocalFile, &la);

            hFileMaping = CreateFileMapping(hLocalFile, NULL, dwMapingDesireAccessType, 0, la.LowPart, NULL);
            if (nullptr != hFileMaping)

            {
                //�ɹ���ӳ���ļ�.
                MapViewBuf = MapViewOfFile(hFileMaping,  dwViewDesireAccessType, 0, 0, 0); //ӳ��ȫ��.
                if (nullptr != MapViewBuf)
                {
                    memcpy(pNtHeaderBuf, (char *)MapViewBuf + pDosHeader->e_lfanew, sizeof(IMAGE_NT_HEADERS));
                    pNtHeder = reinterpret_cast<PIMAGE_NT_HEADERS>(pNtHeaderBuf);
                    if (nullptr != pNtHeder && pNtHeder->Signature == 0x4550)
                    {
                        UnmapViewOfFile(MapViewBuf);
                        CloseHandle(hFileMaping);  //�ͷ���Դ����DOSͷ.
                        CloseHandle(hLocalFile);
                        return reinterpret_cast<PIMAGE_NT_HEADERS>(pNtHeaderBuf);
                    }

                }

            }

            SetFilePointer(hLocalFile, pDosHeader->e_lfanew, 0, FILE_BEGIN);  //�ƶ���NTͷλ�ö�ȡ.
            dwRet = ReadFile(hLocalFile, pNtHeaderBuf, sizeof(IMAGE_NT_HEADERS), &dwReadBytes, nullptr);
            if (dwRet != 0)
            {
                //�ɹ�.����з���.
                pNtHeder = reinterpret_cast<PIMAGE_NT_HEADERS>(pNtHeaderBuf);
                if (nullptr != pNtHeder && pNtHeder->Signature == 0X4550)
                {
                    CloseHandle(hLocalFile);
                    return reinterpret_cast<PIMAGE_NT_HEADERS>(pNtHeaderBuf);
                }
            }

        }

        //������ж��ļ���.



        return nullptr;

    }

    if (INVALID_HANDLE_VALUE != hLocalFile)
    {
        CloseHandle(hLocalFile);
    }
    return nullptr;
}

PIMAGE_FILE_HEADER CPeManger::PeHederGetFileHeader(CBinString FileName, HANDLE hFile, DWORD DesireAccess)
{
   
    PIMAGE_NT_HEADERS  pNtHeder = nullptr;
    PIMAGE_FILE_HEADER pFileHeder = nullptr;
    PVOID pFileHederBuf = new IMAGE_FILE_HEADER;

    pNtHeder = PeHederGetNtHeader(FileName, hFile, DesireAccess);
    if (nullptr == pNtHeder)
    {
        return nullptr;
    }

    //��ȡ�ļ�ͷ
    memcpy(pFileHederBuf, &pNtHeder->FileHeader, sizeof(IMAGE_FILE_HEADER));
    pFileHeder = reinterpret_cast<PIMAGE_FILE_HEADER>(pFileHederBuf);

    if (nullptr == pFileHeder || pFileHeder->NumberOfSections <= 0)
    {
        return nullptr;
    }
    
    return reinterpret_cast<PIMAGE_FILE_HEADER> (pFileHederBuf);
}

PIMAGE_OPTIONAL_HEADER CPeManger::PeHederGetOptHeader(CBinString FileName, HANDLE hFile, DWORD DesireAccess)
{
    PIMAGE_NT_HEADERS  pNtHeder = nullptr;
    PIMAGE_OPTIONAL_HEADER pOptHeader = nullptr;
    PVOID pOptHederBuf = new IMAGE_OPTIONAL_HEADER;

    pNtHeder = PeHederGetNtHeader(FileName, hFile, DesireAccess);
    if (nullptr == pNtHeder)
    {
        return nullptr;
    }

    //��ȡ�ļ�ͷ
    memcpy(pOptHederBuf, &pNtHeder->OptionalHeader, sizeof(IMAGE_OPTIONAL_HEADER));
    pOptHeader = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(pOptHederBuf);

    if (nullptr == pOptHeader || pOptHeader->SizeOfImage == 0 || pOptHeader->AddressOfEntryPoint == 0)
    {
        return nullptr;
    }

    return reinterpret_cast<PIMAGE_OPTIONAL_HEADER> (pOptHederBuf);
   
}

//��ȡPE�Ľڱ�
vector<PIMAGE_SECTION_HEADER> * CPeManger::PeHederGetSectionHeader(CBinString FileName, HANDLE hFile, DWORD DesireAccess)
{


    HANDLE hFileMaping = nullptr;
    LPVOID MapViewBuf = nullptr;
    DWORD dwReadBytes = 0;
    DWORD dwRet = 0;
    LARGE_INTEGER la = { 0 };
    HANDLE hLocalFile = INVALID_HANDLE_VALUE;
    BOOL bRet = FALSE;
    PIMAGE_DOS_HEADER pDosHead = PeHederGetDosHeader(FileName, hFile, DesireAccess);
    PIMAGE_NT_HEADERS pNtHeader = PeHederGetNtHeader(FileName, hFile, DesireAccess);
    PIMAGE_FILE_HEADER pFileHeder = PeHederGetFileHeader(FileName, hFile, DesireAccess);

    if (nullptr == pNtHeader || nullptr == pNtHeader || nullptr == pFileHeder)
        return nullptr;

    DWORD dwSectionCount = pFileHeder->NumberOfSections;




    DWORD dwMapingDesireAccessType = 0;
    DWORD dwViewDesireAccessType = 0;
    DWORD dwFileDesireAccessType = 0;

    bRet = PeDesireInit(DesireAccess, dwMapingDesireAccessType, dwViewDesireAccessType, dwFileDesireAccessType);
    if (!bRet)
        return nullptr;

    vector<PIMAGE_SECTION_HEADER> *pSectionList = new vector<PIMAGE_SECTION_HEADER>;
    PIMAGE_SECTION_HEADER pSection = nullptr;

    if (INVALID_HANDLE_VALUE != hFile)
    {
        //����FileName string ֱ��ͨ�����ȥ��.

        //��ȡ�ļ���С.ӳ���ļ���С��ô��.

        GetFileSizeEx(hFile, &la);

        hFileMaping = CreateFileMapping(hFile, NULL, dwMapingDesireAccessType, 0, la.LowPart, NULL);
        if (nullptr != hFileMaping)

        {
            //�ɹ���ӳ���ļ�.
            MapViewBuf = MapViewOfFile(hFileMaping,  dwViewDesireAccessType, 0, 0, 0); //ӳ��ȫ��.
            if (nullptr != MapViewBuf)
            {
                for (DWORD i = 0; i < dwSectionCount; i++)
                {
                    PIMAGE_SECTION_HEADER pSectionBuf = new IMAGE_SECTION_HEADER();
                    memcpy(pSectionBuf, (char *)MapViewBuf + sizeof(IMAGE_NT_HEADERS) + pDosHead->e_lfanew + (i * sizeof(IMAGE_SECTION_HEADER)), sizeof(IMAGE_SECTION_HEADER));
                    pSection = reinterpret_cast<PIMAGE_SECTION_HEADER>(pSectionBuf);
                    pSectionList->push_back(reinterpret_cast<PIMAGE_SECTION_HEADER>(pSectionBuf));
                }

                UnmapViewOfFile(MapViewBuf);
                CloseHandle(hFileMaping);  //�ͷ���Դ����DOSͷ.
                return pSectionList;
            }

            //�ļ�ӳ��ʧ��.��ôʹ���ƶ��ļ�ָ���ȡ�ļ��ķ�ʽ

        }


        SetFilePointer(hFile, pDosHead->e_lfanew + sizeof(IMAGE_NT_HEADERS), 0, FILE_BEGIN);
        PVOID pSectionBuf = new IMAGE_SECTION_HEADER();
        dwRet = ReadFile(hFile, pSectionBuf, sizeof(IMAGE_SECTION_HEADER), &dwReadBytes, nullptr);

        if (dwRet == 0)
        {
            return nullptr;
        }
        pSectionList->push_back(reinterpret_cast<PIMAGE_SECTION_HEADER>(pSectionBuf));
        for (DWORD i = 1; i < dwSectionCount; i++)
        {

            PVOID pSectionBuf = new IMAGE_SECTION_HEADER();
            dwRet = ReadFile(hFile, pSectionBuf, sizeof(IMAGE_SECTION_HEADER), &dwReadBytes, nullptr);

            if (dwRet == 0)
            {
                return nullptr;
            }
            pSectionList->push_back(reinterpret_cast<PIMAGE_SECTION_HEADER>(pSectionBuf));

        }

        return pSectionList;
    }

    //�õ�Section
    //���ļ����򿪲���ӳ��.

    if (!FileName.empty())
    {
        hLocalFile = CreateFile(FileName.c_str(), dwFileDesireAccessType, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);  //����Ȩ�ޱ��뱣��һ��.
        if (INVALID_HANDLE_VALUE != hLocalFile)
        {
            //�����ļ�ӳ��.
            GetFileSizeEx(hLocalFile, &la);

            hFileMaping = CreateFileMapping(hLocalFile, NULL, dwMapingDesireAccessType, 0, la.LowPart, NULL);
            if (nullptr != hFileMaping)

            {
                //�ɹ���ӳ���ļ�.
                MapViewBuf = MapViewOfFile(hFileMaping, dwViewDesireAccessType, 0, 0, 0); //ӳ��ȫ��.
                if (nullptr != MapViewBuf)
                {
                    for (DWORD i = 0; i < dwSectionCount; i++)
                    {
                        PIMAGE_SECTION_HEADER pSectionBuf = new IMAGE_SECTION_HEADER();
                        memcpy(pSectionBuf, (char *)MapViewBuf + sizeof(IMAGE_NT_HEADERS) + pDosHead->e_lfanew + (i * sizeof(IMAGE_SECTION_HEADER)), sizeof(IMAGE_SECTION_HEADER));
                        pSection = reinterpret_cast<PIMAGE_SECTION_HEADER>(pSectionBuf);
                        pSectionList->push_back(reinterpret_cast<PIMAGE_SECTION_HEADER>(pSectionBuf));
                    }

                    UnmapViewOfFile(MapViewBuf);
                    CloseHandle(hFileMaping);  //�ͷ���Դ����DOSͷ.
                    return pSectionList;
                }
                return nullptr;
            }



            SetFilePointer(hFile, pDosHead->e_lfanew + sizeof(IMAGE_NT_HEADERS), 0, FILE_BEGIN);
            PVOID pSectionBuf = new IMAGE_SECTION_HEADER();
            dwRet = ReadFile(hFile, pSectionBuf, sizeof(IMAGE_SECTION_HEADER), &dwReadBytes, nullptr);

            if (dwRet == 0)
            {
                return nullptr;
            }
            pSectionList->push_back(reinterpret_cast<PIMAGE_SECTION_HEADER>(pSectionBuf));
            for (DWORD i = 1; i < dwSectionCount; i++)
            {

                PVOID pSectionBuf = new IMAGE_SECTION_HEADER();
                dwRet = ReadFile(hFile, pSectionBuf, sizeof(IMAGE_SECTION_HEADER), &dwReadBytes, nullptr);

                if (dwRet == 0)
                {
                    return nullptr;
                }
                pSectionList->push_back(reinterpret_cast<PIMAGE_SECTION_HEADER>(pSectionBuf));

            }

            return pSectionList;


        }
        return nullptr;
    }
    return nullptr;
}

//��ȡĿ¼�����
vector<PIMAGE_DATA_DIRECTORY>* CPeManger::PeDirGetDirectory(CBinString FileName, HANDLE hFile, DWORD DesireAccess)
{

    vector<PIMAGE_DATA_DIRECTORY>* pDirectoryList = new vector<PIMAGE_DATA_DIRECTORY>(); //�����������.
    /*
    ˼·:
     ͨ��Optͷ�ҵ�Ŀ¼��.��ȡĿ¼���е�����.������������,���Ҽ��뵽�����н��з���.
    */
    DWORD dwDirectoryCount = 0;
    PIMAGE_OPTIONAL_HEADER pOptHeder = nullptr;

    pOptHeder = PeHederGetOptHeader(FileName, hFile, DesireAccess);
    if (nullptr == pOptHeder)
    {
        return nullptr;
    }

    
    dwDirectoryCount = pOptHeder->NumberOfRvaAndSizes;
    for (DWORD i = 0; i < dwDirectoryCount; i++)
    {
        PVOID pDirectoryBuf = new PIMAGE_DATA_DIRECTORY();
        if (nullptr == pDirectoryBuf)
            return nullptr;
        memcpy(pDirectoryBuf, &pOptHeder->DataDirectory[i], sizeof(IMAGE_DATA_DIRECTORY));
        pDirectoryList->push_back(reinterpret_cast<PIMAGE_DATA_DIRECTORY>(pDirectoryBuf));
    }

    return pDirectoryList;
}


//��ȡ�����
vector<PIMAGE_IMPORT_DESCRIPTOR>* CPeManger::PeImportGetFileImportList(CBinString FileName, HANDLE hFile, DWORD DesireAccess)
{

    /*
    1.ͨ��Ŀ¼���ҵ������� RVA �Լ���С
    2.��ȡ�ڱ�.ͨ����.�ж�RVA �����Ǹ�����
    */

    vector<PIMAGE_IMPORT_DESCRIPTOR>* pImportList = nullptr;             //�����ṹ
    vector<PIMAGE_DATA_DIRECTORY> *pDataDirectoryList = nullptr;          //Ŀ¼��ṹ
    vector<PIMAGE_SECTION_HEADER> *pSectionList = nullptr;                //��ȡ���нڱ�
    PIMAGE_DATA_DIRECTORY pImportDirectory = nullptr;                       //��������ڵ�Ŀ¼��.
   
    LARGE_INTEGER lSectionMemSize = { 0 };                                       //��¼��ѡ�ڵ��ڴ��С
    LARGE_INTEGER lSectionMemRva = { 0 };                                       //��¼��ѡ�ڵ��ڴ���ʼλ��
    LARGE_INTEGER lSectionFoAsize = { 0 };                                       //��¼��ѡ�ڵ��ļ���С
    LARGE_INTEGER lSectionFoa = { 0 };                                       //��¼��ѡ�ڵ��ļ���ʼλ��

    PIMAGE_IMPORT_DESCRIPTOR pImprtDescriptor = nullptr;

    LARGE_INTEGER limportBeginFAOffset = { 0 };                            //��¼��������ļ��е���ʼλ��.
    LARGE_INTEGER limportBeginVAOffset = { 0 };                            //���ӳ���ڴ����ʾ���ڴ��λ��

    pImportList = new vector<PIMAGE_IMPORT_DESCRIPTOR>();
    if (nullptr == pImportList)
    {
        return nullptr;
    }
    pDataDirectoryList =  PeDirGetDirectory(FileName, hFile, DesireAccess);
    if (nullptr == pDataDirectoryList)
    {
        return nullptr;
    }
    pSectionList = PeHederGetSectionHeader(FileName, hFile, DesireAccess);
    if (nullptr == pSectionList)
    {
        return nullptr;
    }
/*
��ʼ��õ����
1.ͨ��Ŀ¼��ĵڶ���õ������� RVA �Լ� Size
2.ͨ���ڱ��ж� RVA���Ǹ��ڵķ�Χ.
3.�ҵ��������ʼ��ַ�Ϳ�ʼ���������.��ŵ�������.
*/
  

    //��ȡĿ¼���� ������ RVA �Լ� SIze
    vector<PIMAGE_DATA_DIRECTORY>::iterator it = pDataDirectoryList->begin();

    int i = 0;
    while (it != pDataDirectoryList->end())
    {
       
        if (i == IMAGE_DIRECTORY_ENTRY_IMPORT)
        {
            pImportDirectory = *it;
            break;
        }
        it++;
        i++;
    }

    //�ж������Ǹ�����. ��¼�Ǹ��ڵ� RVAsize RVA FOASize FOA �Ա������¼.����64��32.
    vector<PIMAGE_SECTION_HEADER>::iterator itSection = pSectionList->begin();
    vector<PIMAGE_SECTION_HEADER>::iterator itSectionCur = itSection;

  
    while (itSection != pSectionList->end())
    {
        itSectionCur = itSection;
        if (pImportDirectory->VirtualAddress > (*itSectionCur)->VirtualAddress && pImportDirectory->VirtualAddress <= (*(itSectionCur + 1))->VirtualAddress)
        {
            
            lSectionMemSize.QuadPart = (*itSectionCur)->Misc.VirtualSize; //�ڴ��С
            lSectionMemRva.QuadPart = (*itSectionCur)->VirtualAddress;    //�ڴ�RVA
            lSectionFoAsize.QuadPart = (*itSectionCur)->SizeOfRawData;    //�ļ�RVA
            lSectionFoa.QuadPart = (*itSectionCur)->PointerToRawData;     //�ļ�����ʼλ��.
            break;
        }

        itSection++;
    }

   


    //��ȡ�������ʼλ��.ע��86 64����. ���㵼���� RVA �� �ļ���FOAλ��.  RVAtoFoa ������. �õ���������ļ��е���ʼλ��.
#ifdef _WIN64
    limportBeginFAOffset.QuadPart =pImportDirectory->VirtualAddress - lSectionMemRva.QuadPart + lSectionFoa.QuadPart;
   // limportBeginVAOffset.QuadPart = pImportDirectory->VirtualAddress;
#else
    limportBeginFAOffset.QuadPart = pImportDirectory->VirtualAddress - lSectionMemRva.LowPart + lSectionFoa.LowPart;
   // limportBeginVAOffset.QuadPart = pImportDirectory->VirtualAddress;
#endif
   
    /*
    1.����ӳ���ļ�. ��ȡ�������Ϣ.���������ŵ�������.
    2.���ļ�����.��ȡ�ļ�. �����������Ϣ�ŵ�������.
    */

    DWORD dwRet = 0;
    DWORD dwReadBytes = 0;
    //�ƶ��ļ�ָ��,��ȡ�ļ�.���������.
    if (INVALID_HANDLE_VALUE != hFile)
    {
#ifdef _WIN64
        SetFilePointer(hFile, limportBeginFAOffset.QuadPart, 0, FILE_BEGIN);
#else
        SetFilePointer(hFile, limportBeginFAOffset.LowPart, 0, FILE_BEGIN);
#endif
       
        while (true)
        {

            PIMAGE_IMPORT_DESCRIPTOR pImportBuf = new IMAGE_IMPORT_DESCRIPTOR(); //�������������ж�ȡ.
            dwRet = ReadFile(hFile, pImportBuf, sizeof(IMAGE_IMPORT_DESCRIPTOR), &dwReadBytes, nullptr);
            if (!dwRet)
            {
                return nullptr;
            }
            if (pImportBuf->Characteristics == 0
                && pImportBuf->FirstThunk == 0
                && pImportBuf->ForwarderChain == 0
                && pImportBuf->Name == 0
                && pImportBuf->OriginalFirstThunk == 0
                && pImportBuf->TimeDateStamp == 0
                )
            {
                return pImportList;
            }
            pImportList->push_back(reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(pImportBuf));
        }
    }
   
    //hfile == NULL
   

    return nullptr;
}

vector<PIMPORT_IMPORTPE_FULLNAME>* CPeManger::PeImportGetFullBaseName(CBinString FileName, HANDLE hFile, DWORD DesireAccess)
{
    /*
    1.��ȡ���е�����RVA
    2.����RVAת���ļ�ƫ��.��ȡģ������.
    */
    vector<PIMAGE_IMPORT_DESCRIPTOR> *pImportList = PeImportGetFileImportList(FileName, hFile, DesireAccess);
    vector<PIMPORT_IMPORTPE_FULLNAME>* pImportFullName = new vector<PIMPORT_IMPORTPE_FULLNAME>();
    if (nullptr == pImportFullName)
    {
        return nullptr;
    }
    if (nullptr == pImportList)
    {
        return nullptr;
    }
    DWORD dwReadBytes = 0;
    DWORD dwRet = 0;
    if (hFile != INVALID_HANDLE_VALUE)
    {
        vector<PIMAGE_IMPORT_DESCRIPTOR>::iterator it = pImportList->begin();
        while (it != pImportList->end())
        {
            PIMPORT_IMPORTPE_FULLNAME pBuffer = new IMPORT_IMPORTPE_FULLNAME();
            if (pBuffer == nullptr)
            {
                return nullptr;
            }
            SetFilePointer(hFile, RvAToFoA((*it)->Name,FileName,hFile,DesireAccess),NULL,FILE_BEGIN );
            char szBuffer[0x100] = { 0 };
            dwRet = ReadFile(hFile, szBuffer, 0x100, &dwReadBytes, nullptr);

            if (dwRet == 0)
            {
                return nullptr;
            }

            pBuffer->ImportDllName = new TCHAR[strlen(szBuffer) + 1];
#ifdef UNICODE
            MultiByteToWideChar(
                CP_ACP, 
                0,
                szBuffer, 
                sizeof(szBuffer) / sizeof(szBuffer[0]), 
                pBuffer->ImportDllName,
                sizeof(TCHAR) * (strlen(szBuffer) + 1));
#else
            strcpy_s(pBuffer->ImportDllName, strlen(szBuffer) + 1, szBuffer);
#endif
           

            pImportFullName->push_back(pBuffer);
            it++;
        }

        return pImportFullName;
    }
    
    return nullptr;
}

BOOL CPeManger::PeFreeImportFileList(vector<PIMAGE_IMPORT_DESCRIPTOR>* pList)
{
    if (pList == NULL)
    {
        return FALSE;
    }
    vector<PIMAGE_IMPORT_DESCRIPTOR>::iterator it = pList->begin();

    while (it != pList->end())
    {

        if (*(it) != NULL)
        {
            delete *it;
        }
        it++;
    }


    
    return TRUE;
}

vector<PIMAGE_EXPORT_DIRECTORY>* CPeManger::PeExPortGetFileExPort(CBinString FileName, HANDLE hFile, DWORD DesireAccess)
{

   /*
   typedef struct _IMAGE_EXPORT_DIRECTORY {
    DWORD   Characteristics;
    DWORD   TimeDateStamp;
    WORD    MajorVersion;
    WORD    MinorVersion;
    DWORD   Name;                                                   Pe������,������DLL sys ...
    DWORD   Base;                   ������������ʼ���
    DWORD   NumberOfFunctions;      ���е��������ĸ���
    DWORD   NumberOfNames;          �����ֵ����ĺ����ĸ���
    DWORD   AddressOfFunctions;     // RVA from base of image       ������ַ���RVA
    DWORD   AddressOfNames;         // RVA from base of image       �������ֱ��RVA
    DWORD   AddressOfNameOrdinals;  // RVA from base of image       ������ű��RVA
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;
   
   2 �� ��
   */

    /*
    1.��ȡ����Ŀ¼
    2.��ȡ����Ŀ¼��1��,�ó������������ļ�λ��
    3.�ж����Ǹ�����.  rva - ��.va + file.fa = FOAλ��
    4.����������.�洢��������.
    */

    vector<PIMAGE_EXPORT_DIRECTORY> *pExPortList = nullptr;

    vector<PIMAGE_DATA_DIRECTORY> * pDirectory = nullptr;
    LARGE_INTEGER laExPortDirRva = { 0 };
    LARGE_INTEGER laExPortFoA = { 0 };
    
    pDirectory = PeDirGetDirectory(FileName,hFile, DesireAccess);

    if (nullptr == pDirectory)
    {
        return nullptr;
   }

   
    
    vector<PIMAGE_DATA_DIRECTORY>::iterator it = pDirectory->begin();

    //�õ�������RVA
    int i = 0;
    while (it != pDirectory->end())
    {
        
        if (i == IMAGE_DIRECTORY_ENTRY_EXPORT)
        {
            laExPortDirRva.QuadPart = (*it)->VirtualAddress;
        }
        i++;
        it++;
    }

    //�ó���������ļ��е�ƫ��.
    laExPortFoA.QuadPart = RvAToFoA(laExPortDirRva.QuadPart,FileName,hFile,DesireAccess);
    DWORD dwRet = 0;
    DWORD dwReadBytes = 0;
    pExPortList = new vector<PIMAGE_EXPORT_DIRECTORY>();
    if (hFile != INVALID_HANDLE_VALUE)
    {
        //���ļ���ȡ

        SetFilePointer(hFile, laExPortFoA.QuadPart, NULL, FILE_BEGIN);
        PIMAGE_EXPORT_DIRECTORY pBuffer = new IMAGE_EXPORT_DIRECTORY();
        dwRet = ReadFile(hFile, pBuffer, sizeof(IMAGE_EXPORT_DIRECTORY), &dwReadBytes, nullptr);
        if (dwRet == 0)
        {
            return nullptr;
        }
        pExPortList->push_back(pBuffer);
    }

    return pExPortList;
}

//��ȡ���е����������ֵ�RVA.
vector<PEXPORT_TYPE_ADDRESSRVA>* CPeManger::PeExPortGetFullFunctionNameRva(CBinString FileName, HANDLE hFile, DWORD DesireAccess)
{
    vector< PEXPORT_TYPE_ADDRESSRVA> *pFullFunctionNameAnAddressRvaList
        = new vector<PEXPORT_TYPE_ADDRESSRVA>();


    vector<PIMAGE_EXPORT_DIRECTORY>* pExPortList = PeExPortGetFileExPort(FileName, hFile, DesireAccess);//��ȡ������
    if (nullptr == pExPortList)
    {
        return nullptr;
    }

    //����������� ������ַ�� �Լ� ����������ֱ�. ����ڽṹ��.

    vector<PIMAGE_EXPORT_DIRECTORY>::iterator it = pExPortList->begin();

    DWORD dwReadBytes;
    DWORD dwRet = 0;
    DWORD dwNumberOfNames = 0;
    while (it != pExPortList->end())
    {
        SetFilePointer(hFile, RvAToFoA((*it)->AddressOfNames, FileName, hFile, DesireAccess), 0, FILE_BEGIN);//�ƶ��ļ���������ַ��ָ���FAλ��.
         //��ȡ�����ֵ����ĺ���
        for (dwNumberOfNames = (*it)->NumberOfNames; dwNumberOfNames != 0; dwNumberOfNames--)
        {
            PEXPORT_TYPE_ADDRESSRVA pBuffer = new EXPORT_TYPE_ADDRESSRVA();
            //rva

            //����������ַ��,�ó����к�����ַ���RVA.������Զ���ṹ��.
            DWORD dwRva = 0;
            //dwRva = RvAToFoA(0x94137,FileName,hFile,DesireAccess);
            dwRet = ReadFile(hFile, &dwRva, sizeof(DWORD), &dwReadBytes, nullptr);
            if (dwRet == 0)
            {
                return nullptr;
            }
            if (dwRva == 0)
            {
                return pFullFunctionNameAnAddressRvaList;
            }
            pBuffer->ExpTypeRva = dwRva;
            pFullFunctionNameAnAddressRvaList->push_back(pBuffer);
          

        }

        return pFullFunctionNameAnAddressRvaList;
    }
    return nullptr;
}


//��ȡ�����������е��������ĵ�ַ���RVA
vector< PEXPORT_TYPE_ADDRESSRVA> * CPeManger::PeExPortGetFullFunctionAddressRva(CBinString FileName, HANDLE hFile, DWORD DesireAccess)
{


    vector< PEXPORT_TYPE_ADDRESSRVA> *pFullFunctionNameAnAddressRvaList
        = new vector<PEXPORT_TYPE_ADDRESSRVA>();


    vector<PIMAGE_EXPORT_DIRECTORY>* pExPortList = PeExPortGetFileExPort(FileName, hFile, DesireAccess);//��ȡ������
    if (nullptr == pExPortList)
    {
        return nullptr;
    }

    //����������� ������ַ�� �Լ� ����������ֱ�. ����ڽṹ��.

    vector<PIMAGE_EXPORT_DIRECTORY>::iterator it = pExPortList->begin();

    DWORD dwReadBytes;
    DWORD dwRet = 0;
    DWORD dwNumberOfNames = 0;
    while (it != pExPortList->end())
    {
        SetFilePointer(hFile, RvAToFoA((*it)->AddressOfFunctions, FileName, hFile, DesireAccess), 0, FILE_BEGIN);//�ƶ��ļ���������ַ��ָ���FAλ��.
         //��ȡ�����ֵ����ĺ���
        for (dwNumberOfNames = (*it)->NumberOfNames; dwNumberOfNames != 0; dwNumberOfNames--)
        {
            PEXPORT_TYPE_ADDRESSRVA pBuffer = new EXPORT_TYPE_ADDRESSRVA();
            //rva

            //����������ַ��,�ó����к�����ַ���RVA.������Զ���ṹ��.
            DWORD dwRva = 0;
            //dwRva = RvAToFoA(0x94137,FileName,hFile,DesireAccess);
            dwRet = ReadFile(hFile, &dwRva, sizeof(DWORD), &dwReadBytes, nullptr);
            if (dwRet == 0)
            {
                return nullptr;
            }
            if (dwRva == 0)
            {
                return pFullFunctionNameAnAddressRvaList;
            }
            pBuffer->ExpTypeRva = dwRva;
            pFullFunctionNameAnAddressRvaList->push_back(pBuffer);


        }

        return pFullFunctionNameAnAddressRvaList;
    }

    return nullptr;
}





//��ȡ��������������
vector<PEXPORT_FULLNAME>* CPeManger::PeExportGetFullFunctionName(CBinString FileName, HANDLE hFile, DWORD DesireAccess)
{
    /*
    1.��ȡ���������� �������ֵ�RVA.
    2.���� RVATOFOA ȡ�ļ��в��Ҷ�Ӧ������
    */
    vector<PEXPORT_TYPE_ADDRESSRVA> *pFunctionNameRva = nullptr;
    vector<PEXPORT_FULLNAME>* pFullFunctionNameList = new vector<PEXPORT_FULLNAME>();
    if (nullptr == pFullFunctionNameList)
    {
        return nullptr;
    }
    pFunctionNameRva = PeExPortGetFullFunctionNameRva(FileName, hFile, DesireAccess);

    if (nullptr == pFunctionNameRva)
    {
        return nullptr;
    }
    DWORD dwReadBytes = 0;
    DWORD dwRet = 0;
    if (hFile != INVALID_HANDLE_VALUE)
    {
        vector<PEXPORT_TYPE_ADDRESSRVA>::iterator it = pFunctionNameRva->begin();
        while (it != pFunctionNameRva->end())
        {
            //�ƶ��ļ�ָ�뵽��������λ��
            PEXPORT_FULLNAME pBuffer = new EXPORT_FULLNAME();
            if (nullptr == pBuffer)
            {
                return nullptr;
            }
            SetFilePointer(hFile, RvAToFoA((*it)->ExpTypeRva, FileName, hFile, DesireAccess), 0, FILE_BEGIN);

            char szBuffer[0x200];
            dwRet = ReadFile(hFile, szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]), &dwReadBytes, nullptr);
            if (0 == dwRet)
            {
                break;
            }

            pBuffer->FunctionName = new TCHAR[strlen(szBuffer) + 1]();
#ifdef UNICODE
           

            MultiByteToWideChar(CP_ACP,
                0,
                szBuffer,
                sizeof(szBuffer) / sizeof(szBuffer[0]),
                pBuffer->FunctionName, sizeof(TCHAR)*(strlen(szBuffer) + 1));
           
            //ȥ�����Ʒ���.
            pBuffer->UndName = new TCHAR[0x100]();

            UnDecorateSymbolNameW(pBuffer->FunctionName, pBuffer->UndName, 0x100, 0);
#else
            pBuffer->UndName = new TCHAR[0x100]();
            strcpy_s(pBuffer->FunctionName, sizeof(szBuffer) / sizeof(szBuffer[0]), szBuffer);
            UnDecorateSymbolName(pBuffer->FunctionName, pBuffer->UndName, 0x100, 0);
#endif
           
            pFullFunctionNameList->push_back(pBuffer);
            it++;
        }
        return pFullFunctionNameList;
    }
    
    return nullptr;
}

DWORD CPeManger::RvAToFoA(DWORD RvA,CBinString FileName,HANDLE hFile,DWORD DesireAccess)
{
    //����RVA ���Ǹ�����.������ RVA���ļ��е�ƫ��

    vector<PIMAGE_SECTION_HEADER> *pSection = nullptr;

    pSection = PeHederGetSectionHeader(FileName, hFile, DesireAccess);
    if (nullptr == pSection)
    {
        return 0;
    }

    vector<PIMAGE_SECTION_HEADER>::iterator it = pSection->begin();
    while (it != pSection->end())
    {
        if (RvA > (*it)->VirtualAddress && RvA < (*(it + 1))->VirtualAddress)
        {
            return RvA - (*it)->VirtualAddress + (*it)->PointerToRawData;
        }

        it++;
    }
    return 0;
}
DWORD CPeManger::FoAToRva(DWORD FoA, CBinString FileName, HANDLE hFile, DWORD DesireAccess)
{
    vector<PIMAGE_SECTION_HEADER> *pSection = nullptr;

    pSection = PeHederGetSectionHeader(FileName, hFile, DesireAccess);
    if (nullptr == pSection)
    {
        return 0;
    }

    vector<PIMAGE_SECTION_HEADER>::iterator it = pSection->begin();
    while (it != pSection->end())
    {
        if (FoA > (*it)->PointerToRawData && FoA < (*(it + 1))->PointerToRawData)
        {
            return FoA -   (*it)->PointerToRawData + (*it)->VirtualAddress;
        }

        it++;
    }
    
    return 0;
}

BOOL CPeManger::AddSection(CBinString FileName, HANDLE hFile)
{

   
    if (hFile == INVALID_HANDLE_VALUE)
    {
        if (FileName.empty())
        {
            return FALSE;
        }

        hFile = CreateFile(FileName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            return NULL;
        }
        //ӳ�䵽�ڴ�.
    }

   
    return 0;
}

