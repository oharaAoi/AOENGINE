#include "ShaderGraphEditor.h"
#include <windows.h>
#include <shobjidl.h>
#include <iostream>
#include "Engine/System/ShaderGraph/Editor/ShaderGraphSerializer.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/Utilities/FileDialogFunc.h"

using namespace AOENGINE;

ShaderGraphEditor::ShaderGraphEditor() {
}

ShaderGraphEditor::~ShaderGraphEditor() {
	SaveLastPath();
	editor_.reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化関数
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::Init() {
	LoadLastPath();
	editor_ = std::make_unique<ImFlow::ImNodeFlow>();
	resultNode_ = nodeFactory_.Init(editor_.get());
	if (graphPath_ != "") {
		editor_->getNodes().clear();
		resultNode_ = nodeFactory_.CreateGraph(ShaderGraphSerializer::Load(graphPath_));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::Edit() {
	if (ImGui::Begin("Shader Graph Editor", nullptr)) {
		if (ImGui::IsWindowFocused()) {
			EditorWindows::GetInstance()->SetSelectWindow(this);;
		}

		if (graphPath_ != "") {
			ImGui::Text(graphPath_.c_str());
		}

		if (ImGui::Button("Clear")) {
			editor_->getNodes().clear();
			resultNode_ = nodeFactory_.CreateResultNode(editor_.get());
		}
		ImGui::SameLine();
		Reload();
		ImGui::SameLine();
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
	ImGui::End();
}

void ShaderGraphEditor::InspectorWindow() {
	for (auto& node : editor_->getNodes()) {
		if (node.second.get()->isSelected()) {
			node.second.get()->updateGui();
		}
	}
}

void AOENGINE::ShaderGraphEditor::HierarchyWindow() {

}

void AOENGINE::ShaderGraphEditor::ExecutionWindow() {
	Edit();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 親Nodeから子Nodeにたどるようにする
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::ExecuteFrom(ImFlow::BaseNode* node, std::unordered_set<ImFlow::BaseNode*>& visited) {
	// 訪れたNodeかどうかを確認する
	if (!node || visited.contains(node)) return;
	visited.insert(node);

	if (node->getIns().empty()) {
		node->customUpdate();
		return;
	}

	// すべての入力ピンをチェック
	for (auto& in : node->getIns()) {
		// このInPinがリンクされているなら
		auto link = in->getLink().lock();
		if (link) {
			// 左側（出力ピン）の親ノードを取得
			ImFlow::BaseNode* srcNode = link->left()->getParent();
			ExecuteFrom(srcNode, visited); // 依存ノードを先に実行
		}
	}

	node->customUpdate();
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
		popupPos_ = ImGui::GetMousePos();
		nodeFactory_.CreateGui(editor_->screen2grid(popupPos_));

								  });

	editor_->droppedLinkPopUpContent(
		[this]([[maybe_unused]] ImFlow::Pin* draggedPin) {
			ImVec2 mouse = ImGui::GetMousePos();
			ImGui::SetNextWindowPos(mouse);

			ImGui::TextUnformatted("Create Node From Link");
			ImGui::Separator();

			nodeFactory_.CreateGui(editor_->screen2grid(mouse));
		}
	);
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

void AOENGINE::ShaderGraphEditor::Reload() {
	if (ImGui::Button("Reload")) {
		if (graphPath_ != "") {
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

void AOENGINE::ShaderGraphEditor::SaveLastPath() {
	json data;
	data["lastPath"] = graphPath_;
	std::ofstream outFile(kPropertyPath_);
	if (outFile.fail()) {
		std::string message = "Faild open data file for write\n";
		//Log(message);
		return;
	}

	// -------------------------------------------------
		// ↓ ファイルに実際に書き込む
		// -------------------------------------------------
	outFile << std::setw(4) << data << std::endl;
	outFile.close();
}

void AOENGINE::ShaderGraphEditor::LoadLastPath() {
	// 読み込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読み込みように開く
	ifs.open(kPropertyPath_);

	if (ifs.fail()) {
		std::string message = "not Exist " + kPropertyPath_ + ".json";
		graphPath_ = "";
		return;
	}

	json root;
	// json文字列からjsonのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	graphPath_ = root.value("lastPath", "");
}
