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
    }
}