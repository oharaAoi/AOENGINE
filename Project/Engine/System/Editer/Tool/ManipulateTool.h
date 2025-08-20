#pragma once
#include <string>
#include <imgui.h>

enum class UseManipulate {
	SCALE,
	ROTATE,
	TRANSLATE,
};

class ManipulateTool {
public:

	ManipulateTool() = default;
	~ManipulateTool() = default;

	void SelectUseManipulate();

private:

	void Button(const ImTextureID& texId, const std::string& label, UseManipulate checkType);

public:

	static UseManipulate type_;
	static bool isActive_;

};

