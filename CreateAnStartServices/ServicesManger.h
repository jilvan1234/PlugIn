#pragma once
#include <windows.h>
#include <string>
using namespace std;
#ifdef UNICODE
#define STRING wstring
#else
#define STRING string
#endif // UNICODE

class ServicesManger
{
public:
    ServicesManger();
    virtual ~ServicesManger();
public:
    BOOL ScOpenManger();
    BOOL ScCreateServices(STRING strServicePath,STRING strServiceName, STRING strShowServiceName, DWORD dwStartType);
    BOOL ScStartServices(STRING ServiceName);
    BOOL ScStopServices(STRING ServiceName);
    BOOL ScDeleteServices(STRING ServiceName);
private:
    SC_HANDLE m_OpenManger;
    SC_HANDLE m_CreatService;
    SC_HANDLE m_ScOpenService;

};

