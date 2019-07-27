// MyDuiLib.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MyDuiLib.h"
#include <windows.h>
#include "DuiLib\UIlib.h"
using namespace DuiLib;

#ifdef _WIN64

#ifdef _DEBUG
#pragma comment (lib, "UReAnDbLib64\\DDuiLib.lib")
#else
#pragma comment (lib, "UReAnDbLib64\\RDuiLib.lib")
#endif

#else

#ifdef _DEBUG
#pragma comment (lib, "DDuiLib.lib")
#else
#pragma comment (lib, "RDuiLib.lib")
#endif

#endif

class CFrameWindowWnd : public CWindowWnd, public INotifyUI
{
public:
    CFrameWindowWnd() {};
    LPCTSTR GetWindowClassName()const { return _T("HelloWorld"); }
    UINT   GetClassStyle()const { return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; }
    void OnFinalMessage(HWND hWnd) { delete this; }

    void Notify(TNotifyUI& msg)
    {
        if (msg.sType == _T("click"))
        {
            if (msg.pSender->GetName() == _T("closebtn"))
            {
                Close();
            }
        }
    }

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_CREATE)
        {
            m_pm.Init(m_hWnd);
            CControlUI *pButton = new CButtonUI;
            pButton->SetName(_T("closebtn"));
            pButton->SetBkColor(0xFFFF0000);
            m_pm.AttachDialog(pButton);
            m_pm.AddNotifier(this);
            return 0;
        }
        else if (uMsg == WM_DESTROY)
        {
            ::PostQuitMessage(0);
        }
        LRESULT LRes = 0;
        if (m_pm.MessageHandler(uMsg, wParam, lParam, LRes))
            return LRes;
        return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    }
public:
    CPaintManagerUI m_pm;
};


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath((CPaintManagerUI::GetInstancePath()));

    CFrameWindowWnd *pFrame = new CFrameWindowWnd();
    if (NULL == pFrame)
        return 0;
    pFrame->Create(NULL, _T("HelloWorld"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    pFrame->ShowWindow(true);
    CPaintManagerUI::MessageLoop();
    return 0;
}
