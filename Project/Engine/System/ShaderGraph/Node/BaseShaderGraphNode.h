#pragma once
#include <cstdint>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "Flow/ImNodeFlow.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Lib/Node/NodeItems.h"

/// <summary>
/// ShaderGraphの基底クラス
/// </summary>
class BaseShaderGraphNode :
	public ImFlow::BaseNode {
public:	// コンストラクタ
	
	BaseShaderGraphNode() = default;
	virtual ~BaseShaderGraphNode();

public:

	/// <summary>
	/// 初期化処理a
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// Node描画
	/// </summary>
	virtual void draw() override = 0;

};

