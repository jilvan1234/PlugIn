// EnumSysHandleTableTools.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CEnumSysHandleTableToolsApp:
// �йش����ʵ�֣������ EnumSysHandleTableTools.cpp
//

class CEnumSysHandleTableToolsApp : public CWinApp
{
public:
	CEnumSysHandleTableToolsApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	
};

extern CEnumSysHandleTableToolsApp theApp;