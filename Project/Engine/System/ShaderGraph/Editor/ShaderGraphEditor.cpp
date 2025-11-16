#include "ShaderGraphEditor.h"
#include <windows.h>
#include <shobjidl.h>
#include <iostream>
#include "Engine/System/ShaderGraph/Editor/ShaderGraphSerializer.h"
#include "Engine/Utilities/FileDialogFunc.h"

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
	resultNode_ = nodeFactory_.Init(editor_.get());

	graphPath_ = "";
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
	ImGui::Text(graphPath_.c_str());
	OverwriteGraph();
	ImGui::SameLine();
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
		// ファイルを選択
		graphPath_ = FileOpenDialogFunc();
		if (graphPath_ != "") {
			editor_->getNodes().clear();
			resultNode_ = nodeFactory_.CreateGraph(ShaderGraphSerializer::Load(graphPath_));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Graphの上書き保存
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::OverwriteGraph() {
	if (ImGui::Button("Overwrite")) {
		if (graphPath_ != "") {
			ShaderGraphSerializer::Save(graphPath_, editor_.get());
			editor_->getNodes().clear();
			resultNode_ = nodeFactory_.CreateGraph(ShaderGraphSerializer::Load(graphPath_));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Graphを保存する
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::SaveGraph() {
	if (ImGui::Button("Save")) {
		std::string path = FileSaveDialogFunc();

		if (path != "") {
			graphPath_ = path;
			ShaderGraphSerializer::Save(path, editor_.get());
		}
	}
}

