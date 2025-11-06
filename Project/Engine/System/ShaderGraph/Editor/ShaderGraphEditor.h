#pragma once
#include <memory>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "Flow/ImNodeFlow.h"
#include "Engine/Lib/Node/NodeItems.h"
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"

/// <summary>
/// ShaderGraphを編集するクラス
/// </summary>
class ShaderGraphEditor {
public:	// コンストラクタ

	ShaderGraphEditor() = default;
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

	/// <summary>
	/// 編集
	/// </summary>
	void Edit();

	/// <summary>
	/// Nodeの作成
	/// </summary>
	void CreateNode();

	/// <summary>
	/// propertyの作成
	/// </summary>
	void CreateProperty();

	/// <summary>
	/// TextureNodeの作成
	/// </summary>
	void CreteTexture();

private:

	ImFlow::ImNodeFlow editor;

	// nodeEditorのポインタ
	ax::NodeEditor::EditorContext* context_ = nullptr;
	// 接続のidをまとめたコンテナ
	std::vector<Link> links_;
	// node
	std::vector<std::unique_ptr<BaseShaderGraphNode>> nodes_;
	std::vector<std::shared_ptr<BaseShaderGraphNode>> propertyNodes_;

};

