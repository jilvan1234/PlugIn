#pragma once
#include <windows.h>
#include <string>
using namespace std;
#ifdef UNICODE
#define MYISTRING wstring
#else
#define MYISTRING string
#endif // UNICODE

class ServicesManger
{
public:
    ServicesManger();
    virtual ~ServicesManger();
public:
    BOOL ScOpenManger();
    BOOL ScCreateServices(MYISTRING strServicePath,MYISTRING strServiceName, MYISTRING strShowServiceName, DWORD dwStartType);
    BOOL ScStartServices(MYISTRING ServiceName);
    BOOL ScStopServices(MYISTRING ServiceName);
    BOOL ScDeleteServices(MYISTRING ServiceName);
private:
    SC_HANDLE m_OpenManger;
    SC_HANDLE m_CreatService;
    SC_HANDLE m_ScOpenService;

};

