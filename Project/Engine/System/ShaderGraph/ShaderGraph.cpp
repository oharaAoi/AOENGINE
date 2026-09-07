#include "ShaderGraph.h"
#include "Engine/System/ShaderGraph/Editor/ShaderGraphSerializer.h"
#include "Engine/Utilities/FileDialogFunc.h"

using namespace AOENGINE;

ShaderGraph::~ShaderGraph() {
	editor_.reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraph::Debug_Gui() {
	ImGui::Text(param_.path.c_str());
	if (!editor_) {
		return;
	}
	if (ImGui::Button("Load")) {
		// ファイルを選択
		param_.path = FileOpenDialogFunc();
		Load(param_.path);
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

void ShaderGraph::InitRuntime(const std::string& _name) {
	// RuntimeではImNodeFlowやEditor用の仮ResultNodeを生成しません。
	editor_.reset();
	resultNode_.reset();
	runtimeNodes_.clear();
	nodeFactory_.InitRuntime();
	param_.SetName(_name);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新関数
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraph::Update() {
	if (editor_) {
		editor_->get_recursion_blacklist().clear();
	}
	// ----------------------
	// ↓ node更新処理
	// ----------------------
	std::unordered_set<ImFlow::BaseNode*> visited;
	const auto updateNodes = [this, &visited](auto& nodes) {
	for (auto& [id, nodePtr] : nodes) {
		ImFlow::BaseNode* node = nodePtr.get();

		for (auto& out : node->getOuts()) {
			if (out->isConnected()) {
				continue;
			}
		}

		// 末端からたどるようにする
		ExecuteFrom(node, visited);
	}
	};

	if (editor_) {
		updateNodes(editor_->getNodes());
	} else {
		updateNodes(runtimeNodes_);
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

void ShaderGraph::Load(const std::string& _filePath) {
	if (_filePath != "") {
		editor_->getNodes().clear();
		param_.path = _filePath;
		graphData_ = std::make_shared<const json>(ShaderGraphSerializer::Load(_filePath));
		resultNode_ = nodeFactory_.CreateGraph(*graphData_);
	}
}

std::unique_ptr<ShaderGraph> ShaderGraph::CreateInstance() const {
	if (param_.path.empty() || !graphData_) {
		return nullptr;
	}

	auto instance = std::make_unique<ShaderGraph>();
	// 既存のImFlow Pin接続は所属するImNodeFlowへリンクを登録するため、
	// 現段階ではRuntimeでも所有コンテナを用意してから接続します。
	// 単体BaseNode同士をcreateLink()するとm_linksが未初期化になり、
	// ImNodeFlow::addLink()でアクセス違反になります。
	instance->Init(param_.path);
	instance->graphData_ = graphData_;
	instance->editor_->getNodes().clear();
	instance->resultNode_ = instance->nodeFactory_.CreateGraph(*graphData_);
	return instance;
}

AOENGINE::DxResource* ShaderGraph::GetResource() const {
	if (resultNode_) {
		return resultNode_->GetResultSource();
	}
	return nullptr;
}
