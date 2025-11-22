#include "BehaviorTree.h"
#include "Engine/Utilities/Logger.h"
#include "Engine/System/AI/Node/BehaviorRootNode.h"
#include "Engine/System/AI/BehaviorTreeSerializer.h"
#include "Engine/System/AI/BehaviorTreeNodeFactory.h"

BehaviorTree::~BehaviorTree() {
	nodeList_.clear();
	canTaskMap_.clear();
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
			Logger::Log("RootNodeが失敗を返しました");
			return false;
		}
	}
	return true;
}

void BehaviorTree::SetCanTaskMap(const std::unordered_map<std::string, std::shared_ptr<IBehaviorNode>>& _canTaskMap) {
	canTaskMap_ = _canTaskMap;
}

void BehaviorTree::AddGoal(std::shared_ptr<IOrientedGoal> _goal) {
	auto goal = goalArray_.emplace_back(std::move(_goal));
	goal->SetWorldState(worldState_);
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
	editor_.Edit(name_, nodeList_, links_, root_, worldState_, canTaskMap_, goalArray_);

	ImGui::Begin("WorldState");
	if (worldState_) {
		worldState_->Debug_Gui();
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
	Logger::Log("[Create][BehaviorTree] : " + nodeName);
	nodeList_.clear();
	if (root_ != nullptr) {
		root_->ClearChild();
	}

	// treeの生成前にloggerを作成しておく
	logger_ = std::make_unique<BehaviorTreeLogger>();
	logger_->Init(nodeName);

	// jsonからtreeの情報を読み取る
	json nodeTree = BehaviorTreeSerializer::LoadToJson(nodeName);
	root_ = nodeList_.emplace_back(BehaviorTreeNodeFactory::CreateNodeFromJson(nodeTree, nodeList_, links_, worldState_, canTaskMap_, goalArray_)).get();

	Logger::Log("--- success!");
}