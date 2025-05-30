#include "JetEngine.h"
#include "Engine/System/Manager/ParticleManager.h"

JetEngine::~JetEngine() {
	Finalize();
}

void JetEngine::Finalize() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Init() {
	BaseGameObject::Init();
	SetName("JetEngine");
	SetObject("jet.obj");

	transform_->translate_ = { 0.0f, 1.7f, -0.5f };
	transform_->rotation_ = Quaternion::AngleAxis(45.0f * kToRadian, CVector3::RIGHT);

	// -------------------------------------
	// effectの設定
	ParticleManager* manager = ParticleManager::GetInstance();
	jetParticles_ = manager->CrateParticle("JetParticle");
	jetBornParticles_ = manager->CrateParticle("JetBornParticle");
	jetEnergyParticles_ = manager->CrateParticle("JetEnergyParticles");

	jetParticles_->SetParent(transform_->GetWorldMatrix());
	jetBornParticles_->SetParent(transform_->GetWorldMatrix());
	jetEnergyParticles_->SetParent(transform_->GetWorldMatrix());

	cylinder_ = std::make_unique<GeometryObject>();
	cylinder_->Set<CylinderGeometry>(32, 1.0f, 1.0f, 1.f);
	cylinder_->GetMaterial()->SetIsLighting(false);
	cylinder_->SetEditorWindow();

	cylinder_->GetTransform()->SetParent(transform_->GetWorldMatrix());

	jetEngineBurn_ = std::make_unique<JetEngineBurn>();
	jetEngineBurn_->Init();
	jetEngineBurn_->GetWorldTransform()->SetParent(transform_->GetWorldMatrix());

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Update() {
	BaseGameObject::Update();
	jetParticles_->Update();
	jetBornParticles_->Update();
	jetEnergyParticles_->Update();

	cylinder_->Update();

	jetEngineBurn_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Draw() const {
	BaseGameObject::Draw();

	Engine::SetPSOObj(Object3dPSO::TextureBlend);
	jetEngineBurn_->Draw();
	Engine::SetPSOObj(Object3dPSO::Normal);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void JetEngine::Debug_Gui() {
	if (ImGui::CollapsingHeader("Jet")) {
		BaseGameObject::Debug_Gui();
	}

	if(ImGui::CollapsingHeader("jetEngineBurn")) {
		jetEngineBurn_->Debug_Gui();
	}
}
#endif // _DEBUG