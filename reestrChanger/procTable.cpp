#include "framework.h"
#include "Resource.h"
#include <CommCtrl.h>
#include <TlHelp32.h>

// Function declarations
void AddData(HWND hWnd);

HWND CreateProcTable(HWND hWnd) {
    HWND lvProc = CreateWindow(WC_LISTVIEW, L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER,
        10, 10, 460, 350,
        hWnd, NULL, NULL, NULL);

    LVCOLUMN lvCol;
    lvCol.mask = LVCF_TEXT | LVCF_WIDTH;
    lvCol.pszText = (LPWSTR)L"Ќазвание окна";
    lvCol.cx = 400;
    ListView_InsertColumn(lvProc, 0, &lvCol);

    AddData(lvProc);

    return lvProc;
}

void AddData(HWND hWnd) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnapshot, &pe)) {
        do {
            LVITEM lvItem = { 0 };
            lvItem.mask = LVIF_TEXT;
            lvItem.pszText = pe.szExeFile;
            lvItem.iItem = ListView_GetItemCount(hWnd); // следующий индекс
            ListView_InsertItem(hWnd, &lvItem); // добавл€ем строку
        } while (Process32Next(hSnapshot, &pe));
    }
}