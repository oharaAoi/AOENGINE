#include "TargetDeadOriented.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Lib/Math/MyRandom.h"
#include <algorithm>

TargetDeadOriented::TargetDeadOriented() {
    SetName("TargetDead");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 評価値の計算
///////////////////////////////////////////////////////////////////////////////////////////////

float TargetDeadOriented::CalculationScore() {
    return RandomFloat(0.2f, 1.0f);
}

void TargetDeadOriented::Debug_Gui() {
    ImGui::DragInt("priority", &priority_, 1);
}
