#include "JetEngine.h"
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
	BaseGameObject::Init();
	SetName("JetEngine");
	SetObject("jet.obj");

	transform_->translate_ = { 0.0f, 1.7f, -0.5f };
	transform_->rotation_ = Quaternion::AngleAxis(45.0f * kToRadian, CVector3::RIGHT);

	// -------------------------------------
	// effectの設定
	ParticleManager* manager = ParticleManager::GetInstance();
	jetBurn_ = manager->CrateParticle("JetBorn");
	jetBornParticles_ = manager->CrateParticle("JetBornParticle");
	jetEnergyParticles_ = manager->CrateParticle("JetEnergyParticles");

	jetBurn_->SetParent(transform_->GetWorldMatrix());
	jetBornParticles_->SetParent(transform_->GetWorldMatrix());
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
	BaseGameObject::Update();
	jetBurn_->Update();
	//jetBornParticles_->Update();
	jetEnergyParticles_->Update();

	cylinder_->Update();

	jetEngineBurn_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Draw() const {
	BaseGameObject::Draw();

	/*Engine::SetPSOObj(Object3dPSO::TextureBlend);
	jetEngineBurn_->Draw();
	Engine::SetPSOObj(Object3dPSO::Normal);*/
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

void JetEngine::JetIsStop() {
	jetBurn_->SetIsStop(true);
	jetEnergyParticles_->SetIsStop(true);
}

void JetEngine::JetIsStart() {
	jetBurn_->SetIsStop(false);
	jetEnergyParticles_->SetIsStop(false);
}
