#include "framework.h"
#include "reestrChanger.h"
#include <commctrl.h>
#include <vector>
#include <string>
#include <atomic>
#include <tlhelp32.h>
#include <mutex>


#define MAX_LOADSTRING 100
#define CMD_CHOSEPROG 1
#define CMD_CREATERUL 2
#define CMD_CHECKPROGNAME 3
#define CMD_STARTTRACKING 4
#define CMD_STOPTRACKING 5

// Types
struct Rule {
    WCHAR progName[256];
    HKEY keyFolder;
    WCHAR keyPath[512];
    WCHAR valName[100];
    DWORD valTypeCode;
    WCHAR oldValue[512];
    WCHAR newValue[512];
    BOOL isActive;
};

// Global Variables:
HINSTANCE hInst;                                
HWND hWnd;
HWND inpFolder, inpValName, inpPath, inpVal, choosenProg;
HWND lv_rules, lv_progs;
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
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL                InitControls(HWND hwnd);
void                OnButtonClicked(HWND hwnd);
void                AddColumnsToListView(HWND lv_rules);
void                AddRuleToListView(HWND lv_rules, Rule rule);
BOOL                CreateRule(HWND hWnd);
BOOL                GetRegistryValueAndType(Rule *rule);
void                RuleActivityHandler(Rule& rule, bool isNowActive, int i);
void                LvUpdateActivity(int itemIndex, bool isActive);
HANDLE              ThrStartMonitoring();
void                ThrStopMonitoring(HANDLE hThread);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
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
    lv_rules = CreateWindow(WC_LISTVIEW, L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER,
        400, 40, 800, 500,
        hwnd, NULL, NULL, NULL);
    ListView_SetExtendedListViewStyle(lv_rules, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_AUTOSIZECOLUMNS);
    AddColumnsToListView(lv_rules);

    SendMessage(inpFolder, CB_ADDSTRING, 0, (LPARAM)L"HKEY_CURRENT_USER");
    SendMessage(inpFolder, CB_ADDSTRING, 0, (LPARAM)L"HKEY_LOCAL_MACHINE");
    SendMessage(inpFolder, CB_ADDSTRING, 0, (LPARAM)L"HKEY_CLASSES_ROOT");
    SendMessage(inpFolder, CB_ADDSTRING, 0, (LPARAM)L"HKEY_USERS");
    SendMessage(inpFolder, CB_ADDSTRING, 0, (LPARAM)L"HKEY_CURRENT_CONFIG");
    SendMessage(inpFolder, CB_SETCURSEL, 0, 0);

    // TODO: сделать сохранение списка правил в файл
    // TODO: сделать загрузку списка правил из файла
    // TODO: сделать создание нового списка правил

    return true;
}

// Обновление выбранного приложения
void CheckWinName() {
    if (selectedWinName != NULL)
        SetWindowText(choosenProg, selectedWinName);
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
                OnButtonClicked(hWnd);
                break;
            case CMD_CREATERUL:
                if (!CreateRule(hWnd))
                    //TODO: Обработать ошибку создания правила                   
                break;
            case CMD_CHECKPROGNAME:
                CheckWinName();
                break;
            case CMD_STARTTRACKING:
                thrMonitoring = ThrStartMonitoring();
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
        // TODO: сделать или удалить
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

// Callback-функция для перечисления окон
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    // Получение заголовка окна
    wchar_t windowTitle[256];
    GetWindowText(hwnd, windowTitle, sizeof(windowTitle) / sizeof(wchar_t));

    // Проверяем, есть ли заголовок у окна (если пусто — пропускаем)
    if (wcslen(windowTitle) > 0) {  //  && wcscmp(windowTitle, L"Default IME") != 0 && wcscmp(windowTitle, L"") != 0
        // Добавляем заголовок в список (ListView)
        HWND listView = (HWND)lParam;
        LVITEM lvItem = { 0 };
        lvItem.mask = LVIF_TEXT;
        lvItem.pszText = windowTitle;
        lvItem.iItem = ListView_GetItemCount(listView); // следующий индекс
        ListView_InsertItem(listView, &lvItem); // добавляем строку
    }
    return TRUE; // продолжаем перечисление окон
}

// Получение выбранного приложения
bool ReadChosenName(HWND hwnd) {
    int selectedIndex = ListView_GetNextItem(lv_progs, -1, LVNI_SELECTED);
    if (selectedIndex != -1) {
        // Элемент выбран, получаем его текст
        LVITEM lvItem;
        lvItem.iSubItem = 0;
        lvItem.cchTextMax = 256;
        lvItem.pszText = selectedWinName;
        lvItem.iItem = selectedIndex;
        lvItem.mask = LVIF_TEXT;
        ListView_GetItem(lv_progs, &lvItem);
        return true;
    }
    else {
        return false;
    }
}

// Создание окна со списком приложений
void OnButtonClicked(HWND hwnd) {
    HWND hwndNew = CreateWindow(szProgWindowClass, L"Список приложений",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
        hwnd, NULL, NULL, NULL);

    if (hwndNew == NULL) {
        return; // TODO: обработка ошибки создания окна
    }

    lv_progs = CreateWindow(WC_LISTVIEW, L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER,
        10, 10, 460, 350,
        hwndNew, NULL, NULL, NULL);

    LVCOLUMN lvCol;
    lvCol.mask = LVCF_TEXT | LVCF_WIDTH;
    lvCol.pszText = (LPWSTR)L"Название окна";
    lvCol.cx = 400;
    ListView_InsertColumn(lv_progs, 0, &lvCol);

    CreateWindow(L"BUTTON", L"Выбрать", WS_VISIBLE | WS_CHILD, 20, 380, 70, 30, hwndNew, (HMENU)CMD_CHOSEPROG, NULL, NULL);

    // TODO: обновление списка активных приложений
    // TODO: фильтровать повторяющиеся окна или другое решение

    // Перечисление всех работающих окон
   // EnumWindows(EnumWindowsProc, (LPARAM)lv_progs);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnapshot, &pe)) {
        do {
            LVITEM lvItem = { 0 };
            lvItem.mask = LVIF_TEXT;
            lvItem.pszText = pe.szExeFile;
            lvItem.iItem = ListView_GetItemCount(lv_progs); // следующий индекс
            ListView_InsertItem(lv_progs, &lvItem); // добавляем строку
        } while (Process32Next(hSnapshot, &pe));
    }
    // Обновление отображения окна
    ShowWindow(hwndNew, SW_SHOW);
}

// Обработка сообщений окна выбора приложения
LRESULT CALLBACK WndProgProc(HWND hProgWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case CMD_CHOSEPROG:
            if (ReadChosenName(hProgWnd))
                SendMessage(hProgWnd, WM_CLOSE, NULL, NULL);
            break;
        default:
            return DefWindowProc(hProgWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_NOTIFY: 
    {
        LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
        if (lpnmitem->hdr.hwndFrom == lv_progs && lpnmitem->hdr.code == NM_DBLCLK)
            SendMessage(hProgWnd, WM_COMMAND, CMD_CHOSEPROG, NULL);
    }
    break;
    case WM_CLOSE:
    {
        SendMessage(hWnd, WM_COMMAND, CMD_CHECKPROGNAME, NULL);
        SendMessage(hProgWnd, WM_DESTROY, NULL, NULL);
    }
    break;
    case WM_DESTROY: {
        DestroyWindow(hProgWnd);
        UpdateWindow(hWnd);
    }
    break;
    default:
        return DefWindowProc(hProgWnd, message, wParam, lParam);
    }
    return 0;
}

// TODO: в другой модуль
// Добавление столбцов в таблицу правил
void AddColumnsToListView(HWND lv_rules) {
    LVCOLUMN lvColumn;
    lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lvColumn.pszText = (LPWSTR)L"Название программы";
    lvColumn.cx = 100;
    lvColumn.iSubItem = 0;
    ListView_InsertColumn(lv_rules, 0, &lvColumn);

    lvColumn.pszText = (LPWSTR)L"Раздел реестра";
    lvColumn.iSubItem = 1;
    ListView_InsertColumn(lv_rules, 1, &lvColumn);

    lvColumn.pszText = (LPWSTR)L"Путь";
    lvColumn.cx = 150;
    lvColumn.iSubItem = 2;
    ListView_InsertColumn(lv_rules, 2, &lvColumn);

    lvColumn.pszText = (LPWSTR)L"Название величины";
    lvColumn.cx = 100;
    lvColumn.iSubItem = 3;
    ListView_InsertColumn(lv_rules, 3, &lvColumn);

    lvColumn.pszText = (LPWSTR)L"Тип значения";
    lvColumn.iSubItem = 4;
    ListView_InsertColumn(lv_rules, 4, &lvColumn);

    lvColumn.pszText = (LPWSTR)L"Новое значение";
    lvColumn.iSubItem = 5;
    ListView_InsertColumn(lv_rules, 5, &lvColumn);

    lvColumn.pszText = (LPWSTR)L"Старое значение";
    lvColumn.iSubItem = 6;
    ListView_InsertColumn(lv_rules, 6, &lvColumn);

    lvColumn.pszText = (LPWSTR)L"Активно";
    lvColumn.iSubItem = 7;
    lvColumn.cx = 50;
    ListView_InsertColumn(lv_rules, 7, &lvColumn);
}

// в другой модуль
std::wstring TypeToStr(DWORD typeCode) {
    switch (typeCode) {
    case REG_SZ:
        return L"REG_SZ";
    case REG_BINARY:
        return L"REG_BINARY";
    case REG_DWORD:
        return L"REG_DWORD";
    default:
        return L"UNKNOWN_TYPE";
    }
}


// TODO: в другой модуль
// Добавление правила в таблицу
void AddRuleToListView(HWND lv_rules, Rule rule) {
    LVITEM lvItem;
    lvItem.mask = LVIF_TEXT;
    lvItem.iItem = ListView_GetItemCount(lv_rules); // Получаем текущий индекс для новой строки

    lvItem.iSubItem = 0;
    lvItem.pszText = rule.progName;
    ListView_InsertItem(lv_rules, &lvItem);

    lvItem.iSubItem = 1;
    WCHAR hKeyStr[64];
    if (rule.keyFolder == HKEY_CLASSES_ROOT) {
        wcscpy_s(hKeyStr, L"HKEY_CLASSES_ROOT");
    }
    else if (rule.keyFolder == HKEY_CURRENT_USER) {
        wcscpy_s(hKeyStr, L"HKEY_CURRENT_USER");
    }
    else if (rule.keyFolder == HKEY_LOCAL_MACHINE) {
        wcscpy_s(hKeyStr, L"HKEY_LOCAL_MACHINE");
    }
    else if (rule.keyFolder == HKEY_USERS) {
        wcscpy_s(hKeyStr, L"HKEY_USERS");
    }
    else {
        wcscpy_s(hKeyStr, L"UNKNOWN_KEY");
    }
    lvItem.pszText = hKeyStr;
    ListView_SetItem(lv_rules, &lvItem);

    lvItem.iSubItem = 2;
    lvItem.pszText = rule.keyPath;
    ListView_SetItem(lv_rules, &lvItem);

    lvItem.iSubItem = 3;
    lvItem.pszText = rule.valName;
    ListView_SetItem(lv_rules, &lvItem);

    lvItem.iSubItem = 4;
    std::wstring type = TypeToStr(rule.valTypeCode);
    lvItem.pszText = (LPWSTR)type.c_str();
    ListView_SetItem(lv_rules, &lvItem);

    lvItem.iSubItem = 5;
    lvItem.pszText = rule.newValue;
    ListView_SetItem(lv_rules, &lvItem);

    lvItem.iSubItem = 6;
    lvItem.pszText = rule.oldValue;
    ListView_SetItem(lv_rules, &lvItem);

    lvItem.iSubItem = 7;
    lvItem.pszText = rule.isActive ? (LPWSTR)L"Да" : (LPWSTR)L"Нет";
    ListView_SetItem(lv_rules, &lvItem);

    UpdateWindow(hWnd);
}

HKEY GetRootKey() {
    int idx = SendMessage(inpFolder, CB_GETCURSEL, 0, 0);
    switch (idx) {
        case 0: return HKEY_CURRENT_USER;
        case 1: return HKEY_LOCAL_MACHINE;
        case 2: return HKEY_CLASSES_ROOT;
        case 3: return HKEY_USERS;
        case 4: return HKEY_CURRENT_CONFIG;
    }
}

BOOL CreateRule(HWND hWnd) {
    Rule newRule;
    newRule.keyFolder = GetRootKey();
    //TODO: правильно считать введенное значение опираясь на тип
    GetWindowText(choosenProg, newRule.progName, sizeof(newRule.progName) / sizeof(wchar_t));
    GetWindowText(inpPath, newRule.keyPath, sizeof(newRule.keyPath) / sizeof(wchar_t));
    GetWindowText(inpVal, newRule.newValue, sizeof(newRule.newValue) / sizeof(wchar_t));
    GetWindowText(inpValName, newRule.valName, sizeof(newRule.valName) / sizeof(wchar_t));
    if (!GetRegistryValueAndType(&newRule))
        return FALSE;
    newRule.isActive = FALSE;    // TODO: правильно указать isActive
    AddRuleToListView(lv_rules, newRule);
    ruleVec.push_back(newRule);
    return TRUE;
}

// TODO: в другой модуль
BOOL GetRegistryValueAndType(Rule *rule) {
    HKEY hKey; 
    DWORD type;
    DWORD dwSize = sizeof(rule->oldValue);

    // Открываем ключ реестра
    if (RegOpenKeyEx(rule->keyFolder, rule->keyPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        // Получаем значение по указанному имени
        LSTATUS status = RegQueryValueEx(hKey, rule->valName, NULL, &type, (LPBYTE)rule->oldValue, &dwSize);
        if (status != ERROR_SUCCESS) {
            return FALSE;
        }
        rule->valTypeCode = type;
        // Закрываем ключ реестра
        RegCloseKey(hKey);
        return TRUE;
    }
    else {
        return FALSE;
    }
}


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
    ListView_SetItem(lv_rules, &lvItem);
    UpdateWindow(lv_rules);
}