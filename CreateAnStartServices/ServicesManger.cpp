#include "ServicesManger.h"



ServicesManger::ServicesManger()
{
    m_OpenManger = NULL;
    m_CreatService = NULL;
    m_ScOpenService = NULL;
}


ServicesManger::~ServicesManger()
{
    if (NULL != m_OpenManger)
        ::CloseServiceHandle(m_OpenManger);

    if (NULL != m_CreatService)
        ::CloseServiceHandle(m_CreatService);

    if (NULL != m_ScOpenService)
        ::CloseServiceHandle(m_ScOpenService);

   
}

BOOL ServicesManger::ScOpenManger()
{
    m_OpenManger = OpenSCManager(nullptr,nullptr, GENERIC_ALL);
    if (NULL == m_OpenManger)
    {
        ::CloseServiceHandle(m_OpenManger);
        return FALSE;
    }
    return TRUE;
}

BOOL ServicesManger::ScCreateServices(MYISTRING strServicePath,MYISTRING strServiceName,MYISTRING strShowServiceName,DWORD dwStartType)
{
    /*
    m_CreatService =  CreateService(
        m_OpenManger,
        strServiceName.c_str(),
        strShowServiceName.c_str(),
        SERVICE_ALL_ACCESS,
        SERVICE_USER_OWN_PROCESS,
        dwStartType,
        SERVICE_ERROR_IGNORE,
        strServicePath.c_str(),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    );
    */
    
    
    m_CreatService = CreateService(
        m_OpenManger,              // SCM database 
        strServiceName.c_str(),                   // name of service 
        strShowServiceName.c_str(),                   // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        SERVICE_DEMAND_START,      // start type 
        SERVICE_ERROR_NORMAL,      // error control type 
        strServicePath.c_str(),             // path to service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,                      // LocalSystem account 
        NULL);                     // no password 
       
    if (NULL == m_CreatService)
        return FALSE;
    return TRUE;
}

BOOL ServicesManger::ScStartServices(MYISTRING ServiceName)
{
    BOOL bRet = FALSE;
    if (NULL == m_OpenManger)
        return FALSE;
    m_ScOpenService = OpenService(m_OpenManger, ServiceName.c_str(), GENERIC_ALL);
    if (NULL == m_ScOpenService)
        return FALSE;
    bRet = StartService(m_ScOpenService, 0, NULL);
    if (FALSE == bRet)
    {
        ::CloseServiceHandle(m_ScOpenService);
        return FALSE;
    }
    ::CloseServiceHandle(m_ScOpenService);
    return TRUE;
}

BOOL ServicesManger::ScStopServices(MYISTRING ServiceName)
{
    BOOL bRet = FALSE;
    SERVICE_STATUS svcsta;
    m_ScOpenService = OpenService(m_OpenManger, ServiceName.c_str(), GENERIC_ALL);
    if (NULL == m_ScOpenService)
        return FALSE;

    bRet = ControlService(m_ScOpenService, SERVICE_CONTROL_STOP, &svcsta);
  
    if (FALSE == bRet)
    {
        ::CloseServiceHandle(m_ScOpenService);
        return FALSE;
    }
    ::CloseServiceHandle(m_ScOpenService);

    return TRUE;
}

BOOL ServicesManger::ScDeleteServices(MYISTRING ServiceName)
{
    BOOL bRet = FALSE;
    SERVICE_STATUS svcsta;
    m_ScOpenService = OpenService(m_OpenManger, ServiceName.c_str(), GENERIC_ALL);
    if (NULL == m_ScOpenService)
        return FALSE;
    bRet = DeleteService(m_ScOpenService);
    if (FALSE == bRet)
    {
        ::CloseServiceHandle(m_ScOpenService);
        return FALSE;
    }
    ::CloseServiceHandle(m_ScOpenService);

    return TRUE;
}
