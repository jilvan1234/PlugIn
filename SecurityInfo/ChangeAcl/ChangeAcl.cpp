// ChangeAcl.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

/*
修改文件或者对象的ACL属性.
1.使用GetSecurityInfo 或者 GetNamedSecurityinfo 函数.通过句柄返回这个句柄的DACL 或者 SACL
2.对于新的ACE. 使用 BuildExplicitAccessWithName 初始化填充EXPLICIT_ACCESS 结构
3.调用SetEntriesInAcl 为新的ACE 指定现有的ACL和 EXPLICIT_ACCESS结构数组.

4.调用SetSecurityinfo进行设置.

如果获取安全描述符中的控制信息(令牌) 调用GetSecurityDescriptorControl函数.
如果设置相关的安全描述符信息.使用SetSecurityDescriptorControl函数.
*/
#include "pch.h"
#include <iostream>
#include <windows.h>
#include <AclAPI.h>
#define _WIN32_WINNT 0x0500
#pragma comment(lib,"Advapi32.lib")


int main()
{
    HANDLE hFile = INVALID_HANDLE_VALUE;

    SECURITY_INFORMATION SecurityInfo = DACL_SECURITY_INFORMATION;   //获取对象的DACL属性
    PSID pSidOwner = nullptr;
    PSID pSidGroup = nullptr;
    PACL pDacl = nullptr;
    PACL pSacl = nullptr;
    PSECURITY_DESCRIPTOR  pSecurityDescrIptor;
    EXPLICIT_ACCESS ea; //权限结构体

    hFile = CreateFile(TEXT("D:\\1234.txt"), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);

    GetSecurityInfo(hFile,
        SE_FILE_OBJECT,
        SecurityInfo,
        &pSidOwner,
        &pSidGroup,
        &pDacl,
        &pSacl,
        &pSecurityDescrIptor);

    
    GetNamedSecurityInfo(
        TEXT("D:\\1234.TXT"),
        SE_FILE_OBJECT,
        SecurityInfo,
        &pSidOwner,
        &pSidGroup,
        &pDacl,
        &pSacl,
        &pSecurityDescrIptor);

    PSID pUserSid = LocalAlloc(LMEM_ZEROINIT,sizeof(SID));

  /*  AllocateAndInitializeSid(
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, &pUserSid);
*/
   
    TCHAR *lpSystemName = nullptr;
    TCHAR *lpAccountName =(TCHAR*)TEXT("Administractor");
    SID  sid;
    DWORD cbSid = sizeof(SID);
    TCHAR *lpReferencedDomainName = nullptr;
    DWORD cchReferencedDomainName = 0;
    SID_NAME_USE peUser = SidTypeUser;

    LookupAccountName(
        NULL,
        NULL, 
        &sid,
        &cbSid, 
        lpReferencedDomainName, 
        &cchReferencedDomainName, 
        &peUser);
    lpReferencedDomainName = (TCHAR*)LocalAlloc(LMEM_ZEROINIT, cchReferencedDomainName);

    LookupAccountName(
        NULL,
        NULL,
        &sid,
        &cbSid,
        lpReferencedDomainName,
        &cchReferencedDomainName,
        &peUser);

    ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));

    ea.grfAccessPermissions = GENERIC_ALL; //所有访问权限.
    ea.grfAccessMode = GRANT_ACCESS; //设置访问权限
    ea.grfInheritance = INHERIT_ONLY; //设置继承.
    ea.Trustee.ptstrName = (LPWCH)TEXT("Administractors");  //设置用户组.
    BuildExplicitAccessWithName(&ea, lpReferencedDomainName, GENERIC_ALL, GRANT_ACCESS, INHERIT_ONLY);

    PACL NewDacl;
    SetEntriesInAcl(1, &ea, pDacl, &pDacl);

    //SetSecurityInfo(hFile, SE_FILE_OBJECT, SecurityInfo, pSidOwner, pSidGroup, NewDacl, pSacl);
    SetNamedSecurityInfo(
        (LPWSTR)TEXT("D:\\1234.txt"),
        SE_FILE_OBJECT,
        SecurityInfo, 
        pSidOwner,
        pSidGroup,
        pDacl,
        pSacl);

    int a = 10;
}

