#include "pch.h"
#include "CPeManger.h"


CPeManger::CPeManger()
{
}


CPeManger::~CPeManger()
{
}

//初始化权限类型信息 权限类型相关.
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
获取PE头信息
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
    // 根据文件句柄映射或者打开文件.
    if (INVALID_HANDLE_VALUE != hFile)
    {
        //无视FileName string 直接通过句柄去做.
       
        //获取文件大小.映射文件大小那么大.
       
        GetFileSizeEx(hFile, &la);
     
        hFileMaping =  CreateFileMapping(hFile, NULL, dwMapingDesireAccessType, 0, la.LowPart, NULL);
        if (nullptr != hFileMaping)

        {
            //成功则映射文件.
            MapViewBuf = MapViewOfFile(hFileMaping, dwViewDesireAccessType, 0, 0, 0); //映射全部.
            if (nullptr != MapViewBuf)
            {
                memcpy(DosHeadBuffer, MapViewBuf, sizeof(IMAGE_DOS_HEADER));
                pDosHead = reinterpret_cast<PIMAGE_DOS_HEADER>(DosHeadBuffer);
                if (nullptr != pDosHead && pDosHead->e_magic == 0x5A4D)
                {
                    UnmapViewOfFile(MapViewBuf);
                    CloseHandle(hFileMaping);  //释放资源返回DOS头.
                    return reinterpret_cast<PIMAGE_DOS_HEADER>(DosHeadBuffer);
                }

            }

        }
//否则映射失败.以读文件的方式打开

       
        dwRet = ReadFile(hFile, DosHeadBuffer, sizeof(IMAGE_DOS_HEADER), &dwReadBytes, nullptr);
        if (dwRet != 0)
        {
            //成功.则进行返回.
            pDosHead = reinterpret_cast<PIMAGE_DOS_HEADER>(DosHeadBuffer);
            if (nullptr != pDosHead && pDosHead->e_magic == 0x5A4D)
            {
                return reinterpret_cast<PIMAGE_DOS_HEADER>(DosHeadBuffer);
            }
        }

        return nullptr;
    }
    
    //FileName不为空.则进行映射文件打开.
    if (!FileName.empty())
    {
        hLocalFile = CreateFile(FileName.c_str(), dwFileDesireAccessType, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);  //三方权限必须保持一样.
        if (INVALID_HANDLE_VALUE != hLocalFile)
        {
            //进行文件映射.
            GetFileSizeEx(hLocalFile, &la);

            hFileMaping = CreateFileMapping(hLocalFile, NULL, dwMapingDesireAccessType, 0, la.LowPart, NULL);
            if (nullptr != hFileMaping)

            {
                //成功则映射文件.
                MapViewBuf = MapViewOfFile(hFileMaping,  dwViewDesireAccessType, 0, 0, 0); //映射全部.
                if (nullptr != MapViewBuf)
                {
                    memcpy(DosHeadBuffer, MapViewBuf, sizeof(IMAGE_DOS_HEADER));
                    pDosHead = reinterpret_cast<PIMAGE_DOS_HEADER>(DosHeadBuffer);
                    if (nullptr != pDosHead && pDosHead->e_magic == 0x5A4D)
                    {
                        UnmapViewOfFile(MapViewBuf);
                        CloseHandle(hFileMaping);  //释放资源返回DOS头.
                        CloseHandle(hLocalFile);
                        return reinterpret_cast<PIMAGE_DOS_HEADER>(DosHeadBuffer);
                    }

                }

            }


            dwRet = ReadFile(hLocalFile, DosHeadBuffer, sizeof(IMAGE_DOS_HEADER), &dwReadBytes, nullptr);
            if (dwRet != 0)
            {
                //成功.则进行返回.
                pDosHead = reinterpret_cast<PIMAGE_DOS_HEADER>(DosHeadBuffer);
                if (nullptr != pDosHead && pDosHead->e_magic == 0x5A4D)
                {
                    CloseHandle(hLocalFile);
                    return reinterpret_cast<PIMAGE_DOS_HEADER>(DosHeadBuffer);
                }
            }

        }

        //否则进行读文件打开.

      

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

    //hFile 不为空 映射 或者 读文件返回.


    DWORD dwMapingDesireAccessType = 0;
    DWORD dwViewDesireAccessType = 0;
    DWORD dwFileDesireAccessType = 0;

    bRet =  PeDesireInit(DesireAccess, dwMapingDesireAccessType, dwViewDesireAccessType, dwFileDesireAccessType);
    if (!bRet)
        return nullptr;



    if (INVALID_HANDLE_VALUE != hFile)
    {
        //无视FileName string 直接通过句柄去做.

        //获取文件大小.映射文件大小那么大.

        GetFileSizeEx(hFile, &la);

        hFileMaping = CreateFileMapping(hFile, NULL, dwMapingDesireAccessType, 0, la.LowPart, NULL);
        if (nullptr != hFileMaping)

        {
            //成功则映射文件.
            MapViewBuf = MapViewOfFile(hFileMaping,  dwViewDesireAccessType, 0, 0, 0); //映射全部.
            if (nullptr != MapViewBuf)
            {
                memcpy(pNtHeaderBuf, (char *)MapViewBuf + pDosHeader->e_lfanew, sizeof(IMAGE_NT_HEADERS));
                pNtHeder = reinterpret_cast<PIMAGE_NT_HEADERS>(pNtHeaderBuf);
                if (nullptr != pNtHeder && pNtHeder->Signature == 0x4550)
                {
                    UnmapViewOfFile(MapViewBuf);
                    CloseHandle(hFileMaping);  //释放资源返回DOS头.
                    return reinterpret_cast<PIMAGE_NT_HEADERS>(pNtHeaderBuf);
                }

            }

        }
        //否则映射失败.以读文件的方式打开

        SetFilePointer(hFile, pDosHeader->e_lfanew, 0, FILE_BEGIN);  //移动到NT头位置读取.
        dwRet = ReadFile(hFile, pNtHeaderBuf, sizeof(IMAGE_NT_HEADERS), &dwReadBytes, nullptr);
        if (dwRet != 0)
        {
            //成功.则进行返回.
            pNtHeder = reinterpret_cast<PIMAGE_NT_HEADERS>(pNtHeaderBuf);
            if (nullptr != pNtHeder && pNtHeder->Signature == 0X4550)
            {
                return reinterpret_cast<PIMAGE_NT_HEADERS>(pNtHeaderBuf);
            }
        }

        return nullptr;
    }

    //FileName不为空.则进行映射文件打开.
    if (!FileName.empty())
    {
        hLocalFile = CreateFile(FileName.c_str(),  dwFileDesireAccessType, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);  //三方权限必须保持一样.
        if (INVALID_HANDLE_VALUE != hLocalFile)
        {
            //进行文件映射.
            GetFileSizeEx(hLocalFile, &la);

            hFileMaping = CreateFileMapping(hLocalFile, NULL, dwMapingDesireAccessType, 0, la.LowPart, NULL);
            if (nullptr != hFileMaping)

            {
                //成功则映射文件.
                MapViewBuf = MapViewOfFile(hFileMaping,  dwViewDesireAccessType, 0, 0, 0); //映射全部.
                if (nullptr != MapViewBuf)
                {
                    memcpy(pNtHeaderBuf, (char *)MapViewBuf + pDosHeader->e_lfanew, sizeof(IMAGE_NT_HEADERS));
                    pNtHeder = reinterpret_cast<PIMAGE_NT_HEADERS>(pNtHeaderBuf);
                    if (nullptr != pNtHeder && pNtHeder->Signature == 0x4550)
                    {
                        UnmapViewOfFile(MapViewBuf);
                        CloseHandle(hFileMaping);  //释放资源返回DOS头.
                        CloseHandle(hLocalFile);
                        return reinterpret_cast<PIMAGE_NT_HEADERS>(pNtHeaderBuf);
                    }

                }

            }

            SetFilePointer(hLocalFile, pDosHeader->e_lfanew, 0, FILE_BEGIN);  //移动到NT头位置读取.
            dwRet = ReadFile(hLocalFile, pNtHeaderBuf, sizeof(IMAGE_NT_HEADERS), &dwReadBytes, nullptr);
            if (dwRet != 0)
            {
                //成功.则进行返回.
                pNtHeder = reinterpret_cast<PIMAGE_NT_HEADERS>(pNtHeaderBuf);
                if (nullptr != pNtHeder && pNtHeder->Signature == 0X4550)
                {
                    CloseHandle(hLocalFile);
                    return reinterpret_cast<PIMAGE_NT_HEADERS>(pNtHeaderBuf);
                }
            }

        }

        //否则进行读文件打开.



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

    //获取文件头
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

    //获取文件头
    memcpy(pOptHederBuf, &pNtHeder->OptionalHeader, sizeof(IMAGE_OPTIONAL_HEADER));
    pOptHeader = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(pOptHederBuf);

    if (nullptr == pOptHeader || pOptHeader->SizeOfImage == 0 || pOptHeader->AddressOfEntryPoint == 0)
    {
        return nullptr;
    }

    return reinterpret_cast<PIMAGE_OPTIONAL_HEADER> (pOptHederBuf);
   
}

//获取PE的节表
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
        //无视FileName string 直接通过句柄去做.

        //获取文件大小.映射文件大小那么大.

        GetFileSizeEx(hFile, &la);

        hFileMaping = CreateFileMapping(hFile, NULL, dwMapingDesireAccessType, 0, la.LowPart, NULL);
        if (nullptr != hFileMaping)

        {
            //成功则映射文件.
            MapViewBuf = MapViewOfFile(hFileMaping,  dwViewDesireAccessType, 0, 0, 0); //映射全部.
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
                CloseHandle(hFileMaping);  //释放资源返回DOS头.
                return pSectionList;
            }

            //文件映射失败.那么使用移动文件指针读取文件的方式

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

    //得到Section
    //以文件名打开并且映射.

    if (!FileName.empty())
    {
        hLocalFile = CreateFile(FileName.c_str(), dwFileDesireAccessType, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);  //三方权限必须保持一样.
        if (INVALID_HANDLE_VALUE != hLocalFile)
        {
            //进行文件映射.
            GetFileSizeEx(hLocalFile, &la);

            hFileMaping = CreateFileMapping(hLocalFile, NULL, dwMapingDesireAccessType, 0, la.LowPart, NULL);
            if (nullptr != hFileMaping)

            {
                //成功则映射文件.
                MapViewBuf = MapViewOfFile(hFileMaping, dwViewDesireAccessType, 0, 0, 0); //映射全部.
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
                    CloseHandle(hFileMaping);  //释放资源返回DOS头.
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

//获取目录表表项
vector<PIMAGE_DATA_DIRECTORY>* CPeManger::PeDirGetDirectory(CBinString FileName, HANDLE hFile, DWORD DesireAccess)
{

    vector<PIMAGE_DATA_DIRECTORY>* pDirectoryList = new vector<PIMAGE_DATA_DIRECTORY>(); //创建链表对象.
    /*
    思路:
     通过Opt头找到目录表.获取目录表中的内容.拷贝到缓冲区,并且加入到链表中进行返回.
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


//获取导入表
vector<PIMAGE_IMPORT_DESCRIPTOR>* CPeManger::PeImportGetImport(CBinString FileName, HANDLE hFile, DWORD DesireAccess)
{

    /*
    1.通过目录项找到导入表的 RVA 以及大小
    2.获取节表.通过节.判断RVA 落在那个节上
    */

    vector<PIMAGE_IMPORT_DESCRIPTOR>* pImportList = nullptr;             //导入表结构
    vector<PIMAGE_DATA_DIRECTORY> *pDataDirectoryList = nullptr;          //目录表结构
    vector<PIMAGE_SECTION_HEADER> *pSectionList = nullptr;                //获取所有节表
    PIMAGE_DATA_DIRECTORY pImportDirectory = nullptr;                       //导入表所在的目录项.
   
    LARGE_INTEGER lSectionMemSize = { 0 };                                       //记录落选节的内存大小
    LARGE_INTEGER lSectionMemRva = { 0 };                                       //记录落选节的内存起始位置
    LARGE_INTEGER lSectionFoAsize = { 0 };                                       //记录落选节的文件大小
    LARGE_INTEGER lSectionFoa = { 0 };                                       //记录落选节的文件起始位置

    PIMAGE_IMPORT_DESCRIPTOR pImprtDescriptor = nullptr;

    LARGE_INTEGER limportBeginFAOffset = { 0 };                            //记录导入表在文件中的起始位置.
    LARGE_INTEGER limportBeginVAOffset = { 0 };                            //如果映射内存则表示在内存的位置

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
开始获得导入表
1.通过目录表的第二项得到导入表的 RVA 以及 Size
2.通过节表判断 RVA在那个节的范围.
3.找到导入表起始地址就开始遍历导入表.存放到链表中.
*/
  

    //获取目录表中 导入表的 RVA 以及 SIze
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

    //判断落在那个节中. 记录那个节的 RVAsize RVA FOASize FOA 以标记来记录.兼容64跟32.
    vector<PIMAGE_SECTION_HEADER>::iterator itSection = pSectionList->begin();
    vector<PIMAGE_SECTION_HEADER>::iterator itSectionCur = itSection;

  
    while (itSection != pSectionList->end())
    {
        itSectionCur = itSection;
        if (pImportDirectory->VirtualAddress > (*itSectionCur)->VirtualAddress && pImportDirectory->VirtualAddress <= (*(itSectionCur = itSectionCur + 1))->VirtualAddress)
        {
            
            lSectionMemSize.QuadPart = (*itSectionCur)->Misc.VirtualSize; //内存大小
            lSectionMemRva.QuadPart = (*itSectionCur)->VirtualAddress;    //内存RVA
            lSectionFoAsize.QuadPart = (*itSectionCur)->SizeOfRawData;    //文件RVA
            lSectionFoa.QuadPart = (*itSectionCur)->PointerToRawData;     //文件的起始位置.
            break;
        }

        itSection++;
    }

   


    //获取导入表起始位置.注意86 64区别. 计算导入表的 RVA 在 文件的FOA位置.  RVAtoFoa 不对齐. 得到导入表在文件中的起始位置.
#ifdef _WIN64
    limportBeginFAOffset.QuadPart =pImportDirectory->VirtualAddress - lSectionMemRva.QuadPart + lSectionFoa.QuadPart;
   // limportBeginVAOffset.QuadPart = pImportDirectory->VirtualAddress;
#else
    limportBeginFAOffset.QuadPart = pImportDirectory->VirtualAddress - lSectionMemRva.LowPart + lSectionFoa.LowPart;
   // limportBeginVAOffset.QuadPart = pImportDirectory->VirtualAddress;
#endif
   
    /*
    1.创建映射文件. 读取导入表信息.遍历导入表放到链表中.
    2.打开文件对象.读取文件. 遍历导入表信息放到链表中.
    */

    DWORD dwRet = 0;
    DWORD dwReadBytes = 0;
    //移动文件指针,读取文件.遍历导入表.
    if (INVALID_HANDLE_VALUE != hFile)
    {
#ifdef _WIN64
        SetFilePointer(hFile, limportBeginFAOffset.QuadPart, 0, FILE_BEGIN);
#else
        SetFilePointer(hFile, limportBeginFAOffset.LowPart, 0, FILE_BEGIN);
#endif
        PIMAGE_IMPORT_DESCRIPTOR pImportBuf = new IMAGE_IMPORT_DESCRIPTOR(); //创建缓冲区进行读取.
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

            PIMAGE_IMPORT_DESCRIPTOR pImportBuf = new IMAGE_IMPORT_DESCRIPTOR(); //创建缓冲区进行读取.
        }
    }
   

    return nullptr;
}



