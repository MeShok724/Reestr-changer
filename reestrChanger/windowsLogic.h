#pragma once
#include "framework.h"

ATOM RegisterMainClass(HINSTANCE hInstance, WCHAR szWindowClass[]);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND* hWnd, HINSTANCE* hInst, WCHAR szWindowClass[], WCHAR szTitle[]);