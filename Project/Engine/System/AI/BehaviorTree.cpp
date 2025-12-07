#include "BehaviorTree.h"
#include "Engine/Utilities/Logger.h"
#include "Engine/System/AI/Node/BehaviorRootNode.h"
#include "Engine/System/AI/BehaviorTreeSerializer.h"
#include "Engine/System/AI/BehaviorTreeNodeFactory.h"

using namespace AOENGINE;

BehaviorTree::~BehaviorTree() {
	nodeList_.clear();
	canTaskMap_.clear();
	editor_.Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::Init() {
	editor_.Init();

	isExecute_ = true;

	root_ = nodeList_.emplace_back(std::make_shared<BehaviorRootNode>()).get();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Tree処理
//////////////////////////////////////////////////////////////////////////////////////////////////

bool BehaviorTree::Run() {
	if (root_ == nullptr) { return false; }

	// すべてのnodeの更新を走らせる
	for (auto it = nodeList_.begin(); it != nodeList_.end();) {
		(*it)->Update();
		it++;
	}

	if (!isExecute_) { return false; }
	// nodeの内容を実行させる
	if (root_ != nullptr) {
		BehaviorStatus state = root_->Execute();
		if (state == BehaviorStatus::Failure) {
			AOENGINE::Logger::Log("RootNodeが失敗を返しました");
			return false;
		}
	}
	return true;
}

void BehaviorTree::SetCanTaskMap(const std::unordered_map<std::string, std::shared_ptr<BaseBehaviorNode>>& _canTaskMap) {
	canTaskMap_ = _canTaskMap;
}

void BehaviorTree::AddGoal(std::shared_ptr<IOrientedGoal> _goal) {
	auto goal = goalArray_.emplace_back(std::move(_goal));
	goal->SetBlackboard(blackboard_);
}

void BehaviorTree::DisplayState(const Math::Matrix4x4& ownerWorldPos) {
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoBackground;
	Math::Vector2 screenPos = WorldToScreenCoordinate(ownerWorldPos, AOENGINE::Render::GetVpvpMatrix());
	ImGui::SetNextWindowPos(ImVec2(screenPos.x, screenPos.y), ImGuiCond_Always);
	if (ImGui::Begin("TreeRunName", nullptr, flags)) {
		ImGui::Text("行動 : %s", root_->GetCurrentRunNodeName().c_str());
	}
	ImGui::End();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::Edit() {
	// Treeに関する処理
	ImGui::Text("isExecute");
	ImGui::SameLine();
	ImGui::Checkbox("##isExecute", &isExecute_);
	ImGui::SameLine();
	editor_.Edit(name_, nodeList_, links_, root_, blackboard_, canTaskMap_, goalArray_);

	ImGui::Begin("Blackboard");
	if (blackboard_) {
		blackboard_->Debug_Gui();
	}
	ImGui::End();
}

void BehaviorTree::EditSelect() {
	editor_.EditSelect();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Treeを作成
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::CreateTree(const std::string& nodeName) {
	AOENGINE::Logger::Log("[Create][BehaviorTree] : " + nodeName);
	nodeList_.clear();
	if (root_ != nullptr) {
		root_->ClearChild();
	}

	// treeの生成前にloggerを作成しておく
	logger_ = std::make_unique<BehaviorTreeLogger>();
	logger_->Init(nodeName);

	// jsonからtreeの情報を読み取る
	json nodeTree = BehaviorTreeSerializer::LoadToJson(nodeName);
	root_ = nodeList_.emplace_back(BehaviorTreeNodeFactory::CreateNodeFromJson(nodeTree, nodeList_, links_, blackboard_, canTaskMap_, goalArray_)).get();

	AOENGINE::Logger::Log("--- success!");
}