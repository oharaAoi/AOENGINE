#include "FileDialogFunc.h"
#include <windows.h>
#include <shobjidl.h>
#include <iostream>
#include <filesystem>
#include "Engine/Utilities/Convert.h"

std::string FileSaveDialogFunc() {
	std::wstring path;
	IFileSaveDialog* pFileSave = nullptr;

	// 現在のルートパスを探索する
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(nullptr, buffer, MAX_PATH);

	std::filesystem::path exePath = buffer;
	exePath = exePath.parent_path().parent_path().parent_path().parent_path().wstring();

	// ダイアログを開く処理
	if (SUCCEEDED(CoCreateInstance(
		CLSID_FileSaveDialog, nullptr, CLSCTX_ALL,
		IID_IFileSaveDialog, (void**)&pFileSave))) {

		// 初期フォルダ指定
		if (!exePath.empty()) {
			IShellItem* pFolder = nullptr;
			if (SUCCEEDED(SHCreateItemFromParsingName(exePath.c_str(), nullptr, IID_PPV_ARGS(&pFolder)))) {
				pFileSave->SetFolder(pFolder);
				pFolder->Release();
			}
		}

		// フィルター
		COMDLG_FILTERSPEC filters[] = {
			{L"Text Files", L"*.json"},
			{L"All Files",  L"*.*"}
		};
		pFileSave->SetFileTypes(ARRAYSIZE(filters), filters);
		pFileSave->SetFileName(L"newfile.txt"); // 初期ファイル名

		// ダイアログ表示
		if (SUCCEEDED(pFileSave->Show(nullptr))) {
			IShellItem* pItem = nullptr;
			if (SUCCEEDED(pFileSave->GetResult(&pItem))) {
				PWSTR pszFilePath = nullptr;
				if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath))) {
					path = pszFilePath; // 完全パス
					CoTaskMemFree(pszFilePath);
				}
				pItem->Release();
			}
		}

		pFileSave->Release();
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

	// 現在のルートパスを探索する
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(nullptr, buffer, MAX_PATH);

	std::filesystem::path exePath = buffer;
	exePath = exePath.parent_path().parent_path().parent_path().parent_path().wstring();

	// ダイアログを開く処理
	if (SUCCEEDED(hr)) {
		// 初期フォルダの指定
		IShellItem* pFolder;
		if (SUCCEEDED(SHCreateItemFromParsingName(exePath.c_str(), nullptr, IID_PPV_ARGS(&pFolder)))) {
			pFileOpen->SetFolder(pFolder);
			pFolder->Release();
		}

		// ダイアログ表示
		if (SUCCEEDED(pFileOpen->Show(nullptr))) {
			IShellItem* pItem;
			if (SUCCEEDED(pFileOpen->GetResult(&pItem))) {
				PWSTR filePath;
				if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath))) {
					path = filePath;
					CoTaskMemFree(filePath);
				}
				pItem->Release();
			}
		}

		pFileOpen->Release();
	}

	std::filesystem::path resultPath = path;
	resultPath = std::filesystem::relative(resultPath, exePath);

	return ".\\" + ConvertString(resultPath.c_str());
}
