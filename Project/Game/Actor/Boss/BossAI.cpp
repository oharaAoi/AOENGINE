#include "BossAI.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/Actor/Boss/Action/Move/BossActionApproach.h"
#include "Game/Actor/Boss/Action/Move/BossActionKeepDistance.h"
#include "Game/Actor/Boss/Action/Move/BossActionLeave.h"

void BossAI::Init() {
	SetName("BossAI");
	param_.FromJson(JsonItems::GetData("Boss", param_.GetName()));
}

size_t BossAI::MoveActionAI(const WorldTransform* bossTransform, const Vector3& targetPos) {

	Vector3 direction = targetPos - bossTransform->translate_;  // targetへの方向
	float distance = direction.Length();                        // targetとの距離

	Vector3 bossForward = bossTransform->GetQuaternion().MakeForward(); // bossの前方方向
	float dot = Vector3::Dot(direction, bossForward);           // bossと同じ方向を向いているかを計算

	// 敵との距離が遠いのであれば近づく移動を行う(中~遠の場合の移動)
	// 一定時間 or ある程度近くまで
	if (distance >= param_.farDistance) {
		return typeid(BossActionApproach).hash_code();   // 近づく移動
	}

	// bossの後ろにtargetがいるのであればbossは離れる行動を行う
	if (dot <= -0.6f) {
		return typeid(BossActionLeave).hash_code();   // 近づく移動
	}

	// 敵との距離が近いのであれば回り込んだり遠くに移動したりする
	if (distance > param_.nearDistance && distance < param_.midDistance) {
		return typeid(BossActionKeepDistance).hash_code();   // 近づく移動
	}

	return typeid(BossActionKeepDistance).hash_code();
}

void BossAI::Debug_Gui() {
	ImGui::DragFloat("farDistance", &param_.farDistance, .1f);
	ImGui::DragFloat("midDistance", &param_.midDistance, .1f);
	ImGui::DragFloat("nearDistance", &param_.nearDistance, .1f);
	
	if (ImGui::Button("Save")) {
		JsonItems::Save("Boss", param_.ToJson(param_.GetName()));
	}
}
