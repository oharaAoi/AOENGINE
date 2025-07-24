#include "BehaviorTree.h"
#include <cassert>
#include "Engine/Utilities/Logger.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/Module/Components/AI/SequenceNode.h"
#include "Engine/Module/Components/AI/SelectorNode.h"
#include "Engine/Module/Components/AI/WeightSelectorNode.h"
#include "Engine/System/Input/Input.h"
#include "Engine/Module/Components/AI/BehaviorTreeSerializer.h"
#include <fstream>

BehaviorTree::~BehaviorTree() {
	nodeList_.clear();
	canTaskMap_.clear();
	ax::NodeEditor::DestroyEditor(context_);
	context_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::Init() {
	context_ = ax::NodeEditor::CreateEditor();
	ax::NodeEditor::SetCurrentEditor(context_);
	auto& style = ax::NodeEditor::GetStyle();
	style.LinkStrength = 0.0f;

	isOpenEditor_ = false;
	isExecute_ = true;

	windowFlags_ = ImGuiWindowFlags_None;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Tree処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::Run() {
	if (!context_) return;
	if (root_ == nullptr) { return; }
	if (!isExecute_) { return; }

	// すべてのnodeの更新を走らせる
	for (auto it = nodeList_.begin(); it != nodeList_.end();) {
		if ((*it)->GetIsDelete()) {
			for (auto& node : nodeList_) {
				node->DeleteChild((*it).get());
			}

			if (selectNode_ == (*it).get()) {
				selectNode_ = root_;
			}

			it = nodeList_.erase(it);

		} else {
			(*it)->Update();
			it++;
		}
	}

	// nodeの内容を実行させる
	if (root_ != nullptr) {
		BehaviorStatus state = root_->Execute();
		if (state == BehaviorStatus::Failure) {
			Logger::Log("RootNodeが失敗を返しました");
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　接続処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::Connect() {
	// 現在実行中のNodeを探索する
	IBehaviorNode* runningNode = nullptr;
	for (auto& node : nodeList_) {
		if (node->GetState() == BehaviorStatus::Running) {
			runningNode = node.get();
		}
	}

	// 実行中のnodeまでのidを探索する
	std::vector<Link> executelinks;
	if (runningNode != nullptr) {
		ax::NodeEditor::NodeId id = runningNode->GetId();
		while (id != root_->GetId()) {
			ax::NodeEditor::PinId input = runningNode->GetInput().id;
			IBehaviorNode* parent = nullptr;
			// inputに向かうlinkを探して親のNodeを割り出す
			for (auto& link : links_) {
				if (link.to == input) {
					parent = FindNodeFromPin(link.from);
				}
			}

			// 親が存在していたらNodeを更新する
			if (parent != nullptr) {
				executelinks.push_back({ 0, parent->GetOutput().id, input });
				id = parent->GetId();
				runningNode = parent;
			} else {
				break;
			}
		}
	}

	// リンクを結ぶ
	for (auto& link : links_) {
		ax::NodeEditor::Link(link.id, link.from, link.to);

		// 実行中のリンクと一致していたら
		for (auto& exeLink : executelinks) {
			if (exeLink.from == link.from && exeLink.to == link.to) {
				ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_Flow, ImColor(255, 0, 0));
				ax::NodeEditor::Flow(link.id);
				ax::NodeEditor::PopStyleColor();
			}
		}
	}

	// 新たにlinkを結ぶ処理
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
					IBehaviorNode* parent = FindNodeFromPin(output);
					IBehaviorNode* child = FindNodeFromPin(input);

					if (parent != child) {
						links_.push_back({ IBehaviorNode::GetNextId(), input, output });

						if (parent && child) {
							parent->AddChild(child);
						}
					}
				}
			}
		}
	}
	ax::NodeEditor::EndCreate();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　接続解除処理
//////////////////////////////////////////////////////////////////////////////////////////////////

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

					IBehaviorNode* parent = FindNodeFromPin(from);
					IBehaviorNode* child = FindNodeFromPin(to);

					if (parent && child) {
						parent->DeleteChild(child);
					}

					links_.erase(it);
					root_->ResetIndex();
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
	ImGui::Checkbox("isExecute", &isExecute_);
	if (isOpenEditor_) {
		if (ImGui::Button("Close")) {
			isOpenEditor_ = false;
		}
	} else {
		if (ImGui::Button("Open")) {
			isOpenEditor_ = true;
		}
	}

	if (isOpenEditor_) {
		// Treeに関する処理
		if (ImGui::Begin("BehaviorTree", &isOpenEditor_, windowFlags_)) {
			ax::NodeEditor::SetCurrentEditor(context_);
			ax::NodeEditor::Begin("BehaviorTree");

			// nodeの描画
			DrawNode();

			// 接続をする
			Connect();

			// 接続解除
			UnConnect();

			ax::NodeEditor::End();
		}
		ImGui::End();

		// Editorに関する処理
		if (ImGui::Begin("BehaviorTreeEditor", &isOpenEditor_, windowFlags_)) {

			std::string loadFilePath;
			if (ButtonOpenDialog("Load Tree", "Load_Tree", "LoadTree", ".json", loadFilePath)) {
				CreateTree(loadFilePath);
			}

			std::string filePath;
			if (ButtonOpenDialog("Save Tree", "Save_Tree", "SaveTree", ".json", filePath)) {
				BehaviorTreeSerializer::Save(filePath, root_->ToJson());
			}

			if (ImGui::CollapsingHeader("Nodeの作成")) {
				CreateNodeWindow();
			}

			ImGui::Separator();
			if (selectNode_ != nullptr) {
				selectNode_->Debug_Gui();
			}

		}
		ImGui::End();
	}

	for (auto& node : nodeList_) {
		if (node->IsSelectNode()) {
			if (selectId_ != node->GetId()) {
				selectNode_ = node.get();
				selectId_ = selectNode_->GetId();
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Nodeを作成するWindowの表示
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::CreateNodeWindow() {
	ImGui::BulletText("Nodeを作成");
	static std::string name = "node ";
	if (!InputTextWithString("nodeの名前","##createNode", name)) {
		assert("名前が入力できません");
	}

	static int nodeType = 1;
	ImGui::Combo("##type", &nodeType, "Root\0Sequence\0Selector\0WeightSelector\0Task");

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

		// nameが空だったら
		if (createTaskName_ == "") {
			if (!typeNames.empty()) {
				createTaskName_ = typeNames[0];
			}
		}
	}

	if (nodeType != NodeType::Root) {
		if (ImGui::Button("Create Node")) {
			CreateNode(nodeType);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Nodeを作成
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::CreateNode(int nodeType) {
	if (nodeType == NodeType::Sequencer) {
		nodeList_.emplace_back(std::make_shared<SequenceNode>());

	} else if (nodeType == NodeType::Selector) {
		nodeList_.emplace_back(std::make_shared<SelectorNode>());

	} else if (nodeType == NodeType::WeightSelector) {
		nodeList_.emplace_back(std::make_shared<WeightSelectorNode>());

	} else if (nodeType == NodeType::Task) {
		auto& node = nodeList_.emplace_back(canTaskMap_[createTaskName_]->Clone());
		node->Init();
		node->SetPos(CVector2::ZERO);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Treeを作成
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::CreateTree(const std::string& nodeName) {
	nodeList_.clear();
	if (root_ != nullptr) {
		root_->ClearChild();
	}
	json nodeTree = BehaviorTreeSerializer::LoadToJson(nodeName);
	root_ = nodeList_.emplace_back(CreateNodeFromJson(nodeTree)).get();
	
	selectNode_ = root_;
	selectId_ = root_->GetId();
	preSelectId_ = root_->GetId();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　JsonファイルNodeを作成する
//////////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<IBehaviorNode> BehaviorTree::CreateNodeFromJson(const json& _json) {
	// nodeを作成
	std::shared_ptr<IBehaviorNode> node;
	NodeType type = static_cast<NodeType>(_json["nodeType"]);
	std::string name = _json["name"];

	// 種類によってインスタンスを変える
	switch (type) {
	case NodeType::Root: node = std::make_shared<BehaviorRootNode>(); break;
	case NodeType::Sequencer: node = std::make_shared<SequenceNode>(); break;
	case NodeType::Selector: node = std::make_shared<SelectorNode>(); break;
	case NodeType::WeightSelector: 
		node = std::make_shared<WeightSelectorNode>();
		break;
	case NodeType::Task:
		node = canTaskMap_[name]->Clone(); 
		node->Init();
		break;
	}

	// 情報をセットする
	node->SetNodeName(name);
	node->SetNodeType(type);
	node->SetPos(Vector2(_json["nodePos"]["x"], _json["nodePos"]["y"]));
	nodeList_.push_back(node);

	// 子どもがいたら再帰的に処理
	for (const auto& childJson : _json["children"]) {
		std::shared_ptr<IBehaviorNode> child = CreateNodeFromJson(childJson);
		node->AddChild(child.get());

		// nodeと子どもをリンクでつなぐ
		Link link;
		link.id = IBehaviorNode::GetNextId();
		link.from = node->GetOutput().id;
		link.to = child->GetInput().id;
		links_.push_back(link);
	}

	return node;
}

IBehaviorNode* BehaviorTree::FindNodeFromPin(ax::NodeEditor::PinId pin) {
	for (auto& node : nodeList_) {
		if (node->GetInput().id == pin || node->GetOutput().id == pin)
			return node.get();
	}
	return nullptr;
}