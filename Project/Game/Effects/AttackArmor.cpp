#include "AttackArmor.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/System/Manager/GpuParticleManager.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Math/Easing.h"
#include "Game/Information/ColliderCategory.h"

AttackArmor::~AttackArmor() {
	shaderGraph_.reset();
	object_->SetIsDestroy(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void AttackArmor::Debug_Gui() {
	parameter_.shaderGraphPath = shaderGraph_->GetPath();
	object_->Debug_Gui();
	ImGui::Separator();
	ImGui::BulletText("パラメータ");
	parameter_.Debug_Gui();
	shaderGraph_->Debug_Gui();
	ImGui::Separator();
	if (ImGui::Button("start")) {
		isStart_ = true;
		upScaleTimer_.timer_ = 0;
	}

	upScaleTimer_.targetTime_ = parameter_.upScaleTargetTime;
	disappearTimer_.targetTime_ = parameter_.disappearTargetTime;
}

void AttackArmor::Parameter::Debug_Gui() {
	ImGui::DragFloat3("uvScale", &uvScale.x, 0.1f);
	ImGui::DragFloat3("upScale", &upScale.x, 0.1f);
	ImGui::DragFloat("大きくなる時間", &upScaleTargetTime, 0.1f);
	ImGui::DragFloat("消える時間", &disappearTargetTime, 0.1f);
	ImGui::DragFloat("カメラシェイクの強さ", &cameraShakeStrength, 0.1f);
	ImGui::DragFloat("カメラシェイクの時間", &cameraShakeTime, 0.1f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void AttackArmor::Init() {
	parameter_.Load();

	// objectの設定
	object_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<AOENGINE::BaseGameObject>("AttackArmor", "Object_Add.json");
	object_->SetObject("sphere.obj");
	object_->SetEnableShadow(false);
	object_->SetIsLighting(false);
	object_->SetCollider(ColliderTags::Boss::attackArmor, ColliderShape::Sphere);

	// Colliderの設定
	collider_ = object_->GetCollider(ColliderTags::Boss::attackArmor);
	collider_->SetTarget(ColliderTags::Player::own);
	collider_->SetIsTrigger(true);
	collider_->SetIsStatic(false);
	
	transform_ = object_->GetTransform();
	
	// shaderGraphの設定
	shaderGraph_ = std::make_unique<AOENGINE::ShaderGraph>();
	shaderGraph_->Init("armor.json");
	shaderGraph_->Load(parameter_.shaderGraphPath);

	// materialの設定
	for (auto& material : object_->GetMaterials()) {
		material.second->SetShaderGraph(shaderGraph_.get());
		material.second->SetUvScale(parameter_.uvScale);
	}

	// parameterの設定
	isFinish_ = false;
	isStart_ = false;
	isDisappear_ = false;
	upScaleTimer_ = AOENGINE::Timer(parameter_.upScaleTargetTime);
	disappearTimer_ = AOENGINE::Timer(parameter_.disappearTargetTime);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void AttackArmor::Update() {
	if (object_) {
		shaderGraph_->Update();

		// scaleを大きくする
		if (isStart_) {
			ScaleUp();
		}

		// 消える
		if (isDisappear_) {
			Disappear();
		}

		collider_->SetRadius(transform_->GetScale().x);
	}
}

void AttackArmor::Start(const Math::Vector3& _pos) {
	isStart_ = true;
	transform_->SetTranslate(_pos);

	// effectの設定
	particle_ = AOENGINE::GpuParticleManager::GetInstance()->CreateEmitter("attackArmorParticle");
	particle_->SetParent(transform_->GetWorldMatrix());
	particle_->SetIsStop(false);
}


///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 拡大する
///////////////////////////////////////////////////////////////////////////////////////////////

void AttackArmor::ScaleUp() {
	if (upScaleTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		Math::Vector3 scale = Math::Vector3::Lerp(CVector3::ZERO, parameter_.upScale, Ease::In::Sine(upScaleTimer_.t_));
		transform_->SetScale(scale);

		disappearTimer_.timer_ = 0;
	} else {
		isDisappear_ = true;
	}
}

void AttackArmor::Disappear() {
	if (disappearTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		for (auto& material : object_->GetMaterials()) {
			material.second->SetDiscardValue(disappearTimer_.t_);
		}
	} else {
		isFinish_ = true;
		particle_->SetIsStop(true);
		for (auto& material : object_->GetMaterials()) {
			material.second->SetDiscardValue(1.0f);
		}
		object_->SetIsDestroy(true);
	}
}
