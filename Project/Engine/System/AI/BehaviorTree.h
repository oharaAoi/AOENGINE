#pragma once
#include <list>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "Engine/System/AI/BehaviorTreeEditor.h"
#include "Engine/System/AI/Node/BaseBehaviorNode.h"
#include "Engine/System/AI/Node/BehaviorRootNode.h"
#include "Engine/System/AI/Node/ITaskNode.h"
#include "Engine/System/AI/State/IWorldState.h"
#include "Engine/System/AI/GoalOriented/IOrientedGoal.h"
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
	~BehaviorTree();

public:

	// 初期化処理
	void Init();

	// 実行処理
	bool Run();

	// 編集処理
	void Edit();

	// 選択したものを編集
	void EditSelect();

	// タスクの追加
	void AddCanTask(std::shared_ptr<BaseBehaviorNode> _task) {
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
	void SetCanTaskMap(const std::unordered_map<std::string, std::shared_ptr<BaseBehaviorNode>>& _canTaskMap);

	/// <summary>
	/// 目標の設定
	/// </summary>
	/// <param name="_goal"></param>
	void AddGoal(std::shared_ptr<IOrientedGoal> _goal);

	/// <summary>
	/// 現在の状態を表示する
	/// </summary>
	/// <param name="ownerWorldPos">: Treeの所有者のワールド座標</param>
	void DisplayState(const Matrix4x4& ownerWorldPos);

	BaseBehaviorNode* GetRootNode() const { return root_; }

private:

	// jsonからtreeの作成
	std::shared_ptr<BaseBehaviorNode> CreateNodeFromJson(const json& _json);

public:

	void SetName(const std::string& _name) { name_ = _name; }
	const std::string& GetName() const { return name_; }

	void SetWorldState(IWorldState* _worldState) { worldState_ = _worldState; }

private:

	std::string name_ = "Behavior Tree";

	IWorldState* worldState_ = nullptr;

	// treeの所有者のポインタ
	BaseEntity* pTarget_;
	// 接続のidをまとめたコンテナ
	std::vector<Link> links_;
	// nodeのリスト
	std::list<std::shared_ptr<BaseBehaviorNode>> nodeList_;
	// 最上位Node
	BaseBehaviorNode* root_;

	// 行えるTaskをまとめた物
	std::unordered_map<std::string, std::shared_ptr<BaseBehaviorNode>> canTaskMap_;

	std::vector<std::shared_ptr<IOrientedGoal>> goalArray_;

	// フラグ
	bool isExecute_ = false;

	BehaviorTreeEditor editor_;

	// ----------------------
	// ↓ loggerポインタ
	// ----------------------

	std::unique_ptr<BehaviorTreeLogger> logger_;

};

template<typename ActionT, typename Target>
std::shared_ptr<BaseBehaviorNode> CreateTask(Target* target, const std::string& nodeName) {
	auto result = std::make_shared<ActionT>();
	result->SetTarget(target);
	result->SetNodeName(nodeName);
	return (result);
}