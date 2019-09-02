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

    //�жϱ�־�Ƿ�Ҫ�򿪽���
  /*  if (pOptPro->pszName == NULL)
    {
        return OB_PREOP_SUCCESS;
    }*/
  
        //��ȡ����ƥ��
        if (strstr((const char *)pszName, "calc"))
        {


            //DbgPrint((" %s \r\n", pszName));
           
            //�ж��ǲ��ǽ���
            if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess
                & PROCESS_TERMINATE) == 1)
            {
                //��������ǵ�.������DesiredAccessȨ��.ȥ������Ȩ��.
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
    OB_OPERATION_REGISTRATION oper; //��Ļص������Լ����Ͷ���������ṹ����,�����ǽṹ������.
    OB_CALLBACK_REGISTRATION CallbackRegistration;
    CallbackRegistration.Version = OB_FLT_REGISTRATION_VERSION; //�汾��
    CallbackRegistration.OperationRegistrationCount = 1;//�¼�������,Ҳ���ǽṹ���������
    RtlUnicodeStringInit(&CallbackRegistration.Altitude, L"600000");//����UNICODEstring�������Ǹ�ɶ��
    
    CallbackRegistration.RegistrationContext = (PVOID)g_OptPro;         //����Ļص���������ʱ�������ʲô.���������
    CallbackRegistration.OperationRegistration = &oper; //���ӽṹ����������.
    //Ϊ���ӽṹ�帳ֵ
    oper.ObjectType = PsProcessType; //���̲���������.�����̲�������. PsThreadType�����̲߳���
    oper.Operations = OB_OPERATION_HANDLE_CREATE; //�����������ǽ�Ҫ��.�Լ���Ҫ�ظ�
    oper.PreOperation = MyObjectCallBack; //������ָ��,ǰָ�����ָ��.��˼�ֱ��� ���̴���֮ǰ֪ͨ�㻹��֮��
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