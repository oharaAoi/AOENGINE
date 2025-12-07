#pragma once
#include <cstdint>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "Flow/ImNodeFlow.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Lib/Node/NodeItems.h"

namespace AOENGINE {

/// <summary>
/// ShaderGraphの基底クラス
/// </summary>
class BaseShaderGraphNode :
	public ImFlow::BaseNode {
public:	// コンストラクタ

	BaseShaderGraphNode() = default;
	virtual ~BaseShaderGraphNode() override;

public:

	/// <summary>
	/// 初期化処理a
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void customUpdate() override = 0;

	/// <summary>
	/// Guiの更新
	/// </summary>
	virtual void updateGui() override = 0;

	/// <summary>
	/// Node描画
	/// </summary>
	virtual void draw() override = 0;

	/// <summary>
	/// json形式にする
	/// </summary>
	/// <returns></returns>
	virtual nlohmann::json toJson() = 0;

	/// <summary>
	/// json形式から情報を設定する
	/// </summary>
	/// <param name="_json"></param>
	virtual void fromJson(const nlohmann::json& _json) = 0;

protected:

	/// <summary>
	/// 基本の情報を取り出す
	/// </summary>
	/// <param name="_json"></param>
	void BaseInfoFromJson(const nlohmann::json& _json);

	/// <summary>
	/// 基本の情報をjsonにする
	/// </summary>
	/// <param name="_json"></param>
	void BaseInfoToJson(nlohmann::json& _json);

};

}