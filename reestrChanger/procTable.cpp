#include "framework.h"
#include "Resource.h"
#include <CommCtrl.h>
#include <TlHelp32.h>
#include <set>
#include <string>

// Function declarations
void AddData(HWND hWnd);

HWND CreateProcTable(HWND hWnd) {
    HWND lvProc = CreateWindow(WC_LISTVIEW, L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER,
        10, 10, 460, 350,
        hWnd, NULL, NULL, NULL);

    LVCOLUMN lvCol;
    lvCol.mask = LVCF_TEXT | LVCF_WIDTH;
    lvCol.pszText = (LPWSTR)L"Название окна";
    lvCol.cx = 400;
    ListView_InsertColumn(lvProc, 0, &lvCol);

    AddData(lvProc);

    return lvProc;
}

void AddData(HWND hWnd) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot == INVALID_HANDLE_VALUE) {
        MessageBox(hWnd, L"Не удалось создать снимок процессов.", L"Ошибка", MB_OK | MB_ICONERROR);
        return;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    std::set<std::wstring> processNames; // Используем std::set для уникальности и сортировки

    if (Process32First(hSnapshot, &pe)) {
        do {
            processNames.insert(pe.szExeFile); // Автоматически удаляет дубликаты и сортирует
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);

    // Очищаем ListView перед добавлением новых данных
    ListView_DeleteAllItems(hWnd);

    // Добавляем уникальные и отсортированные имена процессов в ListView
    int index = 0;
    for (const auto& processName : processNames) {
        LVITEM lvItem = { 0 };
        lvItem.mask = LVIF_TEXT;
        lvItem.pszText = const_cast<LPWSTR>(processName.c_str()); // Преобразуем const std::wstring в LPWSTR
        lvItem.iItem = index++; // Индекс новой строки
        ListView_InsertItem(hWnd, &lvItem);
    }

    // Обновляем окно для отображения изменений
    UpdateWindow(hWnd);
}