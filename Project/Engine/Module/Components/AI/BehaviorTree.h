#pragma once
#include <list>
#include <vector>
#include <memory>
#include "Engine/Module/Components/AI/IBehaviorNode.h"
#include "Engine/Module/Components/AI/BehaviorRootNode.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "imgui_node_editor.h"

enum NodeType {
	Sequencer,
	Selector,
	Task,
};

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

private:

	// 接続
	void Connect();

	// node描画
	void DrawNode();

	// dockingボタン描画
	void DockingButton();

	// node生成Window
	void CreateNodeWindow();

	// node作成
	void CreateNode(const std::string& nodeName, int nodeType);

	// nodeの検索
	IBehaviorNode* FindNodeFromPin(ax::NodeEditor::PinId pin);

	bool InputTextWithString(const char* label, std::string& str, size_t maxLength = 256);

private:

	ax::NodeEditor::EditorContext* context_ = nullptr;

	std::vector<Link> links_;

	std::list<std::unique_ptr<IBehaviorNode>> nodeList_;
	IBehaviorNode* root_;

	// フラグ
	bool isOpenEditor_ = false;
	bool isOpenCreateNode_ = false;
	bool canDocking_ = false;

	ImGuiWindowFlags windowFlags_;
};

