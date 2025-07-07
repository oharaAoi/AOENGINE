#include "BehaviorTree.h"
#include <cassert>
#include "Engine/Utilities/Logger.h"
#include "Engine/Module/Components/AI/SequenceNode.h"
#include "Engine/Module/Components/AI/SelectorNode.h"
#include "Engine/System/Input/Input.h"

BehaviorTree::~BehaviorTree() {
	ax::NodeEditor::DestroyEditor(context_);
	context_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::Init() {
	context_ = ax::NodeEditor::CreateEditor();

	/*CreateNode("Root");*/
	auto& nodeC = nodeList_.emplace_back(std::make_unique<BehaviorRootNode>());
	nodeC->SetName("Root");
	root_ = nodeC.get();

	isOpenEditor_ = true;
	isOpenCreateNode_ = false;
	canDocking_ = false;

	windowFlags_ = ImGuiWindowFlags_None;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Tree処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::Run() {
	if (!context_) return;

	for (auto it = nodeList_.begin(); it != nodeList_.end();) {
		if ((*it)->GetIsDelete()) {
			it = nodeList_.erase(it);
		} else {
			(*it)->Update();
			it++;
		}
	}

	BehaviorStatus state = root_->Execute();
	if (state == BehaviorStatus::Failure) {
		Logger::Log("RootNodeが失敗を返しました");
	}

	if (isOpenEditor_) {
		if (ImGui::Begin("BehaviorTree", &isOpenEditor_, windowFlags_)) {
			DockingButton();

			ax::NodeEditor::SetCurrentEditor(context_);
			ax::NodeEditor::Begin("BehaviorTree");

			// nodeの描画
			DrawNode();

			// 接続をする
			Connect();

			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
				isOpenCreateNode_ = true;
			}

			ax::NodeEditor::End();
		}
		ImGui::End();
	}

	if (isOpenCreateNode_) {
		CreateNodeWindow();
	}
}

void BehaviorTree::Connect() {
	for (auto& link : links_) {
		ax::NodeEditor::Link(link.id, link.from, link.to);
	}

	if (ax::NodeEditor::BeginCreate()) {
		ax::NodeEditor::PinId input, output;
		if (ax::NodeEditor::QueryNewLink(&input, &output)) {
			auto findPinKind = [&](ax::NodeEditor::PinId id) -> ax::NodeEditor::PinKind {
				for (const auto& node : nodeList_) {
					if (node->GetInput().id == id) return node->GetInput().kind;
					if (node->GetOutput().id == id) return node->GetOutput().kind;
				}
				return ax::NodeEditor::PinKind::Input;
				};

			if (findPinKind(input) == ax::NodeEditor::PinKind::Output)
				std::swap(input, output);

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {

				if (input != output) {

					links_.push_back({ IBehaviorNode::GetNextId(), input, output });

					IBehaviorNode* parent = FindNodeFromPin(output);
					IBehaviorNode* child = FindNodeFromPin(input);

					if (parent && child) {
						parent->AddChild(child);
					}
				}
			}
		}
	}
	ax::NodeEditor::EndCreate();

	if (ax::NodeEditor::BeginDelete()) {
		ax::NodeEditor::LinkId deletedLinkId;
		while (ax::NodeEditor::QueryDeletedLink(&deletedLinkId)) {

			// Deleteキーが押されたときのみ削除
			if (Input::GetInstance()->GetKey(DIK_DELETE)) {

				auto it = std::find_if(links_.begin(), links_.end(), [&](const Link& link) {
					return link.id == deletedLinkId;
									   });

				if (it != links_.end()) {
					// （必要なら親子関係解除処理などをここで）

					links_.erase(it);
				}
			}
		}
		ax::NodeEditor::EndDelete();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Nodeの描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::DrawNode() {
	for (auto& node : nodeList_) {
		node->DrawNode();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::Edit() {
	if (isOpenEditor_) {
		if (ImGui::Button("Close")) {
			isOpenEditor_ = false;
			isOpenCreateNode_ = false;
		}
	} else {
		if (ImGui::Button("Open")) {
			isOpenEditor_ = true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Dockingを行うかのボタンの表示
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::DockingButton() {
	if (!canDocking_) {
		if (ImGui::Button("canDocking")) {
			canDocking_ = true;
			windowFlags_ = ImGuiWindowFlags_None;
		}
	} else {
		if (ImGui::Button("cannotDocking")) {
			canDocking_ = false;
			windowFlags_ = ImGuiWindowFlags_NoDocking;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Nodeを作成するWindowの表示
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::CreateNodeWindow() {
	ImGuiWindowFlags window_Flags = ImGuiWindowFlags_NoDocking;
	ImGui::Begin("CreateNode", &isOpenCreateNode_, window_Flags);
	ImGui::BulletText("Nodeを作成");
	static std::string name = "node ";
	if (!InputTextWithString("nodeの名前", name)) {
		assert("名前が入力できません");
	}

	static int nodeType = 0;
	ImGui::Combo("##type", &nodeType, "Sequence\0Selector\0Task");

	if (ImGui::Button("Create Node")) {
		CreateNode(name, nodeType);
	}

	ImGui::End();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Nodeを作成
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::CreateNode(const std::string& nodeName, int nodeType) {
	if (nodeType == NodeType::Sequencer) {
		nodeList_.emplace_back(std::make_unique<SequenceNode>());

	} else if (nodeType == NodeType::Selector) {
		nodeList_.emplace_back(std::make_unique<SelectorNode>());

	} else if (nodeType == NodeType::Task) {
		auto& nodeC = nodeList_.emplace_back(std::make_unique<SequenceNode>());
		nodeC->SetName(nodeName);
	}
}

IBehaviorNode* BehaviorTree::FindNodeFromPin(ax::NodeEditor::PinId pin) {
	for (auto& node : nodeList_) {
		if (node->GetInput().id == pin || node->GetOutput().id == pin)
			return node.get();
	}
	return nullptr;
}

bool BehaviorTree::InputTextWithString(const char* label, std::string& str, size_t maxLength) {
	// std::vector<char> をバッファとして使用
	std::vector<char> buffer(str.begin(), str.end());
	buffer.resize(maxLength); // 必要なサイズにリサイズ

	// ImGui入力フィールド
	bool changed = ImGui::InputText(label, buffer.data(), buffer.size());

	if (changed) {
		str = buffer.data(); // 入力された文字列をstd::stringに反映
	}

	return changed;
}