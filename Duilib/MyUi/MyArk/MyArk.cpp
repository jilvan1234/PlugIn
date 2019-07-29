#pragma once
#include "stdafx.h"
#define WIN32_LEAN_AND_MEAN    
#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#include <objbase.h>

#include "DuiLib/UIlib.h"

using namespace DuiLib;

#ifdef _DEBUG
#pragma comment (lib, "duilib_ud.lib")
#else
#pragma comment (lib, "duilib_u.lib")
#endif
// 窗口实例及消息响应部分

class CDuiFrameWnd : public WindowImplBase
{
public:
    virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
    virtual CDuiString GetSkinFile() { return _T("duilib.xml"); }
    virtual CDuiString GetSkinFolder() { return _T(""); }
};

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);

    CDuiFrameWnd duiFrame;
    duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    duiFrame.CenterWindow();
    duiFrame.ShowModal();
    return 0;
}
