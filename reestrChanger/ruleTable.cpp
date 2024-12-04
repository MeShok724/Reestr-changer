#include "framework.h"
#include "Resource.h"
#include <CommCtrl.h>

#include "Rule.h"
#include <string>

HWND lvRules;

HWND CreateLvRules(HWND hWnd) {
    lvRules = CreateWindow(WC_LISTVIEW, L"",
        WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER,
        400, 40, 800, 500,
        hWnd, NULL, NULL, NULL);
    ListView_SetExtendedListViewStyle(lvRules, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_AUTOSIZECOLUMNS);
    AddColumnsToListView(lvRules);

    return lvRules;
}

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

    UpdateWindow(lv_rules);
}

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