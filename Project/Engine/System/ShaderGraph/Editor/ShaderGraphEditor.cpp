#include "ShaderGraphEditor.h"
#include <windows.h>
#include <shobjidl.h>
#include <iostream>
#include "Engine/System/ShaderGraph/Editor/ShaderGraphSerializer.h"
#include "Engine/Utilities/Convert.h"

static float zoom = 1.0f;           // 現在のズーム倍率
static ImVec2 pan = ImVec2(0, 0);   // 平行移動（パン）量

ShaderGraphEditor::ShaderGraphEditor() {
}

ShaderGraphEditor::~ShaderGraphEditor() {
	editor_.reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化関数
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::Init() {
	editor_ = std::make_unique<ImFlow::ImNodeFlow>();
	nodeFactory_.Init(editor_.get());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::Update() {
	Edit();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::Edit() {
	SaveGraph();
	ImGui::SameLine();
	LoadGraph();

	editor_->update();

	// ----------------------
	// ↓ node更新処理
	// ----------------------
	std::unordered_set<ImFlow::BaseNode*> visited;
	for (auto& [id, nodePtr] : editor_->getNodes()) {
		ImFlow::BaseNode* node = nodePtr.get();

		for (auto& out : node->getOuts()) {
			if (out->isConnected()) {
				continue;
			}
		}

		// 末端からたどるようにする
		ExecuteFrom(node, visited);
	}

	// ----------------------
	// ↓ 編集処理
	// ----------------------
	ImGui::Begin("Inspector");
	for (auto& node : editor_->getNodes()) {
		if (node.second.get()->isSelected()) {
			node.second.get()->updateGui();
		}
	}
	ImGui::End();

	// ---------------------- 
	// ↓ 作成処理
	// ----------------------
	CreateNode();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 親Nodeから子Nodeにたどるようにする
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::ExecuteFrom(ImFlow::BaseNode* node, std::unordered_set<ImFlow::BaseNode*>& visited) {
	// 訪れたNodeかどうかを確認する
	if (!node || visited.contains(node)) return;
	visited.insert(node);

	// すべての入力ピンをチェック
	for (auto& in : node->getIns()) {
		// このInPinがリンクされているなら
		auto link = in->getLink().lock();
		if (link) {
			// 左側（出力ピン）の親ノードを取得
			ImFlow::BaseNode* srcNode = link->left()->getParent();
			ExecuteFrom(srcNode, visited); // 依存ノードを先に実行
		}

		node->customUpdate();
	}

	if (node->getIns().empty()) {
		node->customUpdate();
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ nodeの作成
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::CreateNode() {
	// 毎フレーム呼ばれる更新処理の中で
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		popupPos_ = ImGui::GetMousePos();
		popupRequested_ = true;
	}

	editor_->rightClickPopUpContent([this](ImFlow::BaseNode*) {
		if (popupRequested_) {
			ImGui::SetNextWindowPos(popupPos_);
			popupRequested_ = false;
		}
	
		ImGui::TextUnformatted(" NodeContextMenu ");
		nodeFactory_.CreateGui(popupPos_);

								  });
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Graphの読み込み
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::LoadGraph() {
	if (ImGui::Button("Load")) {
		editor_.reset(new ImFlow::ImNodeFlow);
		nodeFactory_.Init(editor_.get());

		// ファイルを選択
		std::string path = OpenWindowsExplore();
		if (path != "") {
			nodeFactory_.CreateGraph(ShaderGraphSerializer::Load(path));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Graphを保存する
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::SaveGraph() {
	if (ImGui::Button("Save")) {
		std::string path = SaveWindowsExplore();

		if (path != "") {
			ShaderGraphSerializer::Save(path, editor_.get());
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ windowsのエクスプローラーを開く
///////////////////////////////////////////////////////////////////////////////////////////////

std::string ShaderGraphEditor::OpenWindowsExplore() {
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
	return ConvertString(path);
}

std::string ShaderGraphEditor::SaveWindowsExplore() {
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
	return ConvertString(path);
}
