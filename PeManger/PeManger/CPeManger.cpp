#include "pch.h"
#include "CPeManger.h"


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
vector<PIMAGE_IMPORT_DESCRIPTOR>* CPeManger::PeImportGetImport(CBinString FileName, HANDLE hFile, DWORD DesireAccess)
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
        if (pImportDirectory->VirtualAddress > (*itSectionCur)->VirtualAddress && pImportDirectory->VirtualAddress <= (*(itSectionCur = itSectionCur + 1))->VirtualAddress)
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
        PIMAGE_IMPORT_DESCRIPTOR pImportBuf = new IMAGE_IMPORT_DESCRIPTOR(); //�������������ж�ȡ.
        dwRet = ReadFile(hFile, pImportBuf, sizeof(IMAGE_IMPORT_DESCRIPTOR), &dwReadBytes, nullptr);
           if (!dwRet)
           {
               return nullptr;
           }
        while (pImportBuf != nullptr)
        {

            pImportList->push_back(reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(pImportBuf));

            dwRet = ReadFile(hFile, pImportBuf, sizeof(IMAGE_IMPORT_DESCRIPTOR), &dwReadBytes, nullptr);
            if (!dwRet)
            {
                return nullptr;
            }

            PIMAGE_IMPORT_DESCRIPTOR pImportBuf = new IMAGE_IMPORT_DESCRIPTOR(); //�������������ж�ȡ.
        }
    }
   

    return nullptr;
}



