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

void ShaderGraphEditor::ExecuteFrom(ImFlow::BaseNode* node, std::unordered_set<ImFlow::BaseNode*>& visited) {
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
// ↓ 編集処理dw
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
		
		// たとえば PreviewNode が末端とする
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
// ↓ nodeの作成
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::CreateNode() {
	editor_->rightClickPopUpContent([this](ImFlow::BaseNode*) {

		ImGui::Begin("PopUpMenu", nullptr);
		ImGui::TextUnformatted(" NodeContextMenu ");
		ImGui::Separator();

		nodeFactory_.CreateGui();

		ImGui::End();
								  });
}

void ShaderGraphEditor::LoadGraph() {
	if (ImGui::Button("Load")) {
		editor_.reset(new ImFlow::ImNodeFlow);
		nodeFactory_.Init(editor_.get());

		std::wstring path;
		IFileOpenDialog* pFileOpen = nullptr;
		HRESULT hr = CoCreateInstance(
			CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen)
		);

		wchar_t buffer[MAX_PATH];
		GetModuleFileNameW(nullptr, buffer, MAX_PATH);

		namespace fs = std::filesystem;
		fs::path exePath = buffer;
		exePath = exePath.parent_path().parent_path().parent_path().parent_path().wstring();

		if (SUCCEEDED(hr)) {

			// ★ 初期フォルダの指定
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

		if (ConvertString(path) != "") {
			nodeFactory_.CreateGraph(ShaderGraphSerializer::Load(ConvertString(path)));
		}
	}
}

void ShaderGraphEditor::SaveGraph() {
	if (ImGui::Button("Save")) {
		ShaderGraphSerializer::Save("./Project/Packages/Game/Assets/GameData/ShaderGraph/", "test.json", editor_.get());
	}
}