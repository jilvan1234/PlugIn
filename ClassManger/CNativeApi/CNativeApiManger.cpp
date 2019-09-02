#include "pch.h"
#include "CNativeApiManger.h"


CNativeApiManger::CNativeApiManger()
{
    m_ZwQueryInformationProcess = NULL;
    m_ZwQueryObject = NULL;
    m_ZwQuerySystemInformation = NULL;

    m_ZwQueryInformationProcess = NatMmGetProcessRoutine(TEXT("ntdll.dll"), "ZwQueryInformationProcess");
    m_ZwQuerySystemInformation = NatMmGetProcessRoutine(TEXT("ntdll.dll"), "ZwQuerySystemInformation");
    m_ZwQueryObject = NatMmGetProcessRoutine(TEXT("ntdll.dll"), "ZwQueryObject");


    ZwQueryInformationProcess = NULL;
    ZwQueryObject = NULL;
    ZwQuerySystemInformation = NULL;

    ZwQueryInformationProcess =  
        reinterpret_cast<PfnZwQueryInformationProcess>(NatMmGetProcessRoutine(TEXT("ntdll.dll"), "ZwQueryInformationProcess"));

    ZwQueryObject = reinterpret_cast<PfnZwQueryObject>(NatMmGetProcessRoutine(TEXT("ntdll.dll"), "ZwQueryObject"));

    ZwQuerySystemInformation = reinterpret_cast<PfnZwQuerySystemInformation>( NatMmGetProcessRoutine(TEXT("ntdll.dll"), "ZwQuerySystemInformation"));

}


CNativeApiManger::~CNativeApiManger()
{
}


PVOID CNativeApiManger::NatMmGetProcessRoutine(CBinString ModelName, string FunctionName)
{

    if (ModelName.empty() || FunctionName.empty())
        return NULL;
    HMODULE hDll = LoadLibrary(ModelName.c_str());
    if (NULL == hDll)
        return NULL;
    return GetProcAddress(hDll, FunctionName.c_str());
}
PVOID CNativeApiManger::NatGetZwQueryInformationProcess() const
{
 
   
    return m_ZwQueryInformationProcess;

}
PVOID CNativeApiManger::NatGetZwQueryObject() const
{
  
    return m_ZwQueryObject;
}
PVOID CNativeApiManger::NatGetZwQuerySystemInformation()const
{
   
    return m_ZwQuerySystemInformation;
}

USHORT CNativeApiManger::NatHndGetHandleTypeWithHandle(HANDLE handle)
{

    if (INVALID_HANDLE_VALUE == handle || 0 == handle)
        return 0;
    if (NULL == m_ZwQueryObject)
        return 0;

    PfnZwQueryObject CurZwQueryObject = NULL;
    CurZwQueryObject = reinterpret_cast<PfnZwQueryObject>(m_ZwQueryObject);
    if (CurZwQueryObject == NULL)
        return 0;

    char * pBuffer = new char[0x300];
    ULONG uRetSize = 0;
    OBJECT_INFORMATION_CLASS ob = ObjectTypeInformation;
    CurZwQueryObject(handle, ob, pBuffer, 0x300, &uRetSize);
    USHORT Type = 0;
    PPUBLIC_OBJECT_TYPE_INFORMATION PtypeInfo = reinterpret_cast<PPUBLIC_OBJECT_TYPE_INFORMATION>(pBuffer);  //��ѯ������Ϣ
    Type = PtypeInfo->MaintainTypeList;
    delete[] pBuffer;

    return Type;
 
}

BOOL CNativeApiManger::NatHndDuplicateHandle(HANDLE hSourceProcessHandle, HANDLE hSourceHandle, HANDLE hTargetProcessHandle, LPHANDLE lpTargetHandle, DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwOptions)
{


     return DuplicateHandle(
        hSourceProcessHandle,
        hSourceHandle,
        hTargetProcessHandle,
        lpTargetHandle,
        dwDesiredAccess,
        bInheritHandle,
        dwOptions);;
}

BOOL CNativeApiManger::NatHndCloseSpecilProcessOccFileHandle(DWORD dwPid, string OccFileName)
{

    //1.��ʼ�� Native API

    
    //2.��ȡ�ļ�����.
    DWORD dwSystemFileHandleType = 0;
    TCHAR szPath[MAX_PATH] = { 0 };
    GetModuleFileName(GetModuleHandle(NULL), szPath, MAX_PATH * sizeof(TCHAR));
    HANDLE hFile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
        return 0;
    dwSystemFileHandleType = NatHndGetHandleTypeWithHandle(hFile);
    
    CloseHandle(hFile);
    //��.����ָ�����̵ľ������
    DWORD dwProcessHandleCount = 0;
    HANDLE hProcess = 0;
    ULONG uRetLength = 0;
    


    hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE, dwPid);

    if (0 == hProcess)
        return 0;
    ZwQueryInformationProcess(hProcess, ProcessHandleCount, &dwProcessHandleCount, sizeof(dwProcessHandleCount), &uRetLength);
    //DUPLICATE_CLOSE_SOURCE

    //4.�ӽ����п������.�жϲ�ѯ����.������������.�����¿���һ�¹ر�Ԫ��� ���߳�.

    HANDLE hTarHandle;
    for (int i = 0; i < 0x40000; i++) //65535
    {
        if (NatHndDuplicateHandle(hProcess, (HANDLE)i, GetCurrentProcess(), &hTarHandle, 0, 0, 2))
        {
            //�ɹ���.��ѯ������Ϣ

            char * pBuffer = new char[0x300]();
            ULONG uRetSize = 0;
            OBJECT_INFORMATION_CLASS ob = ObjectTypeInformation;
            ZwQueryObject((HANDLE)hTarHandle, ob, pBuffer, 0x300, &uRetSize);

            PPUBLIC_OBJECT_TYPE_INFORMATION PtypeInfo = reinterpret_cast<PPUBLIC_OBJECT_TYPE_INFORMATION>(pBuffer);  //��ѯ������Ϣ
            PtypeInfo->MaintainTypeList;

            if (PtypeInfo->MaintainTypeList == dwSystemFileHandleType)
            {
                //���ļ�.����ʹ��DumpLicateHandle ����DUPLICATE_CLOSE_SOURCE ���ر�.
                //psOpt.HndDuplicateHandle(hProcess, (HANDLE)i, GetCurrentProcess(), NULL, 0, 0, DUPLICATE_CLOSE_SOURCE);

                //��������ļ�.��ѯ���ļ���.
                OBJECT_INFORMATION_CLASS ob = ObjectFileInformation;
                char * Buffer1 = new char[0x100];
                memset(Buffer1, 0, 0x100);
                ZwQueryObject(hTarHandle, ob, Buffer1, 0x100, &uRetLength);
                POBJECT_NAME_INFORMATION pFileInfo = reinterpret_cast<POBJECT_NAME_INFORMATION>(Buffer1);
                PWSTR pszBuffer = new wchar_t[0x255]();

                char szBuffer[0x256] = { 0 };
                if (pFileInfo->Name.Buffer == 0)
                    continue;
                memcpy(pszBuffer, pFileInfo->Name.Buffer, pFileInfo->Name.Length);

                WideCharToMultiByte(CP_ACP, 0, pszBuffer, 0X256 * sizeof(TCHAR), szBuffer, 0x256, 0, 0);
                string str = szBuffer;
                str.substr(str.find_last_of("\\"));
                if (str.find(OccFileName) != string::npos)
                {
                    //�ر�����
                    NatHndDuplicateHandle(hProcess, (HANDLE)i, GetCurrentProcess(), NULL, 0, 0, DUPLICATE_CLOSE_SOURCE);
                    CloseHandle(hProcess);
                    return TRUE;
                }

            }

        }
    }
    CloseHandle(hProcess);

    return FALSE;
}

PVOID CNativeApiManger::NatHndGetSystemHandleInfo()
{
    

    //����ϵͳ�����Ϣ.
    PULONG szBuffer = NULL;
    DWORD dwSize = 0x1000;
    DWORD dwRetSize = 0;
    NTSTATUS ntStatus;
    szBuffer = new ULONG[dwSize];
    if (NULL == szBuffer)
        return NULL;
    if (NULL == ZwQuerySystemInformation)
        return NULL;
    //��һ����ÿ��ܲ��ɹ�.���Ի�ȡ����ֵ. ���Ҹ���DwRetSize��ֵȥ��������ռ�
    do
    {
        ntStatus = ZwQuerySystemInformation(SystemHandleInformation, szBuffer, dwSize, &dwRetSize);
        if (ntStatus == STATUS_INFO_LENGTH_MISMATCH) //����ռ䲻��.��������ռ�
        {

            delete[] szBuffer;
            szBuffer = new ULONG[dwSize *= 2];

        }
        else
        {
            if (!NT_SUCCESS(ntStatus))
            {
                delete[] szBuffer;
                return NULL;
            }
        }
    } while (ntStatus == STATUS_INFO_LENGTH_MISMATCH);

    //�ɹ�������Ϣ
    return szBuffer;

}
