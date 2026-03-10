#include "BossFlamethrowers.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/GameTimer.h"

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

	deployParam_.Debug_Gui();

	static bool isDeploy = false;
	ImGui::Checkbox("isDeploy", &isDeploy);
	if (isDeploy) {
		isDeploy = !Deploy();
	} else {
		deployTimer_.timer_ = 0;
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

void BossFlamethrowers::DeployFlamethrowerParameter::Debug_Gui() {
	ImGui::DragFloat("deployTime", &deployTime);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossFlamethrowers::Init(AOENGINE::WorldTransform* transform) {
	SetName("Flamethrowers");

	param_[(int)BossFlamethrowersType::Left].SetName("LeftBossFlamethrower");
	param_[(int)BossFlamethrowersType::Right].SetName("RightBossFlamethrower");
	deployParam_.Load();
	deployTimer_ = AOENGINE::Timer(deployParam_.deployTime);

	for (int i = 0; i < kFlamethrowerCount_; ++i) {
		param_[i].Load();

		flamethrowers_[i] = std::make_unique<Flamethrower>();
		flamethrowers_[i]->Init();

		flamethrowers_[i]->GetTransform()->SetParent(transform->GetWorldMatrix());
		flamethrowers_[i]->GetTransform()->SetSRT(param_[i].flamethrowerSRT);
	}

	// 解除状態にしておく
	Remove();
	flamethrowers_[(int)BossFlamethrowersType::Left]->ColliderLocalPosInverse();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossFlamethrowers::Update() {
	for (int i = 0; i < kFlamethrowerCount_; ++i) {
		if (flamethrowers_[i]->GetIsAttack()) {
			Math::Vector3 pos = CalcOrbitPosition(CVector3::ZERO, CVector3::UP, param_[i].radius, param_[i].angle * kToRadian, param_[i].clockwise);
			flamethrowers_[i]->GetTransform()->SetTranslate(pos);

			Math::Vector3 dir = (pos - CVector3::ZERO).Normalize();
			flamethrowers_[i]->GetTransform()->SetRotate(Math::Quaternion::LookRotation(dir));
			flamethrowers_[i]->Attack(AttackContext());
		}

		flamethrowers_[i]->Update();
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

bool BossFlamethrowers::Deploy() {
	if (deployTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		for (int i = 0; i < kFlamethrowerCount_; ++i) {
			Math::Vector3 pos = CalcOrbitPosition(CVector3::ZERO, CVector3::UP, param_[i].radius, 90.0f * kToRadian, param_[i].clockwise);
			flamethrowers_[i]->GetTransform()->SetTranslate(pos);

			Math::Vector3 dir = (pos - CVector3::ZERO).Normalize();
			Math::Quaternion defaultRotate = Math::Quaternion::LookRotation(dir);
			Math::Quaternion endRotate = Math::Quaternion::AngleAxis(kPI, CVector3::FORWARD);
			Math::Quaternion lerpRotate = Math::Quaternion::Slerp(defaultRotate, endRotate, deployTimer_.t_);

			Math::Quaternion rotate = defaultRotate * lerpRotate;
			flamethrowers_[i]->GetTransform()->SetRotate(rotate);
		}

		return false;
	} else {
		for (int i = 0; i < kFlamethrowerCount_; ++i) {
			flamethrowers_[i]->SetIsAttack(true);
		}
		return true;
	}
}

void BossFlamethrowers::Remove() {
	for (int i = 0; i < kFlamethrowerCount_; ++i) {
		flamethrowers_[i]->SetIsAttack(false);
		Math::Vector3 pos = CalcOrbitPosition(CVector3::ZERO, CVector3::UP, param_[i].radius, param_[i].angle * kToRadian, param_[i].clockwise);
		flamethrowers_[i]->GetTransform()->SetTranslate(pos);

		Math::Vector3 dir = (pos - CVector3::ZERO).Normalize();
		Math::Quaternion rotate = Math::Quaternion::AngleAxis(kPI, CVector3::FORWARD) * Math::Quaternion::LookRotation(dir);
		flamethrowers_[i]->GetTransform()->SetRotate(rotate);
	}
}
