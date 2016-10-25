#include "CCApplication.h"
#include "CCEGLView.h"
#include "CCDirector.h"
#include <algorithm>
/**
@brief    This function change the PVRFrame show/hide setting in register.
@param  bEnable If true show the PVRFrame window, otherwise hide.
*/
static void PVRFrameEnableControlWindow(bool bEnable);

NS_CC_BEGIN

// sharedApplication pointer
CCApplication * CCApplication::sm_pSharedApplication = 0;

CCApplication::CCApplication()
: m_hInstance(NULL)
, m_hAccelTable(NULL)
{
    m_hInstance    = GetModuleHandle(NULL);
    m_nAnimationInterval.QuadPart = 0;
    CC_ASSERT(! sm_pSharedApplication);
    sm_pSharedApplication = this;
}

CCApplication::~CCApplication()
{
    CC_ASSERT(this == sm_pSharedApplication);
    sm_pSharedApplication = NULL;
}

int CCApplication::run()
{
    PVRFrameEnableControlWindow(false);

    // Main message loop:
    MSG msg;
    LARGE_INTEGER nFreq;
    LARGE_INTEGER nStart;
    LARGE_INTEGER nEnd;
    LONGLONG deltaQuadPart;
    DOUBLE sleepTime = 0;

    QueryPerformanceFrequency(&nFreq);

    // Initialize instance and cocos2d.
    if (!applicationDidFinishLaunching())
    {
        return 0;
    }

    CCEGLView* pMainWnd = CCEGLView::sharedOpenGLView();
    ShowWindow(pMainWnd->getHWnd(), SW_SHOW);

    while (1)
    {
        if (! PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            QueryPerformanceCounter(&nStart);
            CCDirector::sharedDirector()->mainLoop();
            QueryPerformanceCounter(&nEnd);

            deltaQuadPart = m_nAnimationInterval.QuadPart - (nEnd.QuadPart - nStart.QuadPart);
            if (deltaQuadPart > 0)
            {
                sleepTime += 1000.0 * deltaQuadPart / nFreq.QuadPart;
                Sleep((int)sleepTime);
                sleepTime -= (int)sleepTime;
            }
            continue;
        }

        if (WM_QUIT == msg.message)
        {
            // Quit message loop.
            break;
        }

        // Deal with windows message.
        if (! m_hAccelTable || ! TranslateAccelerator(msg.hwnd, m_hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

void CCApplication::setAnimationInterval(double interval)
{
    LARGE_INTEGER nFreq;
    QueryPerformanceFrequency(&nFreq);
    m_nAnimationInterval.QuadPart = (LONGLONG)(interval * nFreq.QuadPart);
}

//////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////
CCApplication* CCApplication::sharedApplication()
{
    CC_ASSERT(sm_pSharedApplication);
    return sm_pSharedApplication;
}

NS_CC_END

//////////////////////////////////////////////////////////////////////////
// Local function
//////////////////////////////////////////////////////////////////////////
static void PVRFrameEnableControlWindow(bool bEnable)
{
    HKEY hKey = 0;

    // Open PVRFrame control key, if not exist create it.
    if(ERROR_SUCCESS != RegCreateKeyExW(HKEY_CURRENT_USER,
        L"Software\\Imagination Technologies\\PVRVFRame\\STARTUP\\",
        0,
        0,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,
        0,
        &hKey,
        NULL))
    {
        return;
    }

    const WCHAR* wszValue = L"hide_gui";
    const WCHAR* wszNewData = (bEnable) ? L"NO" : L"YES";
    WCHAR wszOldData[256] = {0};
    DWORD   dwSize = sizeof(wszOldData);
    LSTATUS status = RegQueryValueExW(hKey, wszValue, 0, NULL, (LPBYTE)wszOldData, &dwSize);
    if (ERROR_FILE_NOT_FOUND == status              // the key not exist
        || (ERROR_SUCCESS == status                 // or the hide_gui value is exist
        && 0 != wcscmp(wszNewData, wszOldData)))    // but new data and old data not equal
    {
        dwSize = sizeof(WCHAR) * (wcslen(wszNewData) + 1);
        RegSetValueEx(hKey, wszValue, 0, REG_SZ, (const BYTE *)wszNewData, dwSize);
    }

    RegCloseKey(hKey);
}