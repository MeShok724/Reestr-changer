#include "framework.h"
#include <CommCtrl.h>

// ��������� ���������� ����������
bool ReadChosenName(HWND hwnd, HWND lvProc, WCHAR selectedWinName[]) {
    int selectedIndex = ListView_GetNextItem(lvProc, -1, LVNI_SELECTED);
    if (selectedIndex != -1) {
        // ������� ������, �������� ��� �����
        LVITEM lvItem;
        lvItem.iSubItem = 0;
        lvItem.cchTextMax = 256;
        lvItem.pszText = selectedWinName;
        lvItem.iItem = selectedIndex;
        lvItem.mask = LVIF_TEXT;
        ListView_GetItem(lvProc, &lvItem);
        return true;
    }
    else {
        return false;
    }
}