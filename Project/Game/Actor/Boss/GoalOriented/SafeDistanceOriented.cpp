#include "SafeDistanceOriented.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/AI/State/Blackboard.h"
#include "Game/Actor/Boss/BossEvaluationFormula.h"

SafeDistanceOriented::SafeDistanceOriented() {
    SetName("SafeDistance");
	consideration_.Load();
	priority_ = consideration_.priority;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 評価値の計算
///////////////////////////////////////////////////////////////////////////////////////////////

float SafeDistanceOriented::CalculationScore() {
	float distance = blackboard_->Get("BossToPlayer").As<float>();
	float score = BossEvaluationFormula::AppropriateDistance(distance, consideration_.optimal, consideration_.optimalRange);
	return score * priority_ + sMinimumCorrectionCalue_;
} 

void SafeDistanceOriented::Debug_Gui() {
	if (ImGui::CollapsingHeader("Consideration")) {
		consideration_.Debug_Gui();
	}
}

void SafeDistanceOriented::Consideration::Debug_Gui() {
	ImGui::DragFloat("priority", &priority, 1);
	ImGui::DragFloat("optimal", &optimal, 1);
	ImGui::DragFloat("optimalRange", &optimalRange, 1);
	SaveAndLoad();
}
