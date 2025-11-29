#include "ShaderGraph.h"
#include "Engine/System/ShaderGraph/Editor/ShaderGraphSerializer.h"
#include "Engine/Utilities/FileDialogFunc.h"

ShaderGraph::~ShaderGraph() {
	editor_.reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraph::Debug_Gui() {
	ImGui::Text(param_.path.c_str());
	if (ImGui::Button("Load")) {
		// ファイルを選択
		param_.path = FileOpenDialogFunc();
		if (param_.path != "") {
			editor_->getNodes().clear();
			resultNode_ = nodeFactory_.CreateGraph(ShaderGraphSerializer::Load(param_.path));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化関数
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraph::Init(const std::string& _name) {
	editor_ = std::make_unique<ImFlow::ImNodeFlow>();
	resultNode_ = nodeFactory_.Init(editor_.get());

	param_.SetName(_name);
	param_.Load();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新関数
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraph::Update() {
	editor_->get_recursion_blacklist().clear();
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
}

void ShaderGraph::ExecuteFrom(ImFlow::BaseNode* node, std::unordered_set<ImFlow::BaseNode*>& visited) {
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
