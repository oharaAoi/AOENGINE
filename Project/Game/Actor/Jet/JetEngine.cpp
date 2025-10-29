#include "JetEngine.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Module/Geometry/Polygon/SphereGeometry.h"

JetEngine::~JetEngine() {
	Finalize();
}

void JetEngine::Finalize() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Init() {
	SetName("JetEngine");

	object_ = SceneRenderer::GetInstance()->AddObject<BaseGameObject>(GetName(), "Object_Normal.json");
	object_->SetObject("jet.obj");

	transform_ = object_->GetTransform();
	transform_->srt_.translate = { 0.0f, 2.7f, -0.5f };
	transform_->srt_.rotate = Quaternion::AngleAxis(30.0f * kToRadian, CVector3::RIGHT);

	// -------------------------------------
	// effectの設定
	// -------------------------------------

	jetEngineBurn_ = SceneRenderer::GetInstance()->AddObject<JetEngineBurn>("JetBurn", "Object_Dissolve.json", 100);
	jetEngineBurn_->Init();
	jetEngineBurn_->GetWorldTransform()->SetParent(transform_->GetWorldMatrix());

	jetEngineBurn_->GetWorldTransform()->SetParent(object_->GetTransform()->GetWorldMatrix());
	object_->AddChild(jetEngineBurn_);
	
	isBoostMode_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Update() {
	transform_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Debug_Gui() {
	if (ImGui::TreeNode("Engine")) {
		transform_->Debug_Gui();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Burn")) {
		jetEngineBurn_->Debug_Gui();
		ImGui::TreePop();
	}
}

void JetEngine::JetIsStop() {
	jetEngineBurn_->BoostOff();
}

void JetEngine::JetIsStart() {
	jetEngineBurn_->BoostOn();
}
