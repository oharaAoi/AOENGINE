#include "JetEngine.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Module/Geometry/Polygon/SphereGeometry.h"

JetEngine::~JetEngine() {
	Finalize();
}

void JetEngine::Finalize() {
}

void JetEngine::Parameter::Debug_Gui() {
	ImGui::DragFloat("engineIncline", &engineIncline);
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

	object_ = SceneRenderer::GetInstance()->AddObject<BaseGameObject>(GetName(), "Object_Normal.json");
	object_->SetObject("jet.obj");

	transform_ = object_->GetTransform();
	transform_->srt_.translate = { 0.0f, 2.7f, -0.5f };
	transform_->srt_.rotate = Quaternion::AngleAxis(30.0f * kToRadian, CVector3::RIGHT);

	// -------------------------------------
	// effectの親にする空のTransform作成
	// -------------------------------------

	burnParentTransform_ = Engine::CreateWorldTransform();
	burnParentTransform_->SetParent(transform_->GetWorldMatrix());

	// -------------------------------------
	// effectの設定
	// -------------------------------------

	jetEngineBurn_ = SceneRenderer::GetInstance()->AddObject<JetEngineBurn>("JetBurn", "Object_Dissolve.json", 100);
	jetEngineBurn_->Init();
	jetEngineBurn_->GetWorldTransform()->SetParent(burnParentTransform_->GetWorldMatrix());

	jetEngineBurn2_ = SceneRenderer::GetInstance()->AddObject<JetEngineBurn>("JetBurn", "Object_Dissolve.json", 100);
	jetEngineBurn2_->Init();
	jetEngineBurn2_->GetWorldTransform()->SetParent(burnParentTransform_->GetWorldMatrix());

	burnParticle_ = ParticleManager::GetInstance()->CrateParticle("BurnParticle");
	burnParticle_->SetParent(transform_->GetWorldMatrix());

	object_->AddChild(jetEngineBurn_);
	
	isBoostMode_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Update(float diftX) {
	Quaternion engineRotate = Quaternion::AngleAxis(diftX * param_.engineIncline, CVector3::FORWARD);
	Quaternion rotate = Quaternion::Slerp(transform_->GetRotate(), engineRotate, 0.1f);
	transform_->SetRotate(rotate);
	transform_->Update();
	burnParentTransform_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Debug_Gui() {
	if (ImGui::TreeNode("Engine")) {
		transform_->Debug_Gui();
		ImGui::TreePop();
	}

	if (ImGui::CollapsingHeader("burnParent")) {
		burnParentTransform_->Debug_Gui();
	}

	if (ImGui::TreeNode("Burn1")) {
		jetEngineBurn_->Debug_Gui();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Burn2")) {
		jetEngineBurn2_->Debug_Gui();
		ImGui::TreePop();
	}

	param_.Debug_Gui();
}

void JetEngine::JetIsStop() {
	jetEngineBurn_->BoostOff();
	jetEngineBurn2_->BoostOff();
	burnParticle_->SetIsStop(true);
}

void JetEngine::JetIsStart() {
	jetEngineBurn_->BoostOn();
	jetEngineBurn2_->BoostOn();
	burnParticle_->SetIsStop(false);
}
