#include "BossAI.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/Actor/Boss/Action/Move/BossActionApproach.h"
#include "Game/Actor/Boss/Action/Move/BossActionKeepDistance.h"
#include "Game/Actor/Boss/Action/Move/BossActionLeave.h"
#include "Game/Actor/Boss/Action/Move/BossActionStrafe.h"
#include "Engine/Lib/Math/MyRandom.h"

void BossAI::Init() {
	SetName("BossAI");
	param_.FromJson(JsonItems::GetData("Boss", param_.GetName()));
	weight.FromJson(JsonItems::GetData("BossAI", weight.GetName()));

	scoreMap_.emplace(typeid(BossActionApproach).hash_code(), ActionScore());
	scoreMap_.emplace(typeid(BossActionLeave).hash_code(), ActionScore());
	scoreMap_.emplace(typeid(BossActionStrafe).hash_code(), ActionScore());
	scoreMap_.emplace(typeid(BossActionKeepDistance).hash_code(), ActionScore());
}

size_t BossAI::MoveActionAI(const WorldTransform* bossTransform, const Vector3& targetPos) {

	Vector3 direction = targetPos - bossTransform->translate_;  // targetへの方向
	distance_ = direction.Length();                        // targetとの距離
	
	Vector3 bossForward = bossTransform->GetQuaternion().MakeForward(); // bossの前方方向
	float dot = Vector3::Dot(direction, bossForward);           // bossと同じ方向を向いているかを計算

	float farScore = GetProximityScore(distance_, param_.farDistance, 100.0f);
	float midScore = GetProximityScore(distance_, param_.midDistance, 100.0f);
	float nearScore = GetProximityScore(distance_, param_.nearDistance, 100.0f);

	scoreMap_.at(typeid(BossActionApproach).hash_code()).distanceScore = farScore;
	scoreMap_.at(typeid(BossActionStrafe).hash_code()).distanceScore = midScore;
	scoreMap_.at(typeid(BossActionLeave).hash_code()).distanceScore = nearScore;
	scoreMap_.at(typeid(BossActionKeepDistance).hash_code()).distanceScore = (midScore + nearScore) * 0.5f;

	// bossの後ろにtargetがいるのであればbossは離れる行動を行う
	if (dot <= -0.6f) {
		scoreMap_.at(typeid(BossActionLeave).hash_code()).specialActionScore = 0.8f;
	} else {
		scoreMap_.at(typeid(BossActionLeave).hash_code()).specialActionScore = 0.0f;
	}

	float maxScore = -999.0f;
	size_t maxId = 0;
	for (auto score : scoreMap_) {
		float total = score.second.TotalScore(weight);
		if (maxScore < total) {
			maxScore = total;
			maxId = score.first;
		} 
	}

	for (auto score : scoreMap_) {
		if (maxId != score.first) {
			score.second.unusedBonusScore++;
		} else {
			score.second.unusedBonusScore = 0;
		}
	}

	return maxId;
}

void BossAI::Debug_Gui() {
	if (ImGui::CollapsingHeader("Parameter")) {
		ImGui::DragFloat("farDistance", &param_.farDistance, .1f);
		ImGui::DragFloat("midDistance", &param_.midDistance, .1f);
		ImGui::DragFloat("nearDistance", &param_.nearDistance, .1f);

		if (ImGui::Button("Save")) {
			JsonItems::Save("Boss", param_.ToJson(param_.GetName()));
		}
	}

	if (ImGui::CollapsingHeader("Weight")) {
		ImGui::DragFloat("distance", &weight.distance, .1f);
		ImGui::DragFloat("special", &weight.special, .1f);
		ImGui::DragFloat("unused", &weight.unused, .1f);

		if (ImGui::Button("Save")) {
			JsonItems::Save("BossAI", weight.ToJson(weight.GetName()));
		}
	}

	ImGui::Separator();
	
	ImGui::Text("PlayerTo : %f", distance_);

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("BossActionApproach")) {
		scoreMap_.at(typeid(BossActionApproach).hash_code()).Debug();
		float score = scoreMap_.at(typeid(BossActionApproach).hash_code()).TotalScore(weight);
		ImGui::Text("score : %f", score);
		ImGui::TreePop();
	}
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("BossActionStrafe")) {
		scoreMap_.at(typeid(BossActionStrafe).hash_code()).Debug();
		float score = scoreMap_.at(typeid(BossActionStrafe).hash_code()).TotalScore(weight);
		ImGui::Text("score : %f", score);
		ImGui::TreePop();
	}
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("BossActionLeave")) {
		scoreMap_.at(typeid(BossActionLeave).hash_code()).Debug();
		float score = scoreMap_.at(typeid(BossActionLeave).hash_code()).TotalScore(weight);
		ImGui::Text("score : %f", score);
		ImGui::TreePop();
	}
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("BossActionKeepDistance")) {
		scoreMap_.at(typeid(BossActionKeepDistance).hash_code()).Debug();
		float score = scoreMap_.at(typeid(BossActionKeepDistance).hash_code()).TotalScore(weight);
		ImGui::Text("score : %f", score);
		ImGui::TreePop();
	}
}

float BossAI::GetProximityScore(float distance, float targetDistance, float maxDistance) {
	float diff = fabs(distance - targetDistance); // 差の絶対値
	float normalized = 1.0f - std::clamp(diff / maxDistance, 0.0f, 1.0f);
	float score = 0.2f + normalized * 0.6f; // → 結果は 0.2 ～ 0.8 に収まる
	return score;
}

float BossAI::ActionScore::TotalScore(const ActionScoreWeight& weight) {
	return (distanceScore * weight.distance)
		+ (specialActionScore * weight.special)
		+ (unusedBonusScore * weight.unused);
}
