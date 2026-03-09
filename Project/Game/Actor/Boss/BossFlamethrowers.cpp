#include "BossFlamethrowers.h"
#include "Engine/Lib/Math/MyMath.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossFlamethrowers::Debug_Gui() {
	if (ImGui::CollapsingHeader("left")) {
		flamethrowers_[(int)BossFlamethrowersType::Left]->Debug_Gui();
		param_[(int)BossFlamethrowersType::Left].Debug_Gui();
	}

	if (ImGui::CollapsingHeader("right")) {
		flamethrowers_[(int)BossFlamethrowersType::Right]->Debug_Gui();
		param_[(int)BossFlamethrowersType::Right].Debug_Gui();
	}
}

void BossFlamethrowers::Parameter::Debug_Gui() {
	Math::Quaternion rotate = Math::Quaternion();

	ImGui::DragFloat3("scale", &flamethrowerSRT.scale.x, 0.1f);
	ImGui::DragFloat4("rotate", &flamethrowerSRT.rotate.x, 0.1f);
	ImGui::DragFloat3("translate", &flamethrowerSRT.translate.x, 0.1f);
	ImGui::DragFloat("radius", &radius);
	ImGui::DragFloat("angle", &angle);
	ImGui::Checkbox("時計回り", &clockwise);

	rotate = flamethrowerSRT.rotate * rotate;

	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossFlamethrowers::Init(AOENGINE::WorldTransform* transform) {
	SetName("Flamethrowers");

	param_[(int)BossFlamethrowersType::Left].SetName("LeftBossFlamethrower");
	param_[(int)BossFlamethrowersType::Right].SetName("RightBossFlamethrower");

	for (int i = 0; i < kFlamethrowerCount_; ++i) {
		param_[i].Load();

		flamethrowers_[i] = std::make_unique<Flamethrower>();
		flamethrowers_[i]->Init();

		flamethrowers_[i]->GetTransform()->SetParent(transform->GetWorldMatrix());
		flamethrowers_[i]->GetTransform()->SetSRT(param_[i].flamethrowerSRT);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossFlamethrowers::Update() {
	for (int i = 0; i < kFlamethrowerCount_; ++i) {
		Math::Vector3 pos = CalcOrbitPosition(CVector3::ZERO, CVector3::UP, param_[i].radius, param_[i].angle * kToRadian, param_[i].clockwise);
		flamethrowers_[i]->GetTransform()->SetTranslate(pos);

		Math::Vector3 dir = (pos - CVector3::ZERO).Normalize();
		flamethrowers_[i]->GetTransform()->SetRotate(Math::Quaternion::LookRotation(dir));

		flamethrowers_[i]->Update();
		flamethrowers_[i]->Attack(AttackContext());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ bulletManagerを設定する
///////////////////////////////////////////////////////////////////////////////////////////////

void BossFlamethrowers::SetBulletManager(BaseBulletManager* manager) {
	for (int i = 0; i < kFlamethrowerCount_; ++i) {
		flamethrowers_[i]->SetBulletManager(manager);
	}
}