#include "FileDialogFunc.h"
#include <windows.h>
#include <shobjidl.h>
#include <iostream>
#include <filesystem>
#include "Engine/Utilities/Convert.h"

std::string FileSaveDialogFunc() {
    std::wstring path;
    IFileSaveDialog* pFileSave = nullptr;

    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);

    std::filesystem::path exePath = buffer;
    exePath = exePath.parent_path().parent_path().parent_path().parent_path().wstring();

    if (SUCCEEDED(CoCreateInstance(
        CLSID_FileSaveDialog, nullptr, CLSCTX_ALL,
        IID_IFileSaveDialog, (void**)&pFileSave))) {

        if (!exePath.empty()) {
            IShellItem* pFolder = nullptr;
            if (SUCCEEDED(SHCreateItemFromParsingName(exePath.c_str(), nullptr, IID_PPV_ARGS(&pFolder)))) {
                pFileSave->SetFolder(pFolder);
                pFolder->Release();
            }
        }

        COMDLG_FILTERSPEC filters[] = {
            {L"Text Files", L"*.json"},
            {L"All Files",  L"*.*"}
        };
        pFileSave->SetFileTypes(ARRAYSIZE(filters), filters);
        pFileSave->SetFileName(L"newfile.json");

        // ---- ここ重要：キャンセル時は "" を返す ----
        if (pFileSave->Show(nullptr) != S_OK) {
            pFileSave->Release();
            return "";   // ← キャンセル or 閉じた場合は空文字
        }

        IShellItem* pItem = nullptr;
        if (pFileSave->GetResult(&pItem) != S_OK) {
            pFileSave->Release();
            return "";   // ← 念のため
        }

        PWSTR pszFilePath = nullptr;
        if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath))) {
            path = pszFilePath;
            CoTaskMemFree(pszFilePath);
        }
        pItem->Release();
        pFileSave->Release();
    }

    if (path.empty()) {
        return ""; // 念のため
    }

    std::filesystem::path resultPath = path;
    resultPath = std::filesystem::relative(resultPath, exePath);

    return ".\\" + ConvertString(resultPath);
}

std::string FileOpenDialogFunc() {
    std::wstring path;
    IFileOpenDialog* pFileOpen = nullptr;

    HRESULT hr = CoCreateInstance(
        CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
        IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen)
    );

    // 現在のルートパス（初期フォルダ用）
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);

    std::filesystem::path exePath = buffer;
    exePath = exePath.parent_path().parent_path().parent_path().parent_path().wstring();

    if (FAILED(hr)) {
        return "";
    }

    // 初期フォルダの設定
    {
        IShellItem* pFolder = nullptr;
        if (SUCCEEDED(SHCreateItemFromParsingName(exePath.c_str(), nullptr, IID_PPV_ARGS(&pFolder)))) {
            pFileOpen->SetFolder(pFolder);
            pFolder->Release();
        }
    }

    // ---- ここが重要：キャンセル時はこの時点で return "" ----
    if (pFileOpen->Show(nullptr) != S_OK) {
        pFileOpen->Release();
        return "";
    }

    // 結果取得
    IShellItem* pItem = nullptr;
    if (pFileOpen->GetResult(&pItem) != S_OK) {
        pFileOpen->Release();
        return "";
    }

    PWSTR filePath = nullptr;
    if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath))) {
        path = filePath;
        CoTaskMemFree(filePath);
    }

    pItem->Release();
    pFileOpen->Release();

    // 取得できなかった場合も空
    if (path.empty()) {
        return "";
    }

    std::filesystem::path resultPath = path;
    resultPath = std::filesystem::relative(resultPath, exePath);

    return ".\\" + ConvertString(resultPath);
}
