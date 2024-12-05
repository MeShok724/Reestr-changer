#include <mutex>
#include <vector>
#include "framework.h"
#pragma once

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

extern WCHAR szWindowClass[];
extern WCHAR szProgWindowClass[];
extern WCHAR szMainWindowTitle[];
extern WCHAR szProcChooseTitle[];

extern HINSTANCE hInst;
extern HWND hWndMainWindow;
extern std::vector<Rule> ruleVec;
extern std::mutex ruleVecMutex;
extern HWND lvRules;
extern std::mutex listViewMutex;
extern WCHAR selectedWinName[256];