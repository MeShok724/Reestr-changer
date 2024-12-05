#include "framework.h"
#include "Resource.h"
#include <CommCtrl.h>

#include "procTable.h"
#include "procChooseControls.h"
#include "Defines.h"
#include "GlobalVariables.h"

HWND hWndChooseProg;
HWND lvProc;

// Function declarations
LRESULT CALLBACK WndProcChose(HWND hProcChoseProg, UINT message, WPARAM wParam, LPARAM lParam);
void CreateProcCooseControls(HWND hWnd);

// Регистрация окна выбора процесса
ATOM RegisterProgChooseClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProcChose;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_REESTRCHANGER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szProgWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// Создание окна со списком приложений
HWND CreateProcChooseWindow(HWND hwnd) {
    HWND hwndNew = CreateWindow(szProgWindowClass, szProcChooseTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
        hwnd, NULL, NULL, NULL);

    if (hwndNew == NULL) {
        return NULL; // TODO: обработка ошибки создания окна
    }

    hWndChooseProg = hwndNew;
    CreateProcCooseControls(hWndChooseProg);
    ShowWindow(hwndNew, SW_SHOW);
    return hwndNew;
}

void CreateProcCooseControls(HWND hWnd) {
    lvProc = CreateProcTable(hWnd);
    CreateWindow(L"BUTTON", L"Выбрать", WS_VISIBLE | WS_CHILD, 20, 380, 70, 30, hWnd, (HMENU)CMD_CHOSEPROG, NULL, NULL);
}

// Обработка сообщений окна выбора приложения
LRESULT CALLBACK WndProcChose(HWND hProcChoseProg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case CMD_CHOSEPROG:
            if (ReadChosenName(hProcChoseProg, lvProc, selectedWinName))
                SendMessage(hProcChoseProg, WM_CLOSE, NULL, NULL);
            break;
        default:
            return DefWindowProc(hProcChoseProg, message, wParam, lParam);
        }
    }
    break;
    case WM_NOTIFY:
    {
        LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
        if (lpnmitem->hdr.hwndFrom == lvProc && lpnmitem->hdr.code == NM_DBLCLK)
            SendMessage(hProcChoseProg, WM_COMMAND, CMD_CHOSEPROG, NULL);
    }
    break;
    case WM_CLOSE:
    {
        SendMessage(hWndMainWindow, WM_COMMAND, CMD_CHECKPROGNAME, NULL);
        SendMessage(hProcChoseProg, WM_DESTROY, NULL, NULL);
    }
    break;
    case WM_DESTROY: {
        DestroyWindow(hProcChoseProg);
        UpdateWindow(hWndMainWindow);
    }
                   break;
    default:
        return DefWindowProc(hProcChoseProg, message, wParam, lParam);
    }
    return 0;
}