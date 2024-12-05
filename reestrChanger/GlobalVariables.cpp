#include "GlobalVariables.h"

WCHAR szWindowClass[] = L"MyClass1";
WCHAR szProgWindowClass[] = L"MyClass2";
WCHAR szMainWindowTitle[] = L"Список правил";
WCHAR szProcChooseTitle[] = L"Список приложений";

HINSTANCE hInst;
HWND hWndMainWindow;
std::vector<Rule> ruleVec;
std::mutex ruleVecMutex;
HWND lvRules;
std::mutex listViewMutex;
WCHAR selectedWinName[256];