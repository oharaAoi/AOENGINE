#include "BehaviorTree.h"
#include <cassert>
#include "Engine/Utilities/Logger.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"
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
	auto& node = nodeList_.emplace_back(std::make_unique<BehaviorRootNode>());
	root_ = node.get();

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

	// すべてのnodeの更新を走らせる
	for (auto it = nodeList_.begin(); it != nodeList_.end();) {
		if ((*it)->GetIsDelete()) {
			it = nodeList_.erase(it);
		} else {
			(*it)->Update();
			it++;
		}
	}

	// nodeの内容を実行させる
	BehaviorStatus state = root_->Execute();
	if (state == BehaviorStatus::Failure) {
		Logger::Log("RootNodeが失敗を返しました");
	}

	// NodeEditorに関する処理
	if (isOpenEditor_) {
		if (ImGui::Begin("BehaviorTree", &isOpenEditor_, windowFlags_)) {
			DockingButton();

			ax::NodeEditor::SetCurrentEditor(context_);
			ax::NodeEditor::Begin("BehaviorTree");

			// nodeの描画
			DrawNode();

			// 接続をする
			Connect();

			// 接続解除
			UnConnect();

			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
				isOpenCreateNode_ = true;
			}

			ax::NodeEditor::End();
		}
		ImGui::End();
	}

	// Node作成windowの表示
	if (isOpenCreateNode_) {
		CreateNodeWindow();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　接続処理
//////////////////////////////////////////////////////////////////////////////////////////////////

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
}

void BehaviorTree::UnConnect() {
	if (ax::NodeEditor::BeginDelete()) {
		ax::NodeEditor::LinkId deletedLinkId;
		while (ax::NodeEditor::QueryDeletedLink(&deletedLinkId)) {

			// Deleteキーが押されたときのみ削除
			if (Input::GetInstance()->GetKey(DIK_DELETE)) {

				auto it = std::find_if(links_.begin(), links_.end(), [&](const Link& link) {
					return link.id == deletedLinkId;
									   });

				if (it != links_.end()) {
					// 親子関係の削除を行う
					ax::NodeEditor::PinId from = it->from;
					ax::NodeEditor::PinId to = it->to;

					IBehaviorNode* parent = FindNodeFromPin(to);
					IBehaviorNode* child = FindNodeFromPin(from);

					if (parent && child) {
						parent->DeleteChild(child);
					}

					links_.erase(it);
				}
			}
		}
		ax::NodeEditor::EndDelete();
	}

	// rootNodeに複数の子がついたときの処理
	if (root_->GetChildren().size() > 1) {
		auto it = std::find_if(links_.begin(), links_.end(), [&](const Link& link) {
			IBehaviorNode* firstChild = root_->GetChildren().front();
			return link.from == firstChild->GetInput().id;
							   });

		// 親子関係の削除を行う
		ax::NodeEditor::PinId from = it->from;
		ax::NodeEditor::PinId to = it->to;

		IBehaviorNode* parent = FindNodeFromPin(to);
		IBehaviorNode* child = FindNodeFromPin(from);

		if (parent && child) {
			parent->DeleteChild(child);
		}

		links_.erase(it);
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

	// taskを生成しようとしていたら生成するtaskの名前を選ぶ
	if (nodeType == NodeType::Task) {
		std::vector<std::string> typeNames;
		for (const auto& pair : canTaskMap_) {
			typeNames.push_back(pair.first);
		}

		static int selectedIndex = 0;
		int changedIndex = ContainerOfComb(typeNames, selectedIndex, "Task Type");

		if (changedIndex != -1) {
			createTaskName_ = typeNames[changedIndex];
		}
	}

	if (ImGui::Button("Create Node")) {
		CreateNode(nodeType);
	}

	ImGui::End();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Nodeを作成
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::CreateNode(int nodeType) {
	if (nodeType == NodeType::Sequencer) {
		nodeList_.emplace_back(std::make_shared<SequenceNode>());

	} else if (nodeType == NodeType::Selector) {
		nodeList_.emplace_back(std::make_shared<SelectorNode>());

	} else if (nodeType == NodeType::Task) {
		nodeList_.push_back(canTaskMap_[createTaskName_]);
	}
}

IBehaviorNode* BehaviorTree::FindNodeFromPin(ax::NodeEditor::PinId pin) {
	for (auto& node : nodeList_) {
		if (node->GetInput().id == pin || node->GetOutput().id == pin)
			return node.get();
	}
	return nullptr;
}