#pragma once
#include <memory>
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include "Engine/DirectX/Resource/DxResource.h"

/// <summary>
/// Textureを出力するNode
/// </summary>
class TextureNode :
	public BaseShaderGraphNode {
public:	// コンストラクタ

	TextureNode() = default;
	~TextureNode() override;

public:

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新関数
	/// </summary>
	void customUpdate() override {};

	/// <summary>
	/// guiの更新
	/// </summary>
	void updateGui() override;

	/// <summary>
	/// Node描画
	/// </summary>
	void draw() override;

private:

	void SelectTexture();

private:

	std::string textureName_ = "";

	DxResource* resource_ = nullptr;

};

