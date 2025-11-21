#pragma once
#include <string>
#include <vector>
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Lib/Color.h"
#include "Engine/System/Manager/ImGuiManager.h"

bool InputTextWithString(const char* filedName, const char* label, std::string& str, size_t maxLength = 256);

bool ButtonOpenDialog(const char* buttonLabel, const char* dialogKey, const char* windowTitle, const char* filter, std::string& outPath);

int ContainerOfComb(const std::vector<std::string>& items, int& selectedIndex, const char* label = "##combo");

template<typename T>
void TemplateValueGui(T& value) {
    if constexpr (std::is_same_v<T, float>) {
        ImGui::DragFloat("Value", &value, 0.01f);
    } else if constexpr (std::is_same_v<T, Vector2>) {
        ImGui::DragFloat2("Value", &value.x, 0.01f);
    } else if constexpr (std::is_same_v<T, Vector3>) {
        ImGui::DragFloat3("Value", &value.x, 0.01f);
    } else if constexpr (std::is_same_v<T, Vector4>) {
        ImGui::DragFloat4("Value", &value.x, 0.01f);
    } else if constexpr (std::is_same_v<T, Color>) {
        ImGui::ColorEdit4("color", &value.r);
    } else if constexpr (std::is_same_v<T, int32_t>) {
        ImGui::DragInt("Value", &value);
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
        ImGui::Text("value(%f)", value);
    } else if constexpr (std::is_same_v<T, Vector2>) {
        ImGui::Text("x(%f) y(%f)", value.x, value.y);
    } else if constexpr (std::is_same_v<T, Vector3>) {
        ImGui::Text("x(%f) y(%f) z(%f)", value.x, value.y, value.z);
    } else if constexpr (std::is_same_v<T, Vector4>) {
        ImGui::Text("x(%f) y(%f) z(%f) w(%f)", value.x, value.y, value.z, value.w);
    } else if constexpr (std::is_same_v<T, Color>) {
        ImGui::Text("r(%f) g(%f) b(%f) a(%f)", value.r, value.g, value.b, value.a);
    } else if constexpr (std::is_same_v<T, int32_t>) {
        ImGui::Text("value(%d)", value);
    } else if constexpr (std::is_same_v<T, std::string>) {
        ImGui::Text(value.c_str());
    }
}