#pragma once
#include <string>
#include <vector>
#include <memory>
#include "imgui_node_editor.h"

struct Pin {
	ax::NodeEditor::PinId id;
	ax::NodeEditor::PinKind kind; // ← これを追加！
};

struct Node {
	ax::NodeEditor::NodeId id;
	Pin inputId;
	Pin outputId;
	std::string name;
};

struct Link {
	ax::NodeEditor::LinkId id;
	ax::NodeEditor::PinId  from;
	ax::NodeEditor::PinId  to;
};

enum BehaviorStatus {
	Inactive,	// 非アクティブ
	Success,	// 成功
	Failure,	// 失敗
	Running		// 実行中
};

/// <summary>
/// 各Nodeで共通となる基礎クラス
/// </summary>
class IBehaviorNode {
public:

	IBehaviorNode();
	virtual ~IBehaviorNode() = default;

	void Update();

	void DrawNode();

	virtual BehaviorStatus Execute() = 0;

	void AddChild(IBehaviorNode* child);

	void DeleteChild(IBehaviorNode* child);

public:


	static uint32_t GetNextId();

	const Pin& GetInput() const { return node_.inputId; }
	const Pin& GetOutput() const { return node_.outputId; }

	void SetNodeName(const std::string& _name) { node_.name = _name; }
	const std::string& GetNodeName() { return node_.name; }

	bool GetIsDelete() const { return isDelete_; }

	const std::vector<IBehaviorNode*>& GetChildren() const { return children_; }

private:

	// NodeにLineを描画
	void DrawImGuiLine(const ImVec2& _texPos);

protected:

	static uint32_t nextSerialNumber_;	// 次のユニークid

	BehaviorStatus state_;	// nodeの状態

	Node node_;		// node本体

	std::vector<IBehaviorNode*> children_;	// 子ども
	uint32_t currentIndex_;

	bool isLeafNode_ = false;

	// -------------------------------------------------
	// ↓ Debug用
	// -------------------------------------------------

	// 編集フラグ
	bool isSelect_;
	bool isDelete_;

	ImVec4 color_;

};

