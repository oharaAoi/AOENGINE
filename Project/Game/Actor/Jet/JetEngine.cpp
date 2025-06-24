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

	transform_->translate_ = { 0.0f, 1.7f, -0.5f };
	transform_->rotation_ = Quaternion::AngleAxis(45.0f * kToRadian, CVector3::RIGHT);

	// -------------------------------------
	// effectの設定
	ParticleManager* manager = ParticleManager::GetInstance();
	jetBurn_ = manager->CrateParticle("JetBorn");
	jetEnergyParticles_ = manager->CrateParticle("JetEnergyParticles");

	jetBurn_->SetParent(transform_->GetWorldMatrix());
	jetEnergyParticles_->SetParent(transform_->GetWorldMatrix());

	cylinder_ = std::make_unique<GeometryObject>();
	cylinder_->Set<SphereGeometry>(Vector2(10.0f, 0.5f), 32);
	//cylinder_->GetMaterial()->SetIsLighting(false);
	cylinder_->SetEditorWindow();

	cylinder_->GetTransform()->SetParent(transform_->GetWorldMatrix());

	jetEngineBurn_ = std::make_unique<JetEngineBurn>();
	jetEngineBurn_->Init();
	jetEngineBurn_->GetWorldTransform()->SetParent(transform_->GetWorldMatrix());

	isBoostMode_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Update() {
	cylinder_->Update();

	jetEngineBurn_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Draw() const {
	
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Debug_Gui() {
	if (ImGui::CollapsingHeader("Jet")) {
		transform_->Debug_Gui();
	}

	if(ImGui::CollapsingHeader("jetEngineBurn")) {
		jetEngineBurn_->Debug_Gui();
	}
}

void JetEngine::JetIsStop() {
	jetBurn_->SetIsStop(true);
	jetEnergyParticles_->SetIsStop(true);
}

void JetEngine::JetIsStart() {
	jetBurn_->SetIsStop(false);
	jetEnergyParticles_->SetIsStop(false);
}
