#pragma once
#include "memory"
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/Core/GraphicsContext.h"

/// <summary>
/// Textureを入力してTextureを出力するNode
/// </summary>
class SampleTexture2dNode :
	public BaseShaderGraphNode {
public:

	struct UVParam {
		Vector2 uv = Vector2(0, 0);
	};

public:	// コンストラクタ

	SampleTexture2dNode();
	~SampleTexture2dNode() override;

public:

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新関数
	/// </summary>
	void customUpdate() override;

	/// <summary>
	/// guiの更新
	/// </summary>
	void updateGui() override {};

	/// <summary>
	/// Node描画
	/// </summary>
	void draw() override;

private:

	std::unique_ptr<DxResource> resource_ = nullptr;
	ComPtr<ID3D12Resource> uvBuffer_;
	UVParam* uvParam_ = nullptr;

	GraphicsContext* ctx_;
	ID3D12GraphicsCommandList* cmdList_;

	// ノード内部の状態
	DxResource* inputResource_ = nullptr;
	Vector2 uv_ = { 0.0f, 0.0f };         // UV入力のデフォルト値
	Color color_ = { 1.0f, 1.0f, 1.0f, 1.0f };

};