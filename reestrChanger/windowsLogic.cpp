#include "framework.h"
#include "Resource.h"
#include <CommCtrl.h>

#include "ruleTable.h"
#include "Defines.h"
#include "GlobalVariables.h"
#include "procChooseWindowLogic.h"
#include "mainControls.h"
#include "monitorProc.h"

HWND inpFolder, inpPath, inpValName, inpVal, choosenProg;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Регистрация главного окна приложения и окна выбора процесса
ATOM RegisterMainClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    hInst = hInstance;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_REESTRCHANGER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_REESTRCHANGER);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));


    return RegisterClassExW(&wcex);
}

// Создание экземпляра окна и сохранение в глобальную переменную hWnd
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND *hWnd, HINSTANCE *hInst)
{
    *hInst = hInstance; // Store instance handle in our global variable

    *hWnd = CreateWindowW(szWindowClass, szMainWindowTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(*hWnd, nCmdShow);
    UpdateWindow(*hWnd);
    return TRUE;
}

// Инициализация компонентов главного окна
BOOL InitControls(HWND hwnd) {
    CreateWindow(L"STATIC", L"Выберите раздел реестра:", WS_CHILD | WS_VISIBLE, 10, 20, 300, 20, hwnd, NULL, NULL, NULL);
    inpFolder = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, 10, 40, 300, 500, hwnd, NULL, NULL, NULL);
    CreateWindow(L"STATIC", L"Введите путь к ключу реестра:", WS_CHILD | WS_VISIBLE, 10, 70, 300, 20, hwnd, NULL, NULL, NULL);
    inpPath = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 90, 300, 20, hwnd, NULL, NULL, NULL);
    CreateWindow(L"STATIC", L"Введите название величины:", WS_CHILD | WS_VISIBLE, 10, 120, 300, 20, hwnd, NULL, NULL, NULL);
    inpValName = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 140, 300, 20, hwnd, NULL, NULL, NULL);
    CreateWindow(L"STATIC", L"Введите нужное значение:", WS_CHILD | WS_VISIBLE, 10, 170, 300, 20, hwnd, NULL, NULL, NULL);
    inpVal = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 190, 300, 20, hwnd, NULL, NULL, NULL);
    CreateWindow(L"STATIC", L"Выбранное приложение:", WS_CHILD | WS_VISIBLE, 10, 220, 300, 20, hwnd, NULL, NULL, NULL);
    choosenProg = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 240, 300, 20, hwnd, NULL, NULL, NULL);
    CreateWindow(L"BUTTON", L"Выбрать приложение", WS_VISIBLE | WS_CHILD, 10, 270, 300, 30, hwnd, (HMENU)CMD_CHOSEPROG, NULL, NULL);
    CreateWindow(L"BUTTON", L"Создать правило", WS_VISIBLE | WS_CHILD, 10, 310, 300, 30, hwnd, (HMENU)CMD_CREATERUL, NULL, NULL);
    CreateWindow(L"BUTTON", L"Начать отслеживание", WS_VISIBLE | WS_CHILD, 10, 500, 300, 30, hwnd, (HMENU)CMD_STARTTRACKING, NULL, NULL);

    CreateWindow(L"STATIC", L"Список правил:", WS_CHILD | WS_VISIBLE, 400, 20, 150, 20, hwnd, NULL, NULL, NULL);
    
    lvRules = CreateLvRules(hwnd);

    SendMessage(inpFolder, CB_ADDSTRING, 0, (LPARAM)L"HKEY_CURRENT_USER");
    SendMessage(inpFolder, CB_ADDSTRING, 0, (LPARAM)L"HKEY_LOCAL_MACHINE");
    SendMessage(inpFolder, CB_ADDSTRING, 0, (LPARAM)L"HKEY_CLASSES_ROOT");
    SendMessage(inpFolder, CB_ADDSTRING, 0, (LPARAM)L"HKEY_USERS");
    SendMessage(inpFolder, CB_ADDSTRING, 0, (LPARAM)L"HKEY_CURRENT_CONFIG");
    SendMessage(inpFolder, CB_SETCURSEL, 0, 0);

    return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case CMD_CHOSEPROG:
            CreateProcChooseWindow(hWnd);
            break;
        case CMD_CREATERUL:
            if (!CreateRule(hWnd, choosenProg, inpFolder, inpPath, inpVal, inpValName)) 
            { //TODO: Обработать ошибку создания правила    
            }      
            break;
        case CMD_CHECKPROGNAME:
            CheckWinName(selectedWinName, choosenProg);
            break;
        case CMD_STARTTRACKING:
            ThrStartMonitoring();
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
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}