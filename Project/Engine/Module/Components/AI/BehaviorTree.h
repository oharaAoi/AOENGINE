#pragma once
#include <list>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "Engine/Module/Components/AI/Node/IBehaviorNode.h"
#include "Engine/Module/Components/AI/Node/BehaviorRootNode.h"
#include "Engine/Module/Components/AI/Node/ITaskNode.h"
#include "Engine/Module/Components/AI/State/IWorldState.h"
#include "Engine/Module/Components/AI/GoalOriented/IOrientedGoal.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Utilities/BehaviorTreeLogger.h"
#include "Engine/System/Manager/ImGuiManager.h"

/// <summary>
/// Objectを制御するためのクラス
/// </summary>
class BehaviorTree {
public: // コンストラクタ

	BehaviorTree() = default;
	virtual ~BehaviorTree();

public:

	// 初期化処理
	void Init();

	// 実行処理
	bool Run();

	// 編集処理
	void Edit();

	// タスクの追加
	void AddCanTask(std::shared_ptr<IBehaviorNode> _task) {
		std::string nodeName = _task->GetNodeName();
		canTaskMap_[nodeName] = std::move(_task);
	}

	/// <summary>
	/// Treeの作成
	/// </summary>
	/// <param name="nodeName"></param>
	void CreateTree(const std::string& nodeName);

	/// <summary>
	/// treeの所有者の設定
	/// </summary>
	/// <param name="_target"></param>
	void SetTarget(BaseEntity* _target) { pTarget_ = _target; }

	/// <summary>
	/// 実行設定の処理
	/// </summary>
	/// <param name="_isExecute"></param>
	void SetExecute(bool _isExecute) { isExecute_ = _isExecute; }

	/// <summary>
	/// 実行できるタスクの設定
	/// </summary>
	/// <param name="_canTaskMap"></param>
	void SetCanTaskMap(const std::unordered_map<std::string, std::shared_ptr<IBehaviorNode>>& _canTaskMap);

	/// <summary>
	/// 目標の設定
	/// </summary>
	/// <param name="_goal"></param>
	void AddGoal(std::shared_ptr<IOrientedGoal> _goal);

private:

	// 接続
	void Connect();

	// 接続解除
	void UnConnect();

	// node描画
	void DrawNode();

	// node生成Window
	void CreateNodeWindow();

	// node作成
	void CreateNode(int nodeType);

	// jsonからtreeの作成
	std::shared_ptr<IBehaviorNode> CreateNodeFromJson(const json& _json);

	// nodeの検索
	IBehaviorNode* FindNodeFromPin(ax::NodeEditor::PinId pin);

public:

	void SetName(const std::string& _name) { name_ = _name; }

	void SetWorldState(IWorldState* _worldState) { worldState_ = _worldState; }

private:

	std::string name_ = "Behavior Tree";

	IWorldState* worldState_ = nullptr;

	// treeの所有者のポインタ
	BaseEntity* pTarget_;

	// nodeEditorのポインタ
	ax::NodeEditor::EditorContext* context_ = nullptr;
	// 接続のidをまとめたコンテナ
	std::vector<Link> links_;
	// nodeのリスト
	std::list<std::shared_ptr<IBehaviorNode>> nodeList_;
	// 最上位Node
	IBehaviorNode* root_;

	// 行えるTaskをまとめた物
	std::unordered_map<std::string, std::shared_ptr<IBehaviorNode>> canTaskMap_;

	std::vector<std::shared_ptr<IOrientedGoal>> goalArray_;

	// フラグ
	bool isOpenEditor_ = false;
	bool isExecute_ = false;
	
	std::string createTaskName_;

	// ----------------------
	// ↓ imgui関連
	// ----------------------
	ImGuiWindowFlags windowFlags_;

	IBehaviorNode* selectNode_;
	ax::NodeEditor::NodeId selectId_;
	ax::NodeEditor::NodeId preSelectId_;

	// ----------------------
	// ↓ loggerポインタ
	// ----------------------

	std::unique_ptr<BehaviorTreeLogger> logger_;

};

template<typename ActionT, typename Target>
std::shared_ptr<IBehaviorNode> CreateTask(Target* target, const std::string& nodeName) {
	auto result = std::make_shared<ActionT>();
	result->SetTarget(target);
	result->SetNodeName(nodeName);
	return (result);
}