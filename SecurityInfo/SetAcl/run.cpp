

/*
����ACL��Ϣ

*/

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "../../publicstruct.h"

#include <windows.h>
#include <AclAPI.h>
#define _WIN32_WINNT 0x0500

#include <sddl.h>
#include <stdio.h>

#pragma comment(lib, "advapi32.lib")
#include "../../ClassManger/CNativeApi/CNativeApiManger.h"
#include "../../ProcessManger/ProcessIterator/CProcessOpt.h"
#include "../../ThreadManger/RemoteThread/CRemoteThread.h"
#include "../../ThreadManger/ShellCode/ShellCode/CShellCode.h"
#include "../../FileManger/CFileManger/CFileManger.h"



#include "../../ProcessManger/ProcessIterator/CProcessOpt.h"




#ifdef _WIN64
#pragma comment(lib,"../../../PulicLib/x64/Release/CNativeApi.lib")
#pragma comment(lib,"../../../PulicLib/x64/Release/ProcessIterator.lib")
#pragma comment(lib,"../../../PulicLib/x64/Release/RegManger.lib")
#pragma comment(lib,"../../../PulicLib/x64/Release/CFileManger.lib")
#pragma comment(lib,"../../../PulicLib/x64/Release/RemoteThread.lib")
#else
#ifdef _DEBUG
#pragma comment(lib,"../../PulicLib/Win32/Debug/CNativeApi.lib")
#pragma comment(lib,"../../PulicLib/Win32/Debug/ProcessIterator.lib")
#pragma comment(lib,"../../PulicLib/Win32/Debug/RegManger.lib")
#pragma comment(lib,"../../PulicLib/Win32/Debug/CFileManger.lib")
#pragma comment(lib,"../../PulicLib/Win32/Debug/RemoteThread.lib")
#else
#pragma comment(lib,"../../PulicLib/Win32/Release/CNativeApi.lib")
#pragma comment(lib,"../../PulicLib/Win32/Release/ProcessIterator.lib")
#pragma comment(lib,"../../PulicLib/Win32/Release/RegManger.lib")
#pragma comment(lib,"../../PulicLib/Win32/Release/CFileManger.lib")
#pragma comment(lib,"../../PulicLib/Win32/Release/RemoteThread.lib")
#endif

#endif


#include <windows.h>
#include <AclAPI.h>
#define _WIN32_WINNT 0x0500
#pragma comment(lib,"Advapi32.lib")



BOOL CreateMyDACL(SECURITY_ATTRIBUTES *);

void main()
{
    SECURITY_ATTRIBUTES  sa;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = FALSE;

    // Call function to set the DACL. The DACL
    // is set in the SECURITY_ATTRIBUTES 
    // lpSecurityDescriptor member.
    if (!CreateMyDACL(&sa))
    {
        // Error encountered; generate message and exit.
        printf("Failed CreateMyDACL\n");
        exit(1);
    }

    // Use the updated SECURITY_ATTRIBUTES to specify
    // security attributes for securable objects.
    // This example uses security attributes during
    // creation of a new directory.
   
    HANDLE hFile = CreateFile(TEXT("D:\\1234.txt"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, &sa, CREATE_NEW, NULL, NULL);
    // Free the memory allocated for the SECURITY_DESCRIPTOR.
    if (NULL != LocalFree(sa.lpSecurityDescriptor))
    {
        // Error encountered; generate message and exit.
        printf("Failed LocalFree\n");
        exit(1);
    }
}


BOOL CreateMyDACL(SECURITY_ATTRIBUTES * pSA)
{
 
    TCHAR * szSD =(TCHAR*) TEXT("D:(A;OICI;GR;;;BA)");    
                                     
    /*
    ����ACE�ַ���: 
    (D;OICI;GRGW;;;BG)       
    D ����ܾ���������.
    OICI: ���� ����̳�  ���������̳�
    GR:  ����ֻ��Ȩ��. Ҳ���� rights GA������Ȩ�� GW��д GX��ִ��
    BG: ��һ��SID�ַ���. ������� ���ÿ����û�,��Ӧ�� BU�������û� BA�����ù���Ա
    */
    if (NULL == pSA)
        return FALSE;

    return ConvertStringSecurityDescriptorToSecurityDescriptor(
        szSD,
        SDDL_REVISION_1,
        &(pSA->lpSecurityDescriptor),
        NULL);
}
