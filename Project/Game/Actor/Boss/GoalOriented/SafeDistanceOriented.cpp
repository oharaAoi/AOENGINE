#include "SafeDistanceOriented.h"
#include "Engine/System/Manager/ImGuiManager.h"

SafeDistanceOriented::SafeDistanceOriented() {
    SetName("SafeDistance");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 目標の合否
///////////////////////////////////////////////////////////////////////////////////////////////

bool SafeDistanceOriented::IsGoal() {
    if (!worldState_->Get<bool>("isMove")) {
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 評価値の計算
///////////////////////////////////////////////////////////////////////////////////////////////

float SafeDistanceOriented::CalculationScore() {
    float current = worldState_->Get<float>("postureStability");
    float maxCurrent = worldState_->Get<float>("maxPostureStability");
    float raito = current / maxCurrent;

    float distance = worldState_->Get<float>("targetToDistance");
    float idealDistance = worldState_->Get<float>("idealDistance");
    float totalDistance = std::fabs(distance - idealDistance) / idealDistance;

    return (raito + totalDistance) / 2.0f;
}

void SafeDistanceOriented::Debug_Gui() {
    ImGui::DragInt("priority", &priority_, 1);
}
