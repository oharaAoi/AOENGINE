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
	float aggressionScore = blackboard_->Get("aggressionScore").As<float>();
	aggressionScore = 1.0f - aggressionScore;
	float score = BossEvaluationFormula::AppropriateDistance(distance, consideration_.optimal, consideration_.optimalRange);
	// 積極性が低いほどスコアが高くなるようにする
	aggressionScore = std::lerp(0.0f, 1.0f, aggressionScore);

	return (score * (aggressionScore * priority_)) + sMinimumCorrectionCalue_;
} 

void SafeDistanceOriented::Debug_Gui() {
	if (ImGui::CollapsingHeader("Consideration")) {
		consideration_.Debug_Gui();
	}
}

void SafeDistanceOriented::Consideration::Debug_Gui() {
	ImGui::DragFloat("優先度", &priority, 1);
	ImGui::DragFloat("適正距離", &optimal, 1);
	ImGui::DragFloat("適正距離許容範囲", &optimalRange, 1);
	SaveAndLoad();
}
