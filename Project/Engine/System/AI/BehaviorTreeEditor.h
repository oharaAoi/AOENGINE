#pragma once
#include <list>
#include <memory>
#include "Engine/System/AI/Node/BaseBehaviorNode.h"
#include "Engine/System/AI/GoalOriented/IOrientedGoal.h"

/// <summary>
/// Treeを編集するためのクラス
/// </summary>
class BehaviorTreeEditor {
public: // コンストラクタ

	BehaviorTreeEditor() = default;
	~BehaviorTreeEditor();

public:

	void Finalize();

	void Init();

	/// <summary>
	/// 編集を行なう
	/// </summary>
	/// <param name="_nodeList"></param>
	/// <param name="_link"></param>
	/// <param name="_root"></param>
	/// <param name="_worldState"></param>
	/// <param name="_canTaskMap"></param>
	/// <param name="_goalArray"></param>
	void Edit(const std::string& _name, 
			  std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList,
			  std::vector<Link>& _link,
			  BaseBehaviorNode* _root,
			  IWorldState* _worldState,
			  std::unordered_map<std::string, std::shared_ptr<BaseBehaviorNode>>& _canTaskMap, 
			  const std::vector<std::shared_ptr<IOrientedGoal>>& _goalArray);

	void EditSelect();

private:

	/// <summary>
	/// 保存と読み込み
	/// </summary>
	/// <param name="_nodeList"></param>
	/// <param name="_link"></param>
	/// <param name="_root"></param>
	/// <param name="_worldState"></param>
	/// <param name="_canTaskMap"></param>
	/// <param name="_goalArray"></param>
	void SaveAndLoad(std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList,
					 std::vector<Link>& _link,
					 BaseBehaviorNode* _root,
					 IWorldState* _worldState,
					 std::unordered_map<std::string,
					 std::shared_ptr<BaseBehaviorNode>>& _canTaskMap,
					 const std::vector<std::shared_ptr<IOrientedGoal>>& _goalArray);

	/// <summary>
	/// Node生成のwindowを表示
	/// </summary>
	/// <param name="_nodeList"></param>
	/// <param name="_worldState"></param>
	/// <param name="_canTaskMap"></param>
	/// <param name="_goalArray"></param>
	void CreateNodeWindow(std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList,
						  IWorldState* _worldState,
						  std::unordered_map<std::string, std::shared_ptr<BaseBehaviorNode>>& _canTaskMap,
						  const std::vector<std::shared_ptr<IOrientedGoal>>& _goalArray);

	/// <summary>
	/// セレクトされているNodeを確認する
	/// </summary>
	/// <param name="_nodeList"></param>
	void CheckSelectNode(std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList);

	/// <summary>
	/// 削除するNodeを確認する
	/// </summary>
	/// <param name="_nodeList"></param>
	/// <param name="_root"></param>
	void CheckDeleteNode(std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList, BaseBehaviorNode* _root);

	/// <summary>
	/// 接続処理
	/// </summary>
	/// <param name="_nodeList"></param>
	/// <param name="_link"></param>
	/// <param name="_root"></param>
	void Connect(std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList, std::vector<Link>& _link, BaseBehaviorNode* _root);

	/// <summary>
	/// 接続解除処理
	/// </summary>
	/// <param name="_nodeList"></param>
	/// <param name="_link"></param>
	/// <param name="_root"></param>
	void UnConnect(std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList, std::vector<Link>& _link, BaseBehaviorNode* _root);

	/// <summary>
	/// Node描画
	/// </summary>
	/// <param name="_nodeList"></param>
	void DrawNode(std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList);

	/// <summary>
	/// Nodeの検索
	/// </summary>
	/// <param name="_nodeList"></param>
	/// <param name="_pin"></param>
	/// <returns></returns>
	BaseBehaviorNode* FindNodeFromPin(std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList, ax::NodeEditor::PinId _pin);

private:

	// nodeEditorのポインタ
	ax::NodeEditor::EditorContext* context_ = nullptr;

	BaseBehaviorNode* selectNode_;
	ax::NodeEditor::NodeId selectId_;

	ImVec2 popupPos_;
	bool popupRequested_ = false;

	bool isOpen_ = false;

};

