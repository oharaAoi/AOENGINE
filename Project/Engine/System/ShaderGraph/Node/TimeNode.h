#pragma once
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"

/// <summary>
/// Time更新用のNode
/// </summary>
class TimeNode :
	public BaseShaderGraphNode {
public:	// コンストラクタ

	TimeNode();
	~TimeNode() override = default;

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

	float time_;

	float sineTime_;

};