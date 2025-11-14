#pragma once
#include <memory>
#include <unordered_set>
#include <vector>
#include <string>
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/ShaderGraph/ShaderGraphNodeFactory.h"
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include "Engine/System/ShaderGraph/Node/ShaderGraphResultNode.h"
#include <functional>

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

private:

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
	/// 読み取りの際のエクスプローラーを開く
	/// </summary>
	/// <returns></returns>
	std::string OpenWindowsExplore();

	/// <summary>
	/// 保存の際のエクスプローラーを開く
	/// </summary>
	/// <returns></returns>
	std::string SaveWindowsExplore();

private:

	std::unique_ptr<ImFlow::ImNodeFlow> editor_;

	ShaderGraphNodeFactory nodeFactory_;
	std::shared_ptr<ShaderGraphResultNode> resultNode_ = nullptr;

	// 編集のための変数
	ImVec2 popupPos_;
	bool popupRequested_ = false;
};

