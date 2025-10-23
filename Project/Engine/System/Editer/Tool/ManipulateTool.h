#pragma once
#include <string>
#include <imgui.h>

enum class UseManipulate {
	SCALE,
	ROTATE,
	TRANSLATE,
};

/// <summary>
/// gizumoTool
/// </summary>
class ManipulateTool {
public:

	ManipulateTool() = default;
	~ManipulateTool() = default;

public:

	/// <summary>
	/// Manipulateの選択
	/// </summary>
	void SelectUseManipulate();

private:

	/// <summary>
	/// 拡縮回転座標に応じたボタンを表示する
	/// </summary>
	/// <param name="texId"></param>
	/// <param name="label"></param>
	/// <param name="checkType"></param>
	void Button(const ImTextureID& texId, const std::string& label, UseManipulate checkType);

public:

	static UseManipulate type_;
	static bool isActive_;

};

