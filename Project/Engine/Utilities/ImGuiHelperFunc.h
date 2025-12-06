#pragma once
#include <string>
#include <vector>
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Lib/Color.h"
#include "Engine/System/Manager/ImGuiManager.h"

bool InputTextWithString(const char* filedName, const char* label, std::string& str, size_t maxLength = 256, float inputFiledSize = 124.f);

bool ButtonOpenDialog(const char* buttonLabel, const char* dialogKey, const char* windowTitle, const char* filter, std::string& outPath);

int ContainerOfComb(const std::vector<std::string>& items, int& selectedIndex, const char* label = "##combo", float inputFiledSize = 124.f);

template<typename T>
void TemplateValueGui(const std::string& _name, T& value) {
	 if constexpr (std::is_same_v<T, float>) {
		ImGui::DragFloat(_name.c_str(), &value, 0.01f);
	} else if constexpr (std::is_same_v<T, int32_t>) {
		ImGui::DragInt(_name.c_str(), &value, 1);
	} else if constexpr (std::is_same_v<T, bool>) {
		ImGui::Checkbox(_name.c_str(), &value);
	} else if constexpr (std::is_same_v<T, Math::Vector2>) {
		ImGui::DragFloat2(_name.c_str(), &value.x, 0.01f);
	} else if constexpr (std::is_same_v<T, Math::Vector3>) {
		ImGui::DragFloat3(_name.c_str(), &value.x, 0.01f);
	} else if constexpr (std::is_same_v<T, Math::Vector4>) {
		ImGui::DragFloat4(_name.c_str(), &value.x, 0.01f);
	} else if constexpr (std::is_same_v<T, AOENGINE::Color>) {
		ImGui::ColorEdit4(_name.c_str(), &value.r);
	} else if constexpr (std::is_same_v<T, std::string>) {
		ImGui::Text(value);
	}
}

template<typename T>
void TemplateValueText(const T& value) {
	// ポインタだった場合デリファレンスして再帰呼び出し
	if constexpr (std::is_pointer_v<T>) {
		if (value == nullptr) {
			ImGui::Text("null");
		} else {
			TemplateValueText(*value); // ★ここが重要！
		}
	}

	// 値型の場合
	if constexpr (std::is_same_v<T, float>) {
		ImGui::Text("float(%f)", value);
	} else if constexpr (std::is_same_v<T, Math::Vector2>) {
		ImGui::Text("x(%f) y(%f)", value.x, value.y);
	} else if constexpr (std::is_same_v<T, Math::Vector3>) {
		ImGui::Text("x(%f) y(%f) z(%f)", value.x, value.y, value.z);
	} else if constexpr (std::is_same_v<T, Math::Vector4>) {
		ImGui::Text("x(%f) y(%f) z(%f) w(%f)", value.x, value.y, value.z, value.w);
	} else if constexpr (std::is_same_v<T, AOENGINE::Color>) {
		ImGui::Text("r(%f) g(%f) b(%f) a(%f)", value.r, value.g, value.b, value.a);
	} else if constexpr (std::is_same_v<T, int32_t>) {
		ImGui::Text("int(%d)", value);
	} else if constexpr (std::is_same_v<T, std::string>) {
		ImGui::Text(value.c_str());
	} else if constexpr (std::is_same_v<T, bool>) {
		if (value) {
			ImGui::Text("bool(true)");
		} else {
			ImGui::Text("bool(false)");
		}

	}
}