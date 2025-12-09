#include "BehaviorTreeEditor.h"
#include "Engine/System/Input/Input.h"
#include "Engine/System/AI/BehaviorTreeSerializer.h"
#include "Engine/System/AI/BehaviorTreeNodeFactory.h"
#include "Engine/System/AI/BehaviorTreeSystem.h"
#include "Engine/Utilities/FileDialogFunc.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

using namespace AI;

BehaviorTreeEditor::~BehaviorTreeEditor() {
	ax::NodeEditor::DestroyEditor(context_);
	context_ = nullptr;
}

void BehaviorTreeEditor::Finalize() {
	selectNode_ = nullptr;
}

void BehaviorTreeEditor::Init() {
	// nodeEditorの初期化
	context_ = ax::NodeEditor::CreateEditor();
	ax::NodeEditor::SetCurrentEditor(context_);
	auto& style = ax::NodeEditor::GetStyle();
	style.LinkStrength = 0.0f;

	isOpen_ = true;
}

void BehaviorTreeEditor::Edit(const std::string& _name, std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList, std::vector<Link>& _link,
							  BaseBehaviorNode* _root,  Blackboard* _worldState, 
							  std::unordered_map<std::string, std::shared_ptr<BaseBehaviorNode>>& _canTaskMap,
							  const std::vector<std::shared_ptr<IOrientedGoal>>& _goalArray) {
	if (!context_) return;

	if (isOpen_) {
		// 保存読み込み
		SaveAndLoad(_nodeList, _link, _root, _worldState, _canTaskMap, _goalArray);

		// node作成windowの表示
		CreateNodeWindow(_nodeList, _worldState, _canTaskMap, _goalArray);

		// --- Middle Mouse (wheel) drag to pan ---
		ImGuiIO& io = ImGui::GetIO();

		if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
			io.MouseDown[ImGuiMouseButton_Right] = false;
		}

		// ▼▼ 2. 中ボタン（ホイール）ドラッグをパンとして扱う ▼▼
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
			// 中ボタン中は右クリックが押されていることにする（パン発動）
			io.MouseDown[ImGuiMouseButton_Right] = true;
		}

		// ▼▼ 3. 中ボタンを離したら偽装右クリックを戻す ▼▼
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
			io.MouseDown[ImGuiMouseButton_Right] = false;
		}


		// NodeEditorの処理
		ax::NodeEditor::SetCurrentEditor(context_);
		ax::NodeEditor::Begin(_name.c_str());

		DrawNode(_nodeList);

		Connect(_nodeList, _link, _root);

		UnConnect(_nodeList, _link, _root);

		ax::NodeEditor::End();

		// 選択しているNode確認
		CheckSelectNode(_nodeList);

		// 削除するNode確認
		CheckDeleteNode(_nodeList, _root);
	}
}

void BehaviorTreeEditor::EditSelect() {
	ImGui::Begin("Inspector##BehaviorTreeNode");
	if (selectNode_) {
		selectNode_->Debug_Gui();
	}
	ImGui::End();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　保存と読み込みをする
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::SaveAndLoad(std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList, std::vector<Link>& _link, BaseBehaviorNode* _root,
									 Blackboard* _worldState, std::unordered_map<std::string, std::shared_ptr<BaseBehaviorNode>>& _canTaskMap,
									 const std::vector<std::shared_ptr<IOrientedGoal>>& _goalArray) {
	if (ImGui::Button("save")) {
		std::string path = FileSaveDialogFunc();
		if (path != "") {
			BehaviorTreeSerializer::Save(path, _root->ToJson());
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("load")) {
		std::string path = FileOpenDialogFunc();
		if (path != "") {
			BehaviorTreeNodeFactory::CreateTree(path, _nodeList, _link, _root, _worldState, _canTaskMap, _goalArray);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Node生成のwindowを表示
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::CreateNodeWindow(std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList, Blackboard* _worldState,
										  std::unordered_map<std::string, std::shared_ptr<BaseBehaviorNode>>& _canTaskMap,
										  const std::vector<std::shared_ptr<IOrientedGoal>>& _goalArray) {
	// 毎フレーム呼ばれる更新処理の中で
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		popupPos_ = ImGui::GetMousePos();
		popupRequested_ = !popupRequested_;
	}

	if (!popupRequested_) return;

	static std::string createTaskName = "";

	if (ImGui::BeginPopupContextWindow("NodeContextMenu", ImGuiPopupFlags_MouseButtonRight)) {
		static std::string name = "node ";
		if (!InputTextWithString("nodeの名前", "##createNode", name)) {
			assert("名前が入力できません");
		}

		static int nodeType = 1;
		ImGui::Combo("##type", &nodeType, "Root\0Sequence\0Selector\0WeightSelector\0Task\0Planner\0PlannerSelector\0Condition\0Parallel");

		// taskを生成しようとしていたら生成するtaskの名前を選ぶ
		if (nodeType == (int)NodeType::Task) {
			std::vector<std::string> typeNames;
			for (const auto& pair : _canTaskMap) {
				typeNames.push_back(pair.first);
			}

			static int selectedIndex = 0;
			int changedIndex = ContainerOfComb(typeNames, selectedIndex, "Task Type");

			if (changedIndex != -1) {
				createTaskName = typeNames[changedIndex];
			}

			// nameが空だったら
			if (createTaskName == "") {
				if (!typeNames.empty()) {
					createTaskName = typeNames[0];
				}
			}
		}

		if (nodeType != (int)NodeType::Root) {
			if (ImGui::Button("Create Node")) {
				BehaviorTreeNodeFactory::CreateNode(nodeType, createTaskName, _nodeList, _worldState, _canTaskMap, _goalArray);
				popupRequested_ = false;
			}
		}
		ImGui::EndPopup();
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 選択されているNodeのポインタを取得
///////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::CheckSelectNode(std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList) {
	// 選択中の編集を表示
	if (selectNode_ != nullptr) {
		selectNode_->Debug_Gui();
	}

	// 選択中のnodeのIdを取得
	for (auto& node : _nodeList) {
		if (node->IsSelectNode()) {
			if (selectId_ != node->GetId()) {
				selectNode_ = node.get();
				selectId_ = selectNode_->GetId();
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Nodeを削除する処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::CheckDeleteNode(std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList, BaseBehaviorNode* _root) {
	// すべてのnodeの更新を走らせる
	for (auto it = _nodeList.begin(); it != _nodeList.end();) {
		if ((*it)->GetIsDelete()) {
			// 削除を行う
			for (auto& node : _nodeList) {
				node->DeleteChild((*it).get());
			}

			if (selectNode_ == (*it).get()) {
				selectNode_ = _root;
			}

			it = _nodeList.erase(it);

		} else {
			(*it)->Update();
			it++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 接続処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::Connect(std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList, std::vector<Link>& _link, BaseBehaviorNode* _root) {
	// 現在実行中のNodeを探索する
	BaseBehaviorNode* runningNode = nullptr;
	for (auto& node : _nodeList) {
		if (node->GetState() == BehaviorStatus::Running) {
			runningNode = node.get();
		}
	}

	// 実行中のnodeまでのidを探索する
	std::vector<Link> executelinks;
	if (runningNode != nullptr) {
		ax::NodeEditor::NodeId id = runningNode->GetId();
		while (id != _root->GetId()) {
			ax::NodeEditor::PinId input = runningNode->GetInput().id;
			BaseBehaviorNode* parent = nullptr;
			// inputに向かうlinkを探して親のNodeを割り出す
			for (auto& link : _link) {
				if (link.to == input) {
					parent = FindNodeFromPin(_nodeList, link.from);
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
	for (auto& link : _link) {
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
				for (const auto& node : _nodeList) {
					if (node->GetInput().id == id) return node->GetInput().kind;
					if (node->GetOutput().id == id) return node->GetOutput().kind;
				}
				return ax::NodeEditor::PinKind::Input;
				};

			if (findPinKind(input) == ax::NodeEditor::PinKind::Output)
				std::swap(input, output);

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
				if (input != output) {
					BaseBehaviorNode* parent = FindNodeFromPin(_nodeList, output);
					BaseBehaviorNode* child = FindNodeFromPin(_nodeList, input);

					if (parent != child) {
						_link.push_back({ BaseBehaviorNode::GetNextId(), input, output });

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

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 接続解除処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::UnConnect(std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList, std::vector<Link>& _link, BaseBehaviorNode* _root) {
	if (ax::NodeEditor::BeginDelete()) {
		ax::NodeEditor::LinkId deletedLinkId;
		while (ax::NodeEditor::QueryDeletedLink(&deletedLinkId)) {

			// Deleteキーが押されたときのみ削除
			if (AOENGINE::Input::GetInstance()->GetKey(DIK_DELETE)) {

				auto it = std::find_if(_link.begin(), _link.end(), [&](const Link& link) {
					return link.id == deletedLinkId;
									   });

				if (it != _link.end()) {
					// 親子関係の削除を行う
					ax::NodeEditor::PinId from = it->from;
					ax::NodeEditor::PinId to = it->to;

					BaseBehaviorNode* parent = FindNodeFromPin(_nodeList, from);
					BaseBehaviorNode* child = FindNodeFromPin(_nodeList, to);

					if (parent && child) {
						parent->DeleteChild(child);
					}

					_link.erase(it);
					_root->ResetIndex();
				}
			}
		}
		ax::NodeEditor::EndDelete();
	}

	// rootNodeに複数の子がついたときの処理
	if (_root->GetChildren().size() > 1) {
		auto it = std::find_if(_link.begin(), _link.end(), [&](const Link& link) {
			BaseBehaviorNode* firstChild = _root->GetChildren().front();
			return link.from == firstChild->GetInput().id;
							   });

		// 親子関係の削除を行う
		ax::NodeEditor::PinId from = it->from;
		ax::NodeEditor::PinId to = it->to;

		BaseBehaviorNode* parent = FindNodeFromPin(_nodeList, to);
		BaseBehaviorNode* child = FindNodeFromPin(_nodeList, from);

		if (parent && child) {
			parent->DeleteChild(child);
		}

		_link.erase(it);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::DrawNode(std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList) {
	for (auto& node : _nodeList) {
		node->DrawNode();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ ピンを探す
///////////////////////////////////////////////////////////////////////////////////////////////

BaseBehaviorNode* BehaviorTreeEditor::FindNodeFromPin(std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList, ax::NodeEditor::PinId _pin) {
	for (auto& node : _nodeList) {
		if (node->GetInput().id == _pin || node->GetOutput().id == _pin) {
			return node.get();
		}
	}
	return nullptr;
}
