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
  先打开占用句柄测试进程.

  1.获得全局句柄表.获取File类型
  2.利用NtQueryObject查询对象名字.
  3.如果对象名字 == 自己文件的文件名
      使用DumpLicateHandle ,传入关闭句柄的参数进行拷贝.
  进而进行关闭.
  */
  // file.GetHandleCount();
    CProcessOpt psOpt;
    PSYSTEM_PROCESS_INFORMATION pProcInfo;
    DWORD dwPid;
    dwPid = psOpt.PsGetProcessIdByProcessFileName(TEXT("lsass.exe"));
    CNativeApiManger Nat;
    Nat.NatHndCloseSpecilProcessOccFileHandle(dwPid, TEXT("xbbrowser.exe")); //关闭指定进程下的文件句柄.
   
      

       
    
    

    return 0;
}