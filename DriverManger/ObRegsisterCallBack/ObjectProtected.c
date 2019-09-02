#include "ObjectProtected.h"

PVOID g_ObjHandle;
POPT_PRO g_OptPro = NULL;

//

OB_PREOP_CALLBACK_STATUS MyObjectCallBack(
    PVOID RegistrationContext,
    POB_PRE_OPERATION_INFORMATION OperationInformation
)
{
    PEPROCESS pProcess = NULL;
    UCHAR *pszName = NULL;
    POPT_PRO pOptPro = (POPT_PRO)RegistrationContext;
    pProcess = (PEPROCESS)OperationInformation->Object;
    pszName = PsGetProcessImageFileName(pProcess);

    //判断标志是否要打开进程
  /*  if (pOptPro->pszName == NULL)
    {
        return OB_PREOP_SUCCESS;
    }*/
  
        //获取名字匹配
        if (strstr((const char *)pszName, "calc"))
        {


            //DbgPrint((" %s \r\n", pszName));
           
            //判断是不是结束
            if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess
                & PROCESS_TERMINATE) == 1)
            {
                //如果是我们的.则设置DesiredAccess权限.去掉结束权限.
                    OperationInformation->Parameters->CreateHandleInformation.DesiredAccess
                        = ~PROCESS_TERMINATE;
                    return STATUS_ACCESS_DENIED;
            }
            return STATUS_SUCCESS;
        }
    
    return STATUS_SUCCESS;
}

NTSTATUS InstallObjectProtecteHOOk()
{
  
    NTSTATUS ntStatus;
    OB_OPERATION_REGISTRATION oper; //你的回调函数以及类型都放在这个结构体中,可以是结构体数组.
    OB_CALLBACK_REGISTRATION CallbackRegistration;
    CallbackRegistration.Version = OB_FLT_REGISTRATION_VERSION; //版本号
    CallbackRegistration.OperationRegistrationCount = 1;//下几个钩子,也就是结构体数组个数
    RtlUnicodeStringInit(&CallbackRegistration.Altitude, L"600000");//给个UNICODEstring表明您是干啥的
    
    CallbackRegistration.RegistrationContext = (PVOID)g_OptPro;         //当你的回调函数到的时候参数是什么.由这里给出
    CallbackRegistration.OperationRegistration = &oper; //钩子结构体类型设置.
    //为钩子结构体赋值
    oper.ObjectType = PsProcessType; //进程操作的类型.当进程操作回来. PsThreadType则是线程操作
    oper.Operations = OB_OPERATION_HANDLE_CREATE; //操作的类型是将要打开.以及将要重复
    oper.PreOperation = MyObjectCallBack; //有两个指针,前指针跟后指针.意思分别是 进程创建之前通知你还是之后
    oper.PostOperation = NULL;
    ntStatus = ObRegisterCallbacks(&CallbackRegistration, &g_ObjHandle);
    return ntStatus;
}

NTSTATUS UnstallObjectProtecteHook()
{
    ObUnRegisterCallbacks(g_ObjHandle);
    return STATUS_SUCCESS;
}



VOID SetProTerminate(BOOLEAN IsSet)
{
    if (IsSet)
        g_OptPro->Pro_Terminate = TRUE;
    g_OptPro->Pro_Terminate = FALSE;
}

VOID SetProCreateThread(BOOLEAN IsSet)
{
    if (IsSet)
        g_OptPro->Pro_CreateThread = TRUE;
    g_OptPro->Pro_CreateThread = FALSE;
}


VOID SetProVmOperation(BOOLEAN IsSet)
{
    if (IsSet)
        g_OptPro->Pro_VmOperation = TRUE;
    g_OptPro->Pro_VmOperation = FALSE;
}

VOID SetProSessionId(BOOLEAN IsSet)
{
    if (IsSet)
        g_OptPro->Pro_SetSessionId = TRUE;
    g_OptPro->Pro_SetSessionId = FALSE;
}

VOID SetProVmRead(BOOLEAN IsSet)
{
    if (IsSet)
        g_OptPro->Pro_VmRead = TRUE;
    g_OptPro->Pro_VmRead = FALSE;
}

VOID SetProVmWrite(BOOLEAN IsSet)
{
    if (IsSet)
        g_OptPro->Pro_VmWrite = TRUE;
    g_OptPro->Pro_VmWrite = FALSE;
}

VOID SetProDupHandle(BOOLEAN IsSet)
{
    if (IsSet)
        g_OptPro->Pro_DupHandle = TRUE;
    g_OptPro->Pro_DupHandle = FALSE;
}

VOID SetProCreateProcess(BOOLEAN IsSet)
{
    if (IsSet)
        g_OptPro->Pro_CreateProcess = TRUE;
    g_OptPro->Pro_CreateProcess = FALSE;
}

VOID SetProSetQuota(BOOLEAN IsSet)
{
    if (IsSet)
        g_OptPro->Pro_SetQuota = TRUE;
    g_OptPro->Pro_SetQuota = FALSE;
}

VOID SetProSetInformation(BOOLEAN IsSet)
{
    if (IsSet)
        g_OptPro->Pro_SetInformation = TRUE;
    g_OptPro->Pro_SetInformation = FALSE;
}
VOID SetProQueryinformation(BOOLEAN IsSet)
{
    if (IsSet)
        g_OptPro->Pro_QueryInformation = TRUE;
    g_OptPro->Pro_QueryInformation = FALSE;
}

VOID SetProSetPort(BOOLEAN IsSet)
{
    if (IsSet)
        g_OptPro->Pro_SetPort = TRUE;
    g_OptPro->Pro_SetPort = FALSE;
}

VOID SetProSuspend_Resume(BOOLEAN IsSet)
{
    if (IsSet)
        g_OptPro->Pro_Suspend_Resume = TRUE;
    g_OptPro->Pro_Suspend_Resume = FALSE;
}

VOID SetProImageName(const char *pszName)
{
    if (pszName != NULL)
        g_OptPro->pszName = pszName;
    g_OptPro->pszName = NULL;
}

NTSTATUS AllocateOptMem()
{
    g_OptPro = ExAllocatePoolWithTag(NonPagedPool,sizeof(OPT_PRO),'1111');
    if (NULL == g_OptPro)
        return STATUS_UNSUCCESSFUL;
    RtlZeroMemory(g_OptPro, sizeof(OPT_PRO));
    return STATUS_SUCCESS;
}
NTSTATUS UnAllocateOptMem()
{
    if (NULL != g_OptPro)
    {
        ExFreePool(g_OptPro);
        return STATUS_SUCCESS;
    }
    return STATUS_UNSUCCESSFUL;
}