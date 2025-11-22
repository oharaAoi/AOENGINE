#include "TargetDeadOriented.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include <algorithm>

TargetDeadOriented::TargetDeadOriented() {
    SetName("TargetDead");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 目標の合否
//////////////////////////////////////////////////////////////////////////////////////////////

bool TargetDeadOriented::IsGoal() {
    /*if(worldState_->Get("isAttack").As<bool>()){
        return true;
    }*/
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 評価値の計算
///////////////////////////////////////////////////////////////////////////////////////////////

float TargetDeadOriented::CalculationScore() {
    /*float distance = worldState_->Get("targetToDistance").As<float>();
    float idealDistance = worldState_->Get("idealDistance").As<float>();
    float diff = std::fabs(distance - idealDistance);*/

    return 1;
}

void TargetDeadOriented::Debug_Gui() {
    ImGui::DragInt("priority", &priority_, 1);
}
