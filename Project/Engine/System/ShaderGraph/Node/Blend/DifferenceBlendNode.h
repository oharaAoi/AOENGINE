#pragma once
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/DirectX/Resource/DxResource.h"

/// <summary>
/// Textureを差分で合成するNode
/// </summary>
class DifferenceBlendNode :
	public BaseShaderGraphNode {
public:	// コンストラクタ

	DifferenceBlendNode();
	~DifferenceBlendNode() override;

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

	/// <summary>
	/// json形式にする
	/// </summary>
	/// <returns></returns>
	nlohmann::json toJson() override;

	/// <summary>
	/// json形式から情報を設定する
	/// </summary>
	/// <param name="_json"></param>
	void fromJson(const nlohmann::json& _json) override;

private:

	/// <summary>
	/// 合成結果のResourceを作成する
	/// </summary>
	void CreateBlendResource();

private:

	DxResource* resourceA_ = nullptr;
	DxResource* resourceB_ = nullptr;

	DxResource* blendResource_;

	GraphicsContext* ctx_;
	ID3D12GraphicsCommandList* cmdList_;
};

