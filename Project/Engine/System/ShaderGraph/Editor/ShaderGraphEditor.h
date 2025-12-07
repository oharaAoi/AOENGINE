#pragma once
#include <memory>
#include <unordered_set>
#include <string>
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/ShaderGraph/ShaderGraphNodeFactory.h"
#include "Engine/System/ShaderGraph/Node/ShaderGraphResultNode.h"

namespace AOENGINE {

/// <summary>
/// ShaderGraphを編集するクラス
/// </summary>
class ShaderGraphEditor {
public:	// コンストラクタ

	ShaderGraphEditor();
	~ShaderGraphEditor();

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void InspectorWindow();

private:

	/// <summary>
	/// Nodeの更新を実行する
	/// </summary>
	/// <param name="node">: 現在のNodeのポインタ</param>
	/// <param name="visited">: 訪れたNodeのポインタマップ</param>
	void ExecuteFrom(ImFlow::BaseNode* node, std::unordered_set<ImFlow::BaseNode*>& visited);

	/// <summary>
	/// 編集
	/// </summary>
	void Edit();

	/// <summary>
	/// Nodeの作成
	/// </summary>
	void CreateNode();

	/// <summary>
	/// Nodeを保存する
	/// </summary>
	void SaveGraph();

	/// <summary>
	/// Graphを読み込む
	/// </summary>
	void LoadGraph();

	/// <summary>
	/// Graphを上書き保存する
	/// </summary>
	void OverwriteGraph();

public:

	AOENGINE::DxResource* GetResource() const { return resultNode_->GetResultSource(); }

private:

	std::unique_ptr<ImFlow::ImNodeFlow> editor_;

	ShaderGraphNodeFactory nodeFactory_;
	std::shared_ptr<ShaderGraphResultNode> resultNode_ = nullptr;

	// 編集のための変数
	ImVec2 popupPos_;
	bool popupRequested_ = false;

	std::string graphPath_ = "";
};

}