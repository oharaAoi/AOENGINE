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

	//ParticleManager* manager = ParticleManager::GetInstance();
	/*jetBurn_ = manager->CrateParticle("JetBorn");
	jetBurn_->SetParent(transform_->GetWorldMatrix());*/

	/*jetEnergyParticles_ = manager->CrateParticle("JetFrea");
	jetEnergyParticles_->SetParent(transform_->GetWorldMatrix());*/

	// -------------------------------------
	// effectの設定
	// -------------------------------------

	jetEngineBurn_ = SceneRenderer::GetInstance()->AddObject<JetEngineBurn>("JetBurn", "Object_Dissolve.json", 100);
	jetEngineBurn_->Init();
	jetEngineBurn_->GetWorldTransform()->SetParent(transform_->GetWorldMatrix());

	jetEngineBurn_2 = SceneRenderer::GetInstance()->AddObject<JetEngineBurn>("JetBurn", "Object_Dissolve.json", 100);
	jetEngineBurn_2->Init();
	jetEngineBurn_2->GetWorldTransform()->SetParent(transform_->GetWorldMatrix());
	Vector3 scale = jetEngineBurn_2->GetWorldTransform()->GetScale();
	jetEngineBurn_2->GetWorldTransform()->SetScale(scale * 0.9f);
	
	object_->AddChild(jetEngineBurn_);
	object_->AddChild(jetEngineBurn_2);

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
	//jetBurn_->SetIsStop(true);
	//jetEnergyParticles_->SetIsStop(true);
	jetEngineBurn_->BoostOff();
	jetEngineBurn_2->BoostOff();
}

void JetEngine::JetIsStart() {
	//jetBurn_->SetIsStop(false);
	//jetEnergyParticles_->SetIsStop(false);
	jetEngineBurn_->BoostOn();
	jetEngineBurn_2->BoostOn();
}
