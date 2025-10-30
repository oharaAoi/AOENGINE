#pragma once
#include <string>
#include <vector>
#include <memory>
#include "imgui_node_editor.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/AI/State/IWorldState.h"
#include "Engine/Utilities/BehaviorTreeLogger.h"
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
	WeightSelector,
	Task,
	Planner,
	PlannerSelector,
};

/// <summary>
/// 各Nodeで共通となる基礎クラス
/// </summary>
class IBehaviorNode :
	public AttributeGui {
public: // コンストラクタ

	IBehaviorNode();
	virtual ~IBehaviorNode() = default;
	// コピーコンストラクタ
	virtual std::shared_ptr<IBehaviorNode> Clone() const = 0;

public:

	// 初期化処理
	void Init();

	// 更新処理
	void Update();

	// nodeの描画
	void DrawNode();

	// indexをリセットする
	void ResetIndex();

public:

	// 実行関数
	virtual BehaviorStatus Execute() = 0;

	// 評価値の算出を行う
	virtual float EvaluateWeight() = 0;

	// 子の追加
	void AddChild(IBehaviorNode* child);

	// 子の削除
	void DeleteChild(IBehaviorNode* child);

	// 子を削除する
	void ClearChild();

	// json形式への変換
	virtual json ToJson() = 0;

	// 保存項目を適応する
	virtual void FromJson(const json& _jsonData);

	// 編集関数
	virtual void Debug_Gui() override {};

	// 現在選択されているNodeかどうか
	bool IsSelectNode();

	// Nodeの名前をつなぐ
	std::string NodeNameCombination();

public:

	ax::NodeEditor::NodeId GetId() { return node_.id; }

	static uint32_t GetNextId();

	const Pin& GetInput() const { return node_.inputId; }
	const Pin& GetOutput() const { return node_.outputId; }

	void SetNodeName(const std::string& _name) {
		node_.name = _name;
		name_ = _name;
	}
	const std::string& GetNodeName() { return node_.name; }

	void EditNodeName();

	void SetNodeType(NodeType _type) { type_ = _type; }

	void SetState(BehaviorStatus state) { state_ = state; }
	BehaviorStatus GetState() const { return state_; }

	bool GetIsDelete() const { return isDelete_; }

	void SetPos(const Vector2& _pos) {
		setNodePos_ = false;
		pos_ = _pos;
	}
	Vector2 GetPos() { return pos_; }

	const std::vector<IBehaviorNode*>& GetChildren() const { return children_; }

	void SetWorldState(IWorldState* _worldState) { worldState_ = _worldState; }

	void SetLogger(BehaviorTreeLogger* _logger) { pLogger_ = _logger; }

private:

	/// <summary>
	/// NodeにLineを描画
	/// </summary>
	/// <param name="_texPos"></param>
	void DrawImGuiLine(const ImVec2& _texPos);

protected:

	static uint32_t nextSerialNumber_;	// 次のユニークid

	Node node_;				// node本体
	NodeType type_;			// nodeのタイプ
	BehaviorStatus state_;	// nodeの状態

	std::vector<IBehaviorNode*> children_;	// 子ども
	uint32_t currentIndex_;					// 現在参照している子のindex

	bool isLeafNode_ = false;				// リーフノードかどうか

	Vector2 pos_;				// Nodeの座標
	bool setNodePos_;			// Node座標の設定を行ったかどうか

	IWorldState* worldState_;

	// -------------------------------------------------
	// ↓ Debug用
	// -------------------------------------------------

	// 編集フラグ
	bool isSelect_;
	bool isDelete_;

	// 選択カラー
	ImColor color_;
	ImColor baseColor_;

	// loggerポインタ
	BehaviorTreeLogger* pLogger_;
};

