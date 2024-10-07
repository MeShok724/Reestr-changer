#include "framework.h"
#include "reestrChanger.h"
#include <commctrl.h>
#include <vector>


#define MAX_LOADSTRING 100
#define CMD_CHOSEPROG 1
#define CMD_CREATERUL 2

// Types
struct Rule {
    WCHAR progName[256];
    HKEY keyFolder;
    WCHAR keyPath[512];
    WCHAR valName[100];
    WCHAR oldValue[512];
    WCHAR newValue[512];
    BOOL isActive;
};

// Global Variables:
HINSTANCE hInst;                                
HWND hWnd;
HWND inp_folder, inp_valName, inp_path, inp_val, choosen_prog;
HWND lv_rules, lv_progs;
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[] = L"MyClass1";            
WCHAR szProgWindowClass[] = L"MyClass2";            
std::vector<Rule> rule_vec;

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
void                GetRegistryValue(Rule *rule);

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

BOOL InitControls(HWND hwnd) {
    CreateWindow(L"STATIC", L"Выберите раздел реестра:", WS_CHILD | WS_VISIBLE, 10, 20, 300, 20, hwnd, NULL, NULL, NULL);
    inp_folder = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, 10, 40, 300, 500, hwnd, NULL, NULL, NULL);
    CreateWindow(L"STATIC", L"Введите путь к ключу реестра:", WS_CHILD | WS_VISIBLE, 10, 70, 300, 20, hwnd, NULL, NULL, NULL);
    inp_path = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 90, 300, 20, hwnd, NULL, NULL, NULL);
    CreateWindow(L"STATIC", L"Введите название величины:", WS_CHILD | WS_VISIBLE, 10, 120, 300, 20, hwnd, NULL, NULL, NULL);
    inp_valName = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 140, 300, 20, hwnd, NULL, NULL, NULL);
    CreateWindow(L"STATIC", L"Введите нужное значение:", WS_CHILD | WS_VISIBLE, 10, 170, 300, 20, hwnd, NULL, NULL, NULL);
    inp_val = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 190, 300, 20, hwnd, NULL, NULL, NULL);
    CreateWindow(L"STATIC", L"Выбранное приложение:", WS_CHILD | WS_VISIBLE, 10, 220, 300, 20, hwnd, NULL, NULL, NULL);
    choosen_prog = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 10, 240, 300, 20, hwnd, NULL, NULL, NULL);
    CreateWindow(L"BUTTON", L"Выбрать приложение", WS_VISIBLE | WS_CHILD, 10, 270, 300, 30, hwnd, (HMENU)CMD_CHOSEPROG, NULL, NULL);
    CreateWindow(L"BUTTON", L"Создать правило", WS_VISIBLE | WS_CHILD, 10, 310, 300, 30, hwnd, (HMENU)CMD_CREATERUL, NULL, NULL);

    CreateWindow(L"STATIC", L"Список правил:", WS_CHILD | WS_VISIBLE, 400, 20, 150, 20, hwnd, NULL, NULL, NULL);
    // ListView для отображения списка правил
    lv_rules = CreateWindow(WC_LISTVIEW, L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER,
        400, 40, 800, 500,
        hwnd, NULL, NULL, NULL);
    ListView_SetExtendedListViewStyle(lv_rules, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_AUTOSIZECOLUMNS);
    AddColumnsToListView(lv_rules);

    SendMessage(inp_folder, CB_ADDSTRING, 0, (LPARAM)L"HKEY_CURRENT_USER");
    SendMessage(inp_folder, CB_ADDSTRING, 0, (LPARAM)L"HKEY_LOCAL_MACHINE");
    SendMessage(inp_folder, CB_ADDSTRING, 0, (LPARAM)L"HKEY_CLASSES_ROOT");
    SendMessage(inp_folder, CB_ADDSTRING, 0, (LPARAM)L"HKEY_USERS");
    SendMessage(inp_folder, CB_ADDSTRING, 0, (LPARAM)L"HKEY_CURRENT_CONFIG");
    SendMessage(inp_folder, CB_SETCURSEL, 0, 0);

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
                OnButtonClicked(hWnd);
                break;
            case CMD_CREATERUL:
                CreateRule(hWnd);
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
    // Получаем заголовок окна
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

// Обработчик нажатия кнопки
void OnButtonClicked(HWND hwnd) {
    HWND hwndNew = CreateWindow(szProgWindowClass, L"Список приложений",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
        hwnd, NULL, NULL, NULL);

    if (hwndNew == NULL) {
        MessageBox(NULL, L"Не удалось создать окно", L"Ошибка", MB_OK | MB_ICONERROR);
        return; // Прерываем выполнение функции, если окно не создано
    }

    // ListView для отображения списка
    lv_progs = CreateWindow(WC_LISTVIEW, L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER,
        10, 10, 460, 350,
        hwndNew, NULL, NULL, NULL);

    // ListView для отображения колонок
    LVCOLUMN lvCol;
    lvCol.mask = LVCF_TEXT | LVCF_WIDTH;
    lvCol.pszText = (LPWSTR)L"Название окна";
    lvCol.cx = 400;
    ListView_InsertColumn(lv_progs, 0, &lvCol);

    // Перечисление всех работающих окон
    EnumWindows(EnumWindowsProc, (LPARAM)lv_progs);

    // Обновляем отображение окна
    ShowWindow(hwndNew, SW_SHOW);
}

LRESULT CALLBACK WndProgProc(HWND hProgWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
            //TODO: processing choosing the program
        default:
            return DefWindowProc(hProgWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_DESTROY:
        DestroyWindow(hProgWnd);
        UpdateWindow(hWnd);
        break;
    default:
        return DefWindowProc(hProgWnd, message, wParam, lParam);
    }
    return 0;
}

// TODO: в другой модуль
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

    lvColumn.pszText = (LPWSTR)L"Новое значение";
    lvColumn.iSubItem = 4;
    ListView_InsertColumn(lv_rules, 4, &lvColumn);

    lvColumn.pszText = (LPWSTR)L"Старое значение";
    lvColumn.iSubItem = 5;
    ListView_InsertColumn(lv_rules, 5, &lvColumn);

    lvColumn.pszText = (LPWSTR)L"Активно";
    lvColumn.iSubItem = 6;
    lvColumn.cx = 50;
    ListView_InsertColumn(lv_rules, 6, &lvColumn);
}


// TODO: в другой модуль
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
    lvItem.pszText = rule.newValue;
    ListView_SetItem(lv_rules, &lvItem);

    lvItem.iSubItem = 5;
    lvItem.pszText = rule.oldValue;
    ListView_SetItem(lv_rules, &lvItem);

    lvItem.iSubItem = 6;
    lvItem.pszText = rule.isActive ? (LPWSTR)L"Да" : (LPWSTR)L"Нет";
    ListView_SetItem(lv_rules, &lvItem);

    UpdateWindow(hWnd);
}

HKEY GetRootKey() {
    int idx = SendMessage(inp_folder, CB_GETCURSEL, 0, 0);
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
    GetWindowText(choosen_prog, newRule.progName, sizeof(newRule.progName) / sizeof(wchar_t));
    GetWindowText(inp_path, newRule.keyPath, sizeof(newRule.keyPath) / sizeof(wchar_t));
    GetWindowText(inp_val, newRule.newValue, sizeof(newRule.newValue) / sizeof(wchar_t));
    GetWindowText(inp_valName, newRule.valName, sizeof(newRule.valName) / sizeof(wchar_t));
    GetRegistryValue(&newRule);
    newRule.isActive = TRUE;    // TODO: правильно указать isActive
    AddRuleToListView(lv_rules, newRule);
    rule_vec.push_back(newRule);
    return TRUE;
}

// TODO: в другой модуль
void GetRegistryValue(Rule *rule) {
    HKEY hKey; 
    DWORD dwType = REG_SZ;   
    DWORD dwSize = sizeof(rule->oldValue);  

    // Открываем ключ реестра
    if (RegOpenKeyEx(rule->keyFolder, rule->keyPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        // Получаем значение по указанному имени
        LSTATUS status = RegQueryValueEx(hKey, rule->valName, NULL, &dwType, (LPBYTE)rule->oldValue, &dwSize);
        if (status != ERROR_SUCCESS) {
            // Выводим код ошибки
            WCHAR errorMessage[256];
            wsprintf(errorMessage, L"Ошибка при чтении реестра: %ld", status);
            MessageBox(hWnd, errorMessage, L"Ошибка", MB_OK | MB_ICONERROR);
        }
        // Закрываем ключ реестра
        RegCloseKey(hKey);
    }
    else {
        MessageBox(hWnd, L"Не удалось открыть ключ реестра", L"Ошибка", MB_OK | MB_ICONERROR);
    }
}