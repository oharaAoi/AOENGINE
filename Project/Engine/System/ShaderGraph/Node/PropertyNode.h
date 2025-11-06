#pragma once
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

/// <summary>
/// floatやVector2などのtemplateに基本の型のNode
/// </summary>
/// <typeparam name="T"></typeparam>
template<typename T>
class PropertyNode :
	public BaseShaderGraphNode {
public: // コンストラクタ

	PropertyNode(const std::string& _variableType) : variableType_(_variableType) {}
	~PropertyNode() = default;

public:

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init() override {};

	/// <summary>
	/// 更新関数
	/// </summary>
	void Update() override {};

	/// <summary>
	/// Node描画
	/// </summary>
	void draw() override;

private:

	T value_;
	std::string variableType_;
};

template<typename T>
inline void PropertyNode<T>::draw() {
	// -------- 出力ピン ----------
	showOUT<T>(
		variableType_,
		[=]() -> T {
			// 簡易的なカラー出力 (本来はGPUサンプリング)
			return value_;
		},
		ImFlow::PinStyle::blue()
	);

	// -------- 内部プレビュー ----------
	ImGui::Spacing();
	ImGui::SetNextItemWidth(150);
	TemplateValueGui(value_);
}
