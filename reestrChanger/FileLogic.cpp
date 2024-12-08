#include <fstream>
#include <vector>
#include <windows.h>
#include <commdlg.h> // ��� ������ ������� ������ �����
#include "GlobalVariables.h"

// ������� ��� ������ ����� ����� ������
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

// ������� ��� ���������� ������� � ����
void SaveRulesToFile() {
    std::wstring fileName = ChooseFileName(true); // true ��� ������� ����������
    if (fileName.empty()) {
        MessageBox(nullptr, L"���������� ��������", L"����������", MB_OK | MB_ICONINFORMATION);
        return;
    }

    std::ofstream outFile(fileName, std::ios::binary);
    if (!outFile) {
        MessageBox(nullptr, L"������ �������� ����� ��� ������", L"������", MB_OK | MB_ICONERROR);
        return;
    }

    size_t size = ruleVec.size();
    outFile.write(reinterpret_cast<const char*>(&size), sizeof(size)); // ��������� ���������� ���������
    outFile.write(reinterpret_cast<const char*>(ruleVec.data()), size * sizeof(Rule)); // ��������� ��������
    outFile.close();

    MessageBox(nullptr, L"������ ������� ���������", L"����������", MB_OK | MB_ICONINFORMATION);
}

// ������� ��� �������� ������� �� �����
void LoadRulesFromFile() {
    std::wstring fileName = ChooseFileName(false); // false ��� ������� ��������
    if (fileName.empty()) {
        MessageBox(nullptr, L"�������� ��������", L"����������", MB_OK | MB_ICONINFORMATION);
        return;
    }

    std::ifstream inFile(fileName, std::ios::binary);
    if (!inFile) {
        MessageBox(nullptr, L"������ �������� ����� ��� ������", L"������", MB_OK | MB_ICONERROR);
        return;
    }

    size_t size = 0;
    inFile.read(reinterpret_cast<char*>(&size), sizeof(size)); // ������ ���������� ���������
    ruleVec.resize(size); // �������� ����� ��� ��������
    inFile.read(reinterpret_cast<char*>(ruleVec.data()), size * sizeof(Rule)); // ������ ��������
    inFile.close();

    MessageBox(nullptr, L"������ ������� ���������", L"����������", MB_OK | MB_ICONINFORMATION);
}