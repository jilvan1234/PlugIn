#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "../../ClassManger/CNativeApi/CNativeApiManger.h"
#include "../../publicstruct.h"
#include "../../ProcessManger/ProcessIterator/CProcessOpt.h"
#include "../../FileManger/CFileManger/CFileManger.h"


int main()
{
    /*
  �ȴ�ռ�þ�����Խ���.

  1.���ȫ�־����.��ȡFile����
  2.����NtQueryObject��ѯ��������.
  3.����������� == �Լ��ļ����ļ���
      ʹ��DumpLicateHandle ,����رվ���Ĳ������п���.
  �������йر�.
  */
  // file.GetHandleCount();
    CProcessOpt psOpt;
    PSYSTEM_PROCESS_INFORMATION pProcInfo;
    DWORD dwPid;
    dwPid = psOpt.PsGetProcessIdByProcessFileName(TEXT("lsass.exe"));
    CNativeApiManger Nat;
    Nat.NatHndCloseSpecilProcessOccFileHandle(dwPid, TEXT("xbbrowser.exe")); //�ر�ָ�������µ��ļ����.
   
      

       
    
    

    return 0;
}