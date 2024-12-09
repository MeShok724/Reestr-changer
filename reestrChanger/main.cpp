#include "framework.h"

#include "windowsLogic.h"
#include "procChooseWindowLogic.h"
#include "GlobalVariables.h"

#include <commctrl.h>
#include <vector>
#include <string>
#include <atomic>
#include <tlhelp32.h>
#include <mutex>
#include "Resource.h"


#define MAX_LOADSTRING 100

// Global Variables:                              
WCHAR szTitle[MAX_LOADSTRING];                  
      

// Главная функция
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    RegisterMainClass(hInstance);
    RegisterProgChooseClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow, &hWndMainWindow, &hInst))
    {
        return FALSE;
    }

    InitControls(hWndMainWindow);
    UpdateWindow(hWndMainWindow);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_REESTRCHANGER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}