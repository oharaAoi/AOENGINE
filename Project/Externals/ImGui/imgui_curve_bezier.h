#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

// この forward 宣言で .cpp 内の実装をリンクさせる
namespace ImGui {
    template<int steps>
    void bezier_table(ImVec2 P[4], ImVec2 results[steps + 1]);

    float BezierValue(float dt01, float P[4]);
    int Bezier(const char* label, float P[5]);
    void ShowBezierDemo();
}