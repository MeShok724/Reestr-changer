#pragma once
#include "framework.h"

BOOL CreateRule(HWND hWnd, HWND choosenProg, HWND inpFolder, HWND inpPath, HWND inpVal, HWND inpValName);
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void CheckWinName(WCHAR selectedWinName[], HWND choosenProg);