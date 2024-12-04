#include "framework.h"

#include "windowsLogic.h"
#include "procChooseWindowLogic.h"
#include "Rule.h"

#include <commctrl.h>
#include <vector>
#include <string>
#include <atomic>
#include <tlhelp32.h>
#include <mutex>
#include "Resource.h"


#define MAX_LOADSTRING 100


// Types


// Global Variables:
HINSTANCE hInst;                                
HWND hWnd;
HWND inpFolder, inpValName, inpPath, inpVal, choosenProg;
HWND lvRules, lvProc;
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[] = L"MyClass1";            
WCHAR szProgWindowClass[] = L"MyClass2";            
std::vector<Rule> ruleVec;
WCHAR selectedWinName[256];
std::atomic<bool> stopMonitoring{ false }; 
std::mutex ruleVecMutex;
std::mutex listViewMutex;
HANDLE thrMonitoring;

// Forward declarations of functions
void CheckWinName();
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL EnumWindowsProc(HWND hwnd, LPARAM lParam);
bool ReadChosenName(HWND hwnd);
BOOL                InitControls(HWND hwnd);
void                AddColumnsToListView(HWND lv_rules);
std::wstring TypeToStr(DWORD typeCode);
void                AddRuleToListView(HWND lv_rules, Rule rule);
HKEY GetRootKey();
BOOL                CreateRule(HWND hWnd);
BOOL                GetRegistryValueAndType(Rule *rule);
void changeData(DWORD type, HKEY rootKey, wchar_t fieldName[255], wchar_t valueData[255]);
DWORD __stdcall MonitorPrograms(LPVOID lpParam);
void                RuleActivityHandler(Rule& rule, bool isNowActive, int i);
void                LvUpdateActivity(int itemIndex, bool isActive);
HANDLE              ThrStartMonitoring();
void                ThrStopMonitoring(HANDLE hThread);
bool UpdateRegistryValue(const Rule& rule, bool isNowActive);

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
    RegisterMainClass(hInstance, szWindowClass);
    RegisterProgChooseClass(hInstance, szProgWindowClass);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow, &hWnd, &hInst, szWindowClass, szProgWindowClass))
    {
        return FALSE;
    }

    InitControls(hWnd);
    UpdateWindow(hWnd);

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







// TODO: в другой модуль
// Добавление столбцов в таблицу правил


// в другой модуль



// TODO: в другой модуль
// Добавление правила в таблицу





void changeData(DWORD type, HKEY rootKey, wchar_t fieldName[255], wchar_t valueData[255]) {
    BOOL setResult;
    if (type == REG_SZ) {
        setResult = RegSetValueEx(rootKey, fieldName, 0, type, (const BYTE*)valueData, (lstrlen(valueData) + 1) * sizeof(wchar_t));
    }
    else if (type == REG_DWORD) {
        DWORD dwValue = _wtoi(valueData);
        setResult = RegSetValueEx(rootKey, fieldName, 0, type, (const BYTE*)&dwValue, sizeof(DWORD));
    }
    else if (type == REG_BINARY) {
        BYTE binaryData[255];
        for (int i = 0; i < lstrlen(valueData) && i < 255; i++) {
            binaryData[i] = (BYTE)valueData[i];
        }
        setResult = RegSetValueEx(rootKey, fieldName, 0, type, binaryData, lstrlen(valueData));
    }
}

DWORD WINAPI MonitorPrograms(LPVOID lpParam) {
  
    while (!stopMonitoring) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        std::vector<bool> currentProgramState;

        // Защищаем доступ к monitoredPrograms и создаем состояние процессов
        {
            std::lock_guard<std::mutex> lock(ruleVecMutex);
            currentProgramState.resize(ruleVec.size(), false);

            if (Process32First(hSnapshot, &pe)) {
                do {
                    for (size_t i = 0; i < ruleVec.size(); ++i) {
                        if (std::wstring(pe.szExeFile) == ruleVec[i].progName) {
                            currentProgramState[i] = true;
                            if (!ruleVec[i].isActive) {
                                RuleActivityHandler(ruleVec[i], true, i);
                            }
                        }
                    }
                } while (Process32Next(hSnapshot, &pe));
            }
        }

        CloseHandle(hSnapshot);

        // Проверка на закрытие процессов
        {
            std::lock_guard<std::mutex> lock(ruleVecMutex);
            for (size_t i = 0; i < ruleVec.size(); ++i) {
                if (ruleVec[i].isActive && !currentProgramState[i]) {
                    RuleActivityHandler(ruleVec[i], false, i);
                }
            }
        }

        Sleep(1000);  // Проверяем процессы каждую секунду
    }

    return 0;
}
// Функция запуска потока
HANDLE ThrStartMonitoring() {
    return CreateThread(nullptr, 0, MonitorPrograms, nullptr, 0, nullptr);
}
// Функция остановки потока
void ThrStopMonitoring(HANDLE hThread) {
    stopMonitoring = true;
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
}

// Функция для изменения значения реестра
bool UpdateRegistryValue(const Rule& rule, bool isNowActive) {
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(rule.keyFolder, rule.keyPath, 0, KEY_SET_VALUE, &hKey);
    if (status != ERROR_SUCCESS) {
        return false;  
    }

    const WCHAR* valueToSet = isNowActive ? rule.newValue : rule.oldValue;
    DWORD dataSize = (rule.valTypeCode == REG_SZ) ? (wcslen(valueToSet) + 1) * sizeof(WCHAR) : 512;

    status = RegSetValueEx(hKey, rule.valName, 0, rule.valTypeCode, (const BYTE*)valueToSet, dataSize);
    RegCloseKey(hKey);

    return status == ERROR_SUCCESS;
}

// Функция для обновления значения реестра и флага активности
void RuleActivityHandler(Rule& rule, bool isNowActive, int i) {
    if (UpdateRegistryValue(rule, isNowActive)) {
        rule.isActive = isNowActive ? TRUE : FALSE;
        LvUpdateActivity(i, rule.isActive);
    }
    else {
        // TODO: добавить обработку ошибки, если нужно
    }
}

// Функция для обновления значения isActive в ListView
void LvUpdateActivity(int itemIndex, bool isActive) {
    std::lock_guard<std::mutex> lock(listViewMutex);

    // Подготавливаем структуру LVITEM для изменения столбца
    LVITEM lvItem;
    lvItem.mask = LVIF_TEXT;
    lvItem.iItem = itemIndex;
    lvItem.iSubItem = 7;  // Столбец с состоянием isActive
    lvItem.pszText = isActive ? (LPWSTR)L"Да" : (LPWSTR)L"Нет";

    // Обновляем значение в ListView
    ListView_SetItem(lvRules, &lvItem);
    UpdateWindow(lvRules);
}