#include "DeployArmorOriented.h"

DeployArmorOriented::DeployArmorOriented() {
	SetName("DeployArmor");
}

bool DeployArmorOriented::IsGoal() {
	if (worldState_->Get<bool>("isDeployArmor")) {
		return true;
	}
	return false;
}

float DeployArmorOriented::CalculationScore() {
	if (worldState_->Get<bool>("isDeployArmor")) {
		return 1.0f;
	}
	return 0.0f;
}
