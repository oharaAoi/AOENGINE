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

	jetParticles_ = std::make_unique<JetParticles>();
	jetParticles_->Init("JetParticle");
	jetParticles_->SetParent(transform_->GetWorldMatrix());
	manager->AddParticleList(jetParticles_.get());

	jetBornParticles_ = std::make_unique<JetBornParticles>();
	jetBornParticles_->Init("JetBornParticle");
	jetBornParticles_->SetParent(transform_->GetWorldMatrix());
	manager->AddParticleList(jetBornParticles_.get());

	jetEnergyParticles_ = std::make_unique<JetEnergyParticles>();
	jetEnergyParticles_->Init("JetEnergyParticles");
	jetEnergyParticles_->SetParent(transform_->GetWorldMatrix());
	manager->AddParticleList(jetEnergyParticles_.get());
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