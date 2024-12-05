#pragma once
#include "framework.h"

ATOM RegisterMainClass(HINSTANCE hInstance);
BOOL InitControls(HWND hwnd);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND* hWnd, HINSTANCE* hInst);