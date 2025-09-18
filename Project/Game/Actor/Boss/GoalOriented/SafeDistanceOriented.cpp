#include "SafeDistanceOriented.h"

SafeDistanceOriented::SafeDistanceOriented() {
    SetName("SafeDistance");
}

bool SafeDistanceOriented::IsGoal() {
    if (worldState_->Get<float>("targetToDistance") < 100.0f) {
        return true;
    }
    return false;
}

float SafeDistanceOriented::CalculationScore() {
    float current = worldState_->Get<float>("postureStability");
    float maxCurrent = worldState_->Get<float>("maxPostureStability");
    float raito = current / maxCurrent;
    return raito;
}
