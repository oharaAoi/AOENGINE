#pragma once
#include <memory>
#include <unordered_set>
#include <vector>
#include <string>
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/ShaderGraph/ShaderGraphNodeFactory.h"
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
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

	std::string OpenWindowsExplore();

private:

	std::unique_ptr<ImFlow::ImNodeFlow> editor_;

	ShaderGraphNodeFactory nodeFactory_;

};

