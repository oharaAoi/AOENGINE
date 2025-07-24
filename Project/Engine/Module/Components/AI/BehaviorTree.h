#pragma once
#include <list>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Engine/Module/Components/AI/IBehaviorNode.h"
#include "Engine/Module/Components/AI/BehaviorRootNode.h"
#include "Engine/Module/Components/AI/ITaskNode.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "imgui_node_editor.h"

/// <summary>
/// Objectを制御するためのクラス
/// </summary>
class BehaviorTree {
public:

	BehaviorTree() = default;
	virtual ~BehaviorTree();

	void Init();

	void Run();

	void Edit();

	void AddCanTask(std::shared_ptr<IBehaviorNode> _task) {
		std::string nodeName = _task->GetNodeName();
		canTaskMap_[nodeName] = std::move(_task);
	}

	void CreateTree(const std::string& nodeName);

	void SetTarget(BaseEntity* _target) { pTarget_ = _target; }

	void SetExecute(bool _isExecute) { isExecute_ = _isExecute; }

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

	std::shared_ptr<IBehaviorNode> CreateNodeFromJson(const json& _json);

	// nodeの検索
	IBehaviorNode* FindNodeFromPin(ax::NodeEditor::PinId pin);

private:

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

	// フラグ
	bool isOpenEditor_ = false;
	bool isExecute_ = false;
	
	std::string createTaskName_;

	ImGuiWindowFlags windowFlags_;

	IBehaviorNode* selectNode_;
	ax::NodeEditor::NodeId selectId_;
	ax::NodeEditor::NodeId preSelectId_;

	BaseEntity* pTarget_;
};

template<typename ActionT, typename Target>
std::shared_ptr<IBehaviorNode> CreateTask(Target* target, const std::string& nodeName) {
	auto result = std::make_shared<ActionT>();
	result->SetTarget(target);
	result->SetNodeName(nodeName);
	return (result);
}