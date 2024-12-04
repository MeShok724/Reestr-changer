#pragma once
#include "framework.h"

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