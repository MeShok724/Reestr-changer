#include <fstream>
#include <vector>
#include <windows.h>
#include <commdlg.h> // Для вызова диалога выбора файла
#include "GlobalVariables.h"

// Функция для выбора файла через диалог
std::wstring ChooseFileName(bool saveDialog) {
    wchar_t fileName[MAX_PATH] = L"";
    OPENFILENAME ofn = {};
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = saveDialog ? OFN_OVERWRITEPROMPT : OFN_FILEMUSTEXIST;

    if (saveDialog) {
        if (GetSaveFileName(&ofn)) {
            return fileName;
        }
    }
    else {
        if (GetOpenFileName(&ofn)) {
            return fileName;
        }
    }
    return L"";
}

// Функция для сохранения вектора в файл
void SaveRulesToFile() {
    std::wstring fileName = ChooseFileName(true); // true для диалога сохранения
    if (fileName.empty()) {
        MessageBox(nullptr, L"Сохранение отменено", L"Информация", MB_OK | MB_ICONINFORMATION);
        return;
    }

    std::ofstream outFile(fileName, std::ios::binary);
    if (!outFile) {
        MessageBox(nullptr, L"Ошибка открытия файла для записи", L"Ошибка", MB_OK | MB_ICONERROR);
        return;
    }

    size_t size = ruleVec.size();
    outFile.write(reinterpret_cast<const char*>(&size), sizeof(size)); // Сохраняем количество элементов
    outFile.write(reinterpret_cast<const char*>(ruleVec.data()), size * sizeof(Rule)); // Сохраняем элементы
    outFile.close();

    MessageBox(nullptr, L"Данные успешно сохранены", L"Информация", MB_OK | MB_ICONINFORMATION);
}

// Функция для загрузки вектора из файла
void LoadRulesFromFile() {
    std::wstring fileName = ChooseFileName(false); // false для диалога открытия
    if (fileName.empty()) {
        MessageBox(nullptr, L"Загрузка отменена", L"Информация", MB_OK | MB_ICONINFORMATION);
        return;
    }

    std::ifstream inFile(fileName, std::ios::binary);
    if (!inFile) {
        MessageBox(nullptr, L"Ошибка открытия файла для чтения", L"Ошибка", MB_OK | MB_ICONERROR);
        return;
    }

    size_t size = 0;
    inFile.read(reinterpret_cast<char*>(&size), sizeof(size)); // Читаем количество элементов
    ruleVec.resize(size); // Выделяем место под элементы
    inFile.read(reinterpret_cast<char*>(ruleVec.data()), size * sizeof(Rule)); // Читаем элементы
    inFile.close();

    MessageBox(nullptr, L"Данные успешно загружены", L"Информация", MB_OK | MB_ICONINFORMATION);
}