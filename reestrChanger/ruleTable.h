#pragma once
#include "framework.h"
#include "GlobalVariables.h"

HWND CreateLvRules(HWND hWnd);
void AddRuleToListView(HWND lv_rules, Rule rule);
void LvUpdateActivity(int itemIndex, bool isActive);