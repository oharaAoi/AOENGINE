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
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Update() {
	BaseGameObject::Update();
	jetParticles_->Update(Render::GetCameraRotate());
	jetBornParticles_->Update(Render::GetCameraRotate());
	jetEnergyParticles_->Update(Render::GetCameraRotate());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Draw() const {
	BaseGameObject::Draw();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void JetEngine::Debug_Gui() {
	BaseGameObject::Debug_Gui();
}
#endif // _DEBUG