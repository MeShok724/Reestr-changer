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

// Callback-функция для перечисления окон
//BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
//    // Получение заголовка окна
//    wchar_t windowTitle[256];
//    GetWindowText(hwnd, windowTitle, sizeof(windowTitle) / sizeof(wchar_t));
//
//    // Проверяем, есть ли заголовок у окна (если пусто — пропускаем)
//    if (wcslen(windowTitle) > 0) {  //  && wcscmp(windowTitle, L"Default IME") != 0 && wcscmp(windowTitle, L"") != 0
//        // Добавляем заголовок в список (ListView)
//        HWND listView = (HWND)lParam;
//        LVITEM lvItem = { 0 };
//        lvItem.mask = LVIF_TEXT;
//        lvItem.pszText = windowTitle;
//        lvItem.iItem = ListView_GetItemCount(listView); // следующий индекс
//        ListView_InsertItem(listView, &lvItem); // добавляем строку
//    }
//    return TRUE; // продолжаем перечисление окон
//}