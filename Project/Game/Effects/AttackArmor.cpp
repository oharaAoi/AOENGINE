#include "AttackArmor.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Math/Easing.h"

AttackArmor::~AttackArmor() {
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
	disapperTimer_.targetTime_ = parameter_.disapperTargetTime;
}

void AttackArmor::Parameter::Debug_Gui() {
	ImGui::DragFloat3("uvScale", &uvScale.x, 0.1f);
	ImGui::DragFloat3("upScale", &upScale.x, 0.1f);
	ImGui::DragFloat("upScaleTargetTime", &upScaleTargetTime, 0.1f);
	ImGui::DragFloat("disapperTargetTime", &disapperTargetTime, 0.1f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void AttackArmor::Init() {
	parameter_.Load();

	// objectの設定
	object_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<AOENGINE::BaseGameObject>("AttackArmor", "Object_Normal.json");
	object_->SetObject("sphere.obj");

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
	isStart_ = false;
	isDisapper_ = false;
	upScaleTimer_ = AOENGINE::Timer(parameter_.upScaleTargetTime);
	disapperTimer_ = AOENGINE::Timer(parameter_.disapperTargetTime);

	AOENGINE::EditorWindows::AddObjectWindow(this, "AttackArmor");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void AttackArmor::Update() {
	shaderGraph_->Update();

	// scaleを大きくする
	if (isStart_) {
		ScaleUp();
	} 

	// 消える
	if (isDisapper_) {
		Disapper();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 拡大する
///////////////////////////////////////////////////////////////////////////////////////////////

void AttackArmor::ScaleUp() {
	if (upScaleTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		Math::Vector3 scale = Math::Vector3::Lerp(CVector3::ZERO, parameter_.upScale, Ease::In::Sine(upScaleTimer_.t_));
		transform_->SetScale(scale);

		disapperTimer_.timer_ = 0;
	} else {
		isDisapper_ = true;
	}
}

void AttackArmor::Disapper() {
	if (disapperTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		for (auto& material : object_->GetMaterials()) {
			material.second->SetDiscardValue(disapperTimer_.t_);
		}
	}
}
