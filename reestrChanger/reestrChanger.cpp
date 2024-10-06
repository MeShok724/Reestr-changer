// reestrChanger.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "reestrChanger.h"
#include <commctrl.h>


#define MAX_LOADSTRING 100
#define CMD_CHOSEPROG 1
#define CMD_CREATERUL 2

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND hWnd;
HWND inp_path, inp_val, choosen_prog;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[] = L"MyClass1";            // the main window class name
WCHAR szProgWindowClass[] = L"MyClass2";            // the class name of window with programs

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL                InitControls(HWND hwnd);
void                OnButtonClicked(HWND hwnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
        // LoadStringW(hInstance, IDC_REESTRCHANGER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }
    InitControls(hWnd);

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



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_REESTRCHANGER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_REESTRCHANGER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    wcex.lpszClassName = szProgWindowClass;
    wcex.lpfnWndProc = WndProgProc;
    wcex.lpszMenuName = NULL;
    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}

BOOL InitControls(HWND hwnd) {
    CreateWindow(L"STATIC", L"������� ���� � ����� �������:", WS_CHILD | WS_VISIBLE, 10, 20, 300, 20, hwnd, NULL, NULL, NULL);
    inp_path = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 40, 300, 20, hwnd, NULL, NULL, NULL);
    CreateWindow(L"STATIC", L"������� ������ ��������:", WS_CHILD | WS_VISIBLE, 10, 70, 300, 20, hwnd, NULL, NULL, NULL);
    inp_val = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 90, 300, 20, hwnd, NULL, NULL, NULL);
    CreateWindow(L"STATIC", L"��������� ����������:", WS_CHILD | WS_VISIBLE, 10, 120, 300, 20, hwnd, NULL, NULL, NULL);
    choosen_prog = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 140, 300, 20, hwnd, NULL, NULL, NULL);
    CreateWindow(L"BUTTON", L"������� ����������", WS_VISIBLE | WS_CHILD, 10, 170, 300, 30, hwnd, (HMENU)CMD_CHOSEPROG, NULL, NULL);
    CreateWindow(L"BUTTON", L"������� �������", WS_VISIBLE | WS_CHILD, 10, 210, 300, 30, hwnd, (HMENU)CMD_CREATERUL, NULL, NULL);
    return true;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case CMD_CHOSEPROG:
                OnButtonClicked(hWnd);
                break;
            case CMD_CREATERUL:
                // TODO: add a creating a rule
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// Callback-������� ��� ������������ ����
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    // �������� ��������� ����
    wchar_t windowTitle[256];
    GetWindowText(hwnd, windowTitle, sizeof(windowTitle) / sizeof(wchar_t));

    // ���������, ���� �� ��������� � ���� (���� ����� � ����������)
    if (wcslen(windowTitle) > 0) {  //  && wcscmp(windowTitle, L"Default IME") != 0 && wcscmp(windowTitle, L"") != 0
        // ��������� ��������� � ������ (ListView)
        HWND listView = (HWND)lParam;
        LVITEM lvItem = { 0 };
        lvItem.mask = LVIF_TEXT;
        lvItem.pszText = windowTitle;
        lvItem.iItem = ListView_GetItemCount(listView); // ��������� ������
        ListView_InsertItem(listView, &lvItem); // ��������� ������
    }
    return TRUE; // ���������� ������������ ����
}

// ���������� ������� ������
void OnButtonClicked(HWND hwnd) {
    // ������� ����� ���� ��� ����������� ������
    HWND hwndNew = CreateWindow(szProgWindowClass, L"������ ����������",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
        hwnd, NULL, NULL, NULL);
    if (hwndNew == NULL)
        MessageBox(NULL, L"�� ������� ������� ����", L"������", MB_OK | MB_ICONERROR);

    // ������� ListView ��� ����������� ������
    HWND listView = CreateWindow(WC_LISTVIEW, L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER,
        10, 10, 460, 350,
        hwndNew, NULL, NULL, NULL);

    // ����������� ListView ��� ����������� �������
    LVCOLUMN lvCol;
    lvCol.mask = LVCF_TEXT | LVCF_WIDTH;
    lvCol.pszText = (LPWSTR)L"�������� ����";
    lvCol.cx = 400;
    ListView_InsertColumn(listView, 0, &lvCol);

    // ����������� ��� ���������� ����
    EnumWindows(EnumWindowsProc, (LPARAM)listView);

    // ��������� ����������� ����
    ShowWindow(hwndNew, SW_SHOW);
}

LRESULT CALLBACK WndProgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_DESTROY:
        DestroyWindow(hWnd);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}