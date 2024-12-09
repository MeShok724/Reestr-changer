#include "framework.h"
#include "Resource.h"
#include <CommCtrl.h>
#include <atomic>
#include <TlHelp32.h>
#include <vector>
#include <mutex>
#include "GlobalVariables.h"
#include "ruleTable.h"

HANDLE thrMonitoring;
std::atomic<bool> stopMonitoring{ true };

// Function declarations
void RuleActivityHandler(Rule& rule, bool isNowActive, int i);
void ReturnReestrStartValues();

DWORD WINAPI MonitorPrograms(LPVOID lpParam) {

    while (!stopMonitoring) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        std::vector<bool> currentProgramState;

        // Защищаем доступ к monitoredPrograms и создаем состояние процессов
        {
            std::lock_guard<std::mutex> lock(ruleVecMutex);
            currentProgramState.resize(ruleVec.size(), false);

            if (Process32First(hSnapshot, &pe)) {
                do {
                    for (size_t i = 0; i < ruleVec.size(); ++i) {
                        if (std::wstring(pe.szExeFile) == ruleVec[i].progName) {
                            currentProgramState[i] = true;
                            if (!ruleVec[i].isActive) {
                                RuleActivityHandler(ruleVec[i], true, i);
                            }
                        }
                    }
                } while (Process32Next(hSnapshot, &pe));
            }
        }

        CloseHandle(hSnapshot);

        // Проверка на закрытие процессов
        {
            std::lock_guard<std::mutex> lock(ruleVecMutex);
            for (size_t i = 0; i < ruleVec.size(); ++i) {
                if (ruleVec[i].isActive && !currentProgramState[i]) {
                    RuleActivityHandler(ruleVec[i], false, i);
                }
            }
        }

        Sleep(1000);  // Проверяем процессы каждую секунду
    }

    return 0;
}
// Функция запуска потока
BOOL ThrStartMonitoring() {
    if (!stopMonitoring) {
        return false; // поток уже запущен
    }
    ruleVec;
    stopMonitoring = false;
    thrMonitoring = CreateThread(nullptr, 0, MonitorPrograms, nullptr, 0, nullptr);
    return true;
}
// Функция остановки потока
BOOL ThrStopMonitoring() {
    if (stopMonitoring) {
        return false; // поток уже остановлен
    }
    stopMonitoring = true;
    WaitForSingleObject(thrMonitoring, INFINITE);
    CloseHandle(thrMonitoring);
    thrMonitoring = nullptr;
    ReturnReestrStartValues();
    return true;
}

// Функция для изменения значения реестра
bool UpdateRegistryValue(const Rule& rule, bool isNowActive) {
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(rule.keyFolder, rule.keyPath, 0, KEY_SET_VALUE, &hKey);
    if (status != ERROR_SUCCESS) {
        return false;
    }

    const WCHAR* valueToSet = isNowActive ? rule.newValue : rule.oldValue;
    DWORD dataSize = (rule.valTypeCode == REG_SZ) ? (wcslen(valueToSet) + 1) * sizeof(WCHAR) : 512;

    status = RegSetValueEx(hKey, rule.valName, 0, rule.valTypeCode, (const BYTE*)valueToSet, dataSize);
    RegCloseKey(hKey);

    return status == ERROR_SUCCESS;
}

void RuleActivityHandler(Rule &rule, bool isNowActive, int i) {
    if (UpdateRegistryValue(rule, isNowActive)) {
        rule.isActive = isNowActive ? TRUE : FALSE;
        LvUpdateActivity(i, rule.isActive);
    }
}
void ReturnReestrStartValues() {
    for (int i = 0; i < ruleVec.size(); i++)
    {
        Rule* currRule = &ruleVec[i];
        if (currRule->isActive) {
            currRule->isActive = false;
            RuleActivityHandler(*currRule, false, i);
        }
    }
}