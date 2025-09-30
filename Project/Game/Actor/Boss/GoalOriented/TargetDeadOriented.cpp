#include "TargetDeadOriented.h"
#include <imgui.h>
#include <algorithm>

TargetDeadOriented::TargetDeadOriented() {
    SetName("TargetDead");
}

void TargetDeadOriented::Update() {

}

bool TargetDeadOriented::IsGoal() {
    if(worldState_->Get<bool>("isAttack")){
        return true;
    }
    return false;
}

float TargetDeadOriented::CalculationScore() {
    float distance = worldState_->Get<float>("targetToDistance");
    float idealDistance = worldState_->Get<float>("idealDistance");
    float diff = std::fabs(distance - idealDistance);

    return diff / idealDistance;
}

void TargetDeadOriented::Debug_Gui() {
    ImGui::DragInt("priority", &priority_, 1);
}
