#pragma once
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"

/// <summary>
/// Textureを入力してTextureを出力するNode
/// </summary>
class SampleTexture2dNode :
	public BaseShaderGraphNode {
public:	// コンストラクタ

	SampleTexture2dNode();
	~SampleTexture2dNode();

public:

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新関数
	/// </summary>
	void Update() override;

	/// <summary>
	/// Node描画
	/// </summary>
	void draw() override;

private:

	// ノード内部の状態
	ImTextureID textureID_ = nullptr;  // ImGui表示用
	Vector2 uv_ = { 0.0f, 0.0f };         // UV入力のデフォルト値
	Color color_ = { 1.0f, 1.0f, 1.0f, 1.0f };

};