#pragma once
#include <Windows.h>

/*
��������: �ص�����
��������: ���Ʒ���״̬
����1: ����Ŀ��ƴ���.���紫��0x3 SERVICE_CONTROL_CONTINUE ��ʾ֪ͨ��ͣ�ķ���Ӧ�ûָ�
����2: �ѷ�����ʱ�������.
����3: ������������豸��Ϣ. ����ȡ���ڲ���1������2��ֵ.
����4: �û��Զ��������.ͨ�� RegisterServiceCtrlHandleEx��Ϣ .��������ʱ��, lpContext���԰���ʶ�����.
����ֵ: ����ֵȡ�����յ��Ŀ��ƴ���.
*/
DWORD WINAPI ServiceHandle(DWORD    dwControl, DWORD    dwEventType, LPVOID   lpEventData, LPVOID   lpContext);

/*
��������: �ص�����.��SCM�������������
��������: �������ڵ�
����1:
����2:
����ֵ:
*/
VOID WINAPI SerMain(DWORD   dwNumServicesArgs, LPWSTR* lpServiceArgVectors);