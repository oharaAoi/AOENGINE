#pragma once
#include <string>
#include <vector>
#include <memory>
#include "imgui_node_editor.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Json/IJsonConverter.h"

struct Pin {
	ax::NodeEditor::PinId id;
	ax::NodeEditor::PinKind kind;
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

struct NodeItems {
	std::string nodeName;
	int nodeType;
	Vector2 nodePos;
	std::vector<std::string> childName;
};

enum BehaviorStatus {
	Inactive,	// 非アクティブ
	Success,	// 成功
	Failure,	// 失敗
	Running		// 実行中
};

enum NodeType {
	Root,
	Sequencer,
	Selector,
	Task,
};

/// <summary>
/// 各Nodeで共通となる基礎クラス
/// </summary>
class IBehaviorNode :
	public AttributeGui {
public:

	IBehaviorNode();
	virtual ~IBehaviorNode() = default;
	// コピーコンストラクタ
	virtual std::shared_ptr<IBehaviorNode> Clone() const = 0;

	void Init();

	void Update();

	void DrawNode();

	virtual BehaviorStatus Execute() = 0;

	void AddChild(IBehaviorNode* child);

	void DeleteChild(IBehaviorNode* child);

	json ToJson();

	virtual void Debug_Gui() override {};

	bool IsSelectNode();

public:

	ax::NodeEditor::NodeId GetId() { return node_.id; }

	static uint32_t GetNextId();

	const Pin& GetInput() const { return node_.inputId; }
	const Pin& GetOutput() const { return node_.outputId; }

	void SetNodeName(const std::string& _name) { node_.name = _name; }
	const std::string& GetNodeName() { return node_.name; }

	void SetNodeType(NodeType _type) { type_ = _type; }

	bool GetIsDelete() const { return isDelete_; }

	void SetPos(const Vector2& _pos) { 
		setNodePos_ = false;
		pos_ = _pos;
	}
	Vector2 GetPos() { return pos_; }

	const std::vector<IBehaviorNode*>& GetChildren() const { return children_; }

private:

	// NodeにLineを描画
	void DrawImGuiLine(const ImVec2& _texPos);

protected:

	static uint32_t nextSerialNumber_;	// 次のユニークid

	NodeType type_;
	BehaviorStatus state_;	// nodeの状態

	Node node_;		// node本体

	std::vector<IBehaviorNode*> children_;	// 子ども
	uint32_t currentIndex_;					// 現在参照している子のindex

	bool isLeafNode_ = false;		// リーフノードかどうか

	Vector2 pos_;		// Nodeの座標
	bool setNodePos_;	// Node座標の設定を行ったかどうか

	// -------------------------------------------------
	// ↓ Debug用
	// -------------------------------------------------

	// 編集フラグ
	bool isSelect_;
	bool isDelete_;

	ImColor color_;
	ImColor baseColor_;

};

