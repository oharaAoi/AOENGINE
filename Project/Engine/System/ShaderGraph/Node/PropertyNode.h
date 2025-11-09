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

	PropertyNode() = default;
	PropertyNode(const std::string& _variableType) : variableType_(_variableType) {};
	~PropertyNode() override = default;

public:

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init() override {};

	/// <summary>
	/// 更新関数
	/// </summary>
	void customUpdate() override {};

	/// <summary>
	/// guiの更新
	/// </summary>
	void updateGui() override {};

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
	if constexpr (std::is_same_v<T, float>) {
		showOUT<float>(
			" x ",
			[=]() -> float {
				// 簡易的なカラー出力 (本来はGPUサンプリング)
				return value_;
			},
			ImFlow::PinStyle::blue()
		);

	} else if constexpr (std::is_same_v<T, Vector2>) {
		showOUT<float>(
			" x ",
			[=]() -> float {
				// 簡易的なカラー出力 (本来はGPUサンプリング)
				return value_.x;
			},
			ImFlow::PinStyle::blue()
		);

		showOUT<float>(
			" y ",
			[=]() -> float {
				// 簡易的なカラー出力 (本来はGPUサンプリング)
				return  value_.y;
			},
			ImFlow::PinStyle::blue()
		);

	} else if constexpr (std::is_same_v<T, Vector3>) {
		showOUT<float>(
			" x ",
			[=]() -> float {
				// 簡易的なカラー出力 (本来はGPUサンプリング)
				return value_.x;
			},
			ImFlow::PinStyle::blue()
		);

		showOUT<float>(
			" y ",
			[=]() -> float {
				// 簡易的なカラー出力 (本来はGPUサンプリング)
				return  value_.y;
			},
			ImFlow::PinStyle::blue()
		);

		showOUT<float>(
			" z ",
			[=]() -> float {
				// 簡易的なカラー出力 (本来はGPUサンプリング)
				return  value_.z;
			},
			ImFlow::PinStyle::blue()
		);
	} else if constexpr (std::is_same_v<T, Vector4>) {
		showOUT<float>(
			" x ",
			[=]() -> float {
				// 簡易的なカラー出力 (本来はGPUサンプリング)
				return value_.x;
			},
			ImFlow::PinStyle::blue()
		);

		showOUT<float>(
			" y ",
			[=]() -> float {
				// 簡易的なカラー出力 (本来はGPUサンプリング)
				return  value_.y;
			},
			ImFlow::PinStyle::blue()
		);

		showOUT<float>(
			" z ",
			[=]() -> float {
				// 簡易的なカラー出力 (本来はGPUサンプリング)
				return  value_.z;
			},
			ImFlow::PinStyle::blue()
		);

		showOUT<float>(
			" w ",
			[=]() -> float {
				// 簡易的なカラー出力 (本来はGPUサンプリング)
				return  value_.w;
			},
			ImFlow::PinStyle::blue()
		);

	} else if constexpr (std::is_same_v<T, Color>) {
		showOUT<Color>(
			variableType_,
			[=]() -> Color {
				// 簡易的なカラー出力 (本来はGPUサンプリング)
				return value_;
			},
			ImFlow::PinStyle::blue()
		);
	}

	// -------- 内部プレビュー ----------
	ImGui::Spacing();
	ImGui::SetNextItemWidth(150);
	TemplateValueGui(value_);
}
