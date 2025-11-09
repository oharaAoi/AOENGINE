#pragma once
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

template<typename T>
class InOutPriorityNode :
	public BaseShaderGraphNode {
public: // コンストラクタ

	InOutPriorityNode() = default;
	~InOutPriorityNode() override = default;

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

	T inputValue_;
	T value_;

};

template<typename T>
inline void InOutPriorityNode<T>::Init() {
	// -------- 入力ピン ----------
	if constexpr (std::is_same_v<T, float>) {
		addIN<float>("x", inputValue_, ImFlow::ConnectionFilter::SameType());

	} else if constexpr (std::is_same_v<T, Vector2>) {
		addIN<float>("x", inputValue_.x, ImFlow::ConnectionFilter::SameType());
		addIN<float>("y", inputValue_.y, ImFlow::ConnectionFilter::SameType());

	} else if constexpr (std::is_same_v<T, Vector3>) {
		addIN<float>("x", inputValue_.x, ImFlow::ConnectionFilter::SameType());
		addIN<float>("y", inputValue_.y, ImFlow::ConnectionFilter::SameType());
		addIN<float>("z", inputValue_.z, ImFlow::ConnectionFilter::SameType());
		
	} else if constexpr (std::is_same_v<T, Vector4>) {
		addIN<float>("x", inputValue_.x, ImFlow::ConnectionFilter::SameType());
		addIN<float>("y", inputValue_.y, ImFlow::ConnectionFilter::SameType());
		addIN<float>("z", inputValue_.z, ImFlow::ConnectionFilter::SameType());
		addIN<float>("w", inputValue_.w, ImFlow::ConnectionFilter::SameType());

	} else if constexpr (std::is_same_v<T, Color>) {
		addIN<Color>("color", inputValue_, ImFlow::ConnectionFilter::SameType());
	}
}

template<typename T>
inline void InOutPriorityNode<T>::customUpdate() {
	value_ = inputValue_;
}

template<typename T>
inline void InOutPriorityNode<T>::draw() {
	// -------- 入力ピン ----------
	if constexpr (std::is_same_v<T, float>) {
		inputValue_ = getInVal<float>("x");

	} else if constexpr (std::is_same_v<T, Vector2>) {
		inputValue_.x = getInVal<float>("x");
		inputValue_.y = getInVal<float>("y");

	} else if constexpr (std::is_same_v<T, Vector3>) {
		inputValue_.x = getInVal<float>("x");
		inputValue_.y = getInVal<float>("y");
		inputValue_.z = getInVal<float>("z");

	} else if constexpr (std::is_same_v<T, Vector4>) {
		inputValue_.x = getInVal<float>("x");
		inputValue_.y = getInVal<float>("y");
		inputValue_.z = getInVal<float>("z");
		inputValue_.w = getInVal<float>("w");

	} else if constexpr (std::is_same_v<T, Color>) {
		inputValue_ = getInVal<Color>("color");
	}

	showOUT<T>(
		"output",
		[=]() -> T {
			// 簡易的なカラー出力 (本来はGPUサンプリング)
			return value_;
		},
		ImFlow::PinStyle::blue()
	);
}
