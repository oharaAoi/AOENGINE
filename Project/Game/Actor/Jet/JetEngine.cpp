#include "JetEngine.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Lib/GameTimer.h"

JetEngine::~JetEngine() {
	Finalize();
}

void JetEngine::Finalize() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Debug_Gui() {
	if (ImGui::TreeNode("Engine")) {
		transform_->Debug_Gui();
		ImGui::TreePop();
	}

	for (size_t i = 0; i < 2; ++i) {
		std::string name = "Burn" + std::to_string(i);
		if (ImGui::TreeNode(name.c_str())) {
			jetEngineBurn_[i]->Debug_Gui();
			ImGui::TreePop();
		}
	}

	if (ImGui::CollapsingHeader("burnParent")) {
		burnParentTransform_->Debug_Gui();
	}

	param_.Debug_Gui();

	if (ImGui::Button("Stop")) {
		this->JetIsStop();
	}
	ImGui::SameLine();
	if (ImGui::Button("Start")) {
		this->JetIsStart();
	}
}


void JetEngine::Parameter::Debug_Gui() {
	ImGui::DragFloat("engineIncline", &engineIncline);
	ImGui::DragFloat("burnScaleUpTime", &burnScaleUpTime);
	ImGui::DragFloat3("burnMoveScale", &burnMoveScale.x);
	burnMoveScaleCurve.Debug_Gui();
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Init() {
	SetName("JetEngine");
	param_.Load();

	// -------------------------------------
	// 基本の初期化
	// -------------------------------------

	object_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<AOENGINE::BaseGameObject>(GetName(), "Object_Normal.json");
	object_->SetObject("jet.obj");

	transform_ = object_->GetTransform();
	transform_->srt_.translate = { 0.0f, 2.7f, -0.5f };
	transform_->srt_.rotate = Math::Quaternion::AngleAxis(30.0f * kToRadian, CVector3::RIGHT);

	// -------------------------------------
	// effectの親にする空のTransform作成
	// -------------------------------------

	burnParentTransform_ = Engine::CreateWorldTransform();
	burnParentTransform_->SetParent(transform_->GetWorldMatrix());

	// -------------------------------------
	// effectの設定
	// -------------------------------------

	for (size_t i = 0; i < 2; ++i) {
		jetEngineBurn_[i] = std::make_unique<JetEngineBurn>();
		if (i == 0) {
			jetEngineBurn_[i]->Init("jetBurnParameter");
		} else {
			jetEngineBurn_[i]->Init("jetBurnCenterParameter");
		}

		jetEngineBurn_[i]->GetTransform()->SetParent(burnParentTransform_->GetWorldMatrix());

		object_->AddChild(jetEngineBurn_[i].get());
	}

	burnParticle_ = AOENGINE::ParticleManager::GetInstance()->CrateParticle("BurnParticle");
	burnParticle_->SetParent(transform_->GetWorldMatrix());

	// -------------------------------------
	// その他初期化
	// -------------------------------------
	burnScaleUpTimer_.targetTime_ = param_.burnScaleUpTime;
	isBoostMode_ = false;
	isStop_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Update(float diftX) {
	// 炎が大きくなる時間がまだ過ぎていないならその処理を行なう
	if (burnScaleUpTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		float value = param_.burnMoveScaleCurve.BezierValue(burnScaleUpTimer_.t_);

		Math::Vector3 scale;
		if (isStop_) {
			scale = Math::Vector3::Lerp(param_.burnMoveScale, CVector3::ZERO, value);
		} else {
			scale = Math::Vector3::Lerp(CVector3::ZERO, param_.burnMoveScale, value);
		}
		burnParentTransform_->SetScale(scale);
	}

	Math::Quaternion engineRotate = Math::Quaternion::AngleAxis(diftX * param_.engineIncline, CVector3::FORWARD);
	Math::Quaternion rotate = Math::Quaternion::Slerp(transform_->GetRotate(), engineRotate, 0.5f);
	transform_->SetRotate(rotate);
	transform_->Update();
	burnParentTransform_->Update();

	for (size_t i = 0; i < 2; ++i) {
		jetEngineBurn_[i]->Update();
	}
}

void JetEngine::JetIsStop() {
	isStop_ = true;
	burnScaleUpTimer_.timer_ = 0;
	burnParticle_->SetIsStop(true);
}

void JetEngine::JetIsStart() {
	isStop_ = false;
	burnScaleUpTimer_.timer_ = 0;
	burnParticle_->SetIsStop(false);
}
