#include "framework.h"
#include "Resource.h"

#include "ruleTable.h"

#include <vector>

// Function declarations
BOOL GetRegistryValueAndType(Rule* rule);

// Обновление выбранного приложения
void CheckWinName(WCHAR selectedWinName[], HWND choosenProg) {
    if (selectedWinName != NULL)
        SetWindowText(choosenProg, selectedWinName);
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

HKEY GetRootKey(HWND inpFolder) {
    int idx = SendMessage(inpFolder, CB_GETCURSEL, 0, 0);
    switch (idx) {
    case 0: return HKEY_CURRENT_USER;
    case 1: return HKEY_LOCAL_MACHINE;
    case 2: return HKEY_CLASSES_ROOT;
    case 3: return HKEY_USERS;
    case 4: return HKEY_CURRENT_CONFIG;
    }
}

BOOL CreateRule(HWND hWnd, HWND choosenProg, HWND inpFolder, HWND inpPath, HWND inpVal, HWND inpValName) {
    Rule newRule;
    newRule.keyFolder = GetRootKey(inpFolder);
    //TODO: правильно считать введенное значение опираясь на тип
    GetWindowText(choosenProg, newRule.progName, sizeof(newRule.progName) / sizeof(wchar_t));
    GetWindowText(inpPath, newRule.keyPath, sizeof(newRule.keyPath) / sizeof(wchar_t));
    GetWindowText(inpVal, newRule.newValue, sizeof(newRule.newValue) / sizeof(wchar_t));
    GetWindowText(inpValName, newRule.valName, sizeof(newRule.valName) / sizeof(wchar_t));
    if (!GetRegistryValueAndType(&newRule))
        return FALSE;
    newRule.isActive = FALSE;    // TODO: правильно указать isActive
    AddRuleToListView(lvRules, newRule);
    ruleVec.push_back(newRule);
    return TRUE;
}

BOOL GetRegistryValueAndType(Rule* rule) {
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