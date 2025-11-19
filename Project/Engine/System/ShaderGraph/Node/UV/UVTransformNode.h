#pragma once
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include "Engine/Lib/Node/NodeItems.h"
#include "Engine/Lib/Math/Vector2.h"

/// <summary>
/// uvTransformを変化させるNode
/// </summary>
class UVTransformNode :
	public BaseShaderGraphNode {
public:	// コンストラクタ

	UVTransformNode() = default;
	~UVTransformNode() override = default;

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

	Vector2 inputUV_ = CVector2::ZERO;
	NodeUVTransform outputUV_;

	Vector2 scale_ = CVector2::UNIT;
	Vector2 offset_ = CVector2::ZERO;
	float rotate_ = 0.f;



};