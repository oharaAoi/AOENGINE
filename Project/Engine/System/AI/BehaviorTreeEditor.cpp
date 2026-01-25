#include "BehaviorTreeEditor.h"
#include "Engine/System/Input/Input.h"
#include "Engine/System/AI/BehaviorTreeSerializer.h"
#include "Engine/System/AI/BehaviorTreeNodeFactory.h"
#include "Engine/System/AI/BehaviorTreeSystem.h"
#include "Engine/Utilities/FileDialogFunc.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/Utilities/Logger.h"
#include <fstream>
#include <filesystem>

using namespace AI;

BehaviorTreeEditor::~BehaviorTreeEditor() {
}

void BehaviorTreeEditor::Finalize() {
	selectNode_ = nullptr;
	ax::NodeEditor::DestroyEditor(context_);
	context_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::Init(const std::string& directoryPath, const std::string& fileName) {
	// nodeEditorの初期化
	ax::NodeEditor::Config config;
	config.NavigateButtonIndex = 2;

	// directoryPathの設定
	std::string configPath = directoryPath + "Config/";
	std::filesystem::path dire(configPath);
	if (!std::filesystem::exists(configPath)) {
		std::filesystem::create_directories(configPath);
	}
	settingsFilePath_ = configPath + "config_" + fileName;

	// 保存先を指定する
	config.SettingsFile = settingsFilePath_.c_str();
	context_ = ax::NodeEditor::CreateEditor(&config);

	// editorを作成する
	ax::NodeEditor::SetCurrentEditor(context_);
	auto& style = ax::NodeEditor::GetStyle();
	style.LinkStrength = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::Edit(const std::string& name, std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList, std::vector<Link>& link,
							  BaseBehaviorNode* root, Blackboard* worldState,
							  const std::unordered_map<std::string, ActionNode>& creators,
							  const std::vector<std::shared_ptr<IOrientedGoal>>& goalArray) {
	if (!context_) return;

	// 保存読み込み
	SaveAndLoad(nodeList, link, root, worldState, creators, goalArray);

	// node作成windowの表示
	CreateNodeWindow(nodeList, worldState, creators, goalArray);
	// コメントNodeを追加
	CreateCommentNode();

	// コメントNodeの更新
	for (auto it = commentBox_.begin(); it != commentBox_.end();) {
		if ((*it)->GetIsDelete()) {
			// 削除を行う
			it = commentBox_.erase(it);
		} else {
			(*it)->Update();
			it++;
		}
	}


	// NodeEditorの処理
	ax::NodeEditor::SetCurrentEditor(context_);
	ax::NodeEditor::Begin(name.c_str());

	// コメントの更新・描画
	CommentFrame();

	// Nodeの表示
	DrawNode(nodeList);
	// 接続処理
	Connect(nodeList, link, root);
	// 接続解除処理
	UnConnect(nodeList, link, root);

	ax::NodeEditor::End();

	// 選択しているNode確認
	CheckSelectNode(nodeList);

	// 削除するNode確認
	CheckDeleteNode(nodeList, root);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　選択されたものを編集する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::EditSelect() {
	ImGui::Begin("Inspector##BehaviorTreeNode");
	if (selectNode_) {
		selectNode_->Debug_Gui();
	}
	ImGui::End();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　commentBoxをjsonから作成する
//////////////////////////////////////////////////////////////////////////////////////////////////

void AI::BehaviorTreeEditor::CreateCommentsFromJson(const json& json) {
	commentBox_.clear();
	if (json.contains("comments")) {
		for (auto& comment : json["comments"]) {
			auto& newComment = commentBox_.emplace_back(std::make_unique<CommentBox>());
			newComment->FromJson(comment);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　commentBoxをjson化する
//////////////////////////////////////////////////////////////////////////////////////////////////

void AI::BehaviorTreeEditor::CommentsToJson(json& json) {
	for (auto& comment : commentBox_) {
		json["comments"].push_back(comment->ToJson());
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　commentBoxを作成する
//////////////////////////////////////////////////////////////////////////////////////////////////

void AI::BehaviorTreeEditor::CreateCommentNode() {
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		drag_.dragging = true;
		drag_.startScreen = ImGui::GetMousePos();
		drag_.currentScreen = drag_.startScreen;
	}

	if (drag_.dragging) {
		drag_.currentScreen = ImGui::GetMousePos();
	}

	ImVec2 screenMin(
		std::min(drag_.startScreen.x, drag_.currentScreen.x),
		std::min(drag_.startScreen.y, drag_.currentScreen.y)
	);

	ImVec2 screenMax(
		std::max(drag_.startScreen.x, drag_.currentScreen.x),
		std::max(drag_.startScreen.y, drag_.currentScreen.y)
	);

	ImVec2 canvasMin = ax::NodeEditor::ScreenToCanvas(screenMin);
	ImVec2 canvasMax = ax::NodeEditor::ScreenToCanvas(screenMax);

	if (AOENGINE::Input::GetInstance()->IsPressKey(DIK_C) && AOENGINE::Input::GetInstance()->IsPressKey(DIK_LMENU)) {
		auto& comment = commentBox_.emplace_back(std::make_unique<CommentBox>());
		comment->Init(canvasMin, canvasMax);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　commentBoxの描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void AI::BehaviorTreeEditor::CommentFrame() {
	// コメントNodeの描画
	for (auto& comment : commentBox_) {
		comment->Draw();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　保存と読み込みをする
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::SaveAndLoad(std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList, std::vector<Link>& link, BaseBehaviorNode* root,
									 Blackboard* worldState, const std::unordered_map<std::string, ActionNode>& creators,
									 const std::vector<std::shared_ptr<IOrientedGoal>>& goalArray) {
	if (ImGui::Button("save")) {
		std::string path = FileSaveDialogFunc();
		if (path != "") {
			json data = root->ToJson();
			for (auto& comment : commentBox_) {
				data["comments"].push_back(comment->ToJson());
			}

			BehaviorTreeSerializer::Save(path, root->ToJson());
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("load")) {
		std::string path = FileOpenDialogFunc();
		if (path != "") {
			AOENGINE::Logger::Log("[Create][BehaviorTree] : " + path);
			json nodeTree = BehaviorTreeSerializer::LoadToJson(path);
			BehaviorTreeNodeFactory::CreateTree(nodeTree, nodeList, link, root, worldState, creators, goalArray);

			CreateCommentsFromJson(nodeTree);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Node生成のwindowを表示
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::CreateNodeWindow(std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList, Blackboard* worldState,
										  const std::unordered_map<std::string, ActionNode>& creators,
										  const std::vector<std::shared_ptr<IOrientedGoal>>& goalArray) {

	// 毎フレーム呼ばれる更新処理の中で
	if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
		popupPos_ = ImGui::GetMousePos();
		popupRequested_ = !popupRequested_;
	}

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
			for (const auto& pair : creators) {
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
				ImVec2 mousePosInNodeEditor = ax::NodeEditor::ScreenToCanvas(ImGui::GetMousePos());
				BehaviorTreeNodeFactory::CreateNode(nodeType, createTaskName, nodeList, worldState, creators, goalArray, mousePosInNodeEditor);
				popupRequested_ = false;
			}
		}
		ImGui::EndPopup();
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 選択されているNodeのポインタを取得
///////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::CheckSelectNode(std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList) {
	// 選択中のnodeのIdを取得
	for (auto& node : nodeList) {
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

void BehaviorTreeEditor::CheckDeleteNode(std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList, BaseBehaviorNode* root) {
	// すべてのnodeの更新を走らせる
	for (auto it = nodeList.begin(); it != nodeList.end();) {
		if ((*it)->GetIsDelete()) {
			// 削除を行う
			for (auto& node : nodeList) {
				node->DeleteChild((*it).get());
			}

			if (selectNode_ == (*it).get()) {
				selectNode_ = root;
			}

			it = nodeList.erase(it);

		} else {
			(*it)->Update();
			it++;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 接続処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::Connect(std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList, std::vector<Link>& links, BaseBehaviorNode* root) {
	// 現在実行中のNodeを探索する
	BaseBehaviorNode* runningNode = nullptr;
	for (auto& node : nodeList) {
		if (node->GetState() == BehaviorStatus::Running) {
			runningNode = node.get();
		}
	}

	// 実行中のnodeまでのidを探索する
	std::vector<Link> executelinks;
	if (runningNode != nullptr) {
		ax::NodeEditor::NodeId id = runningNode->GetId();
		while (id != root->GetId()) {
			ax::NodeEditor::PinId input = runningNode->GetInput().id;
			BaseBehaviorNode* parent = nullptr;
			// inputに向かうlinkを探して親のNodeを割り出す
			for (auto& link : links) {
				if (link.to == input) {
					parent = FindNodeFromPin(nodeList, link.from);
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
	for (auto& link : links) {
		ax::NodeEditor::Link(link.id, link.from, link.to);

		// 実行中のリンクと一致していたら
		for (auto& exeLink : executelinks) {
			if (exeLink.from == link.from && exeLink.to == link.to) {
				ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_Flow, ImColor(255, 0, 0));
				// 実行中のNodeの発光処理
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
				for (const auto& node : nodeList) {
					if (node->GetInput().id == id) return node->GetInput().kind;
					if (node->GetOutput().id == id) return node->GetOutput().kind;
				}
				return ax::NodeEditor::PinKind::Input;
				};

			if (findPinKind(input) == ax::NodeEditor::PinKind::Output)
				std::swap(input, output);

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
				if (input != output) {
					BaseBehaviorNode* parent = FindNodeFromPin(nodeList, output);
					BaseBehaviorNode* child = FindNodeFromPin(nodeList, input);

					if (parent != child) {
						links.push_back({ BaseBehaviorNode::GetNextId(), input, output });

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

void BehaviorTreeEditor::UnConnect(std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList, std::vector<Link>& links, BaseBehaviorNode* root) {
	if (ax::NodeEditor::BeginDelete()) {
		ax::NodeEditor::LinkId deletedLinkId;
		while (ax::NodeEditor::QueryDeletedLink(&deletedLinkId)) {

			// Deleteキーが押されたときのみ削除
			if (AOENGINE::Input::GetInstance()->GetKey(DIK_DELETE)) {

				auto it = std::find_if(links.begin(), links.end(), [&](const Link& link) {
					return link.id == deletedLinkId;
									   });

				if (it != links.end()) {
					// 親子関係の削除を行う
					ax::NodeEditor::PinId from = it->from;
					ax::NodeEditor::PinId to = it->to;

					BaseBehaviorNode* parent = FindNodeFromPin(nodeList, from);
					BaseBehaviorNode* child = FindNodeFromPin(nodeList, to);

					if (parent && child) {
						parent->DeleteChild(child);
					}

					links.erase(it);
					root->ResetIndex();
				}
			}
		}
		ax::NodeEditor::EndDelete();
	}

	// rootNodeに複数の子がついたときの処理
	if (root->GetChildren().size() > 1) {
		auto it = std::find_if(links.begin(), links.end(), [&](const Link& link) {
			BaseBehaviorNode* firstChild = root->GetChildren().front();
			return link.from == firstChild->GetInput().id;
							   });

		// 親子関係の削除を行う
		ax::NodeEditor::PinId from = it->from;
		ax::NodeEditor::PinId to = it->to;

		BaseBehaviorNode* parent = FindNodeFromPin(nodeList, to);
		BaseBehaviorNode* child = FindNodeFromPin(nodeList, from);

		if (parent && child) {
			parent->DeleteChild(child);
		}

		links.erase(it);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTreeEditor::DrawNode(std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList) {
	for (auto& node : nodeList) {
		node->DrawNode();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ ピンを探す
///////////////////////////////////////////////////////////////////////////////////////////////

BaseBehaviorNode* BehaviorTreeEditor::FindNodeFromPin(std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList, ax::NodeEditor::PinId pin) {
	for (auto& node : nodeList) {
		if (node->GetInput().id == pin || node->GetOutput().id == pin) {
			return node.get();
		}
	}
	return nullptr;
}
