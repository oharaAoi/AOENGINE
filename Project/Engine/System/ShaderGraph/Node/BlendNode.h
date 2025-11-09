#pragma once
#include "memory"
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/DirectX/Resource/DxResource.h"

/// <summary>
/// Textureを合成するNode
/// </summary>
class BlendNode :
	public BaseShaderGraphNode {
public:	// コンストラクタ

	BlendNode();
	~BlendNode() override;

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

	DxResource* resourceA_ = nullptr;
	DxResource* resourceB_ = nullptr;

	std::unique_ptr<DxResource> blendResource_;

	GraphicsContext* ctx_;
	ID3D12GraphicsCommandList* cmdList_;
};

