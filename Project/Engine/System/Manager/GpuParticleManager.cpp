#include "GpuParticleManager.h"
#include "Engine/Render/Render.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/System/Editor/Window/EditorWindows.h"

using namespace AOENGINE;

GpuParticleManager::~GpuParticleManager() {
	emitterList_.clear();
	fileds_.clear();
	renderer_.reset();
}

AOENGINE::GpuParticleManager* AOENGINE::GpuParticleManager::GetInstance() {
	static GpuParticleManager instance;
	return &instance;
}

void GpuParticleManager::Finalize() {
	retiredEmitters_.splice(retiredEmitters_.end(), emitterList_);
	retiredFields_.splice(retiredFields_.end(), fileds_);
	if (renderer_) { retiredRenderers_.push_back(std::move(renderer_)); }
}

void GpuParticleManager::ReleaseRetiredResources() {
	retiredEmitters_.clear();
	retiredFields_.clear();
	retiredRenderers_.clear();
}

void GpuParticleManager::Debug_Gui() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleManager::Init() {
	SetName("GpuParticleManager");

	renderer_ = std::make_unique<GpuParticleRenderer>();
	renderer_->Init(instance_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleManager::Update() {
	AOENGINE::GraphicsContext* ctx = AOENGINE::GraphicsContext::GetInstance();
	ID3D12GraphicsCommandList*  commandList = ctx->GetCommandList();
	for (auto& field : fileds_) {
		if (!field->IsActive()) { continue; }
		field->Update();
		field->Execute(commandList);
	}

	renderer_->SetView(AOENGINE::Render::GetViewProjectionMat(), AOENGINE::Render::GetCameraRotate().MakeMatrix());
	renderer_->Update();

	for (auto& emitter : emitterList_) {
		emitter->Update();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleManager::Draw(const Math::Frustum& frustum) const {
	// Game/Editorそれぞれで、描画直前に現在のCamera行列へ切り替える。
	renderer_->SetView(AOENGINE::Render::GetViewProjectionMat(), AOENGINE::Render::GetCameraRotate().MakeMatrix());
	renderer_->Draw(&frustum);

#ifdef _DEVELOPMENT
	if (AOENGINE::EditorWindows::GetInstance()->GetGridDraw()) {
		for (auto& emitter : emitterList_) {
			emitter->DrawShape(AOENGINE::Render::GetViewProjectionMat());
		}
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Particleの追加
///////////////////////////////////////////////////////////////////////////////////////////////

GpuParticleEmitter* GpuParticleManager::CreateEmitter(const std::string& particlesFile) {
	auto& newParticles = emitterList_.emplace_back(std::make_unique<GpuParticleEmitter>());
	newParticles->Init(particlesFile);
	newParticles->SetParticleResourceHandle(renderer_->GetResourceHandle());
	newParticles->SetFreeListIndexHandle(renderer_->GetFreeListIndexHandle());
	newParticles->SetFreeListHandle(renderer_->GetFreeListHandle());
	newParticles->SetMaxParticleResource(renderer_->GetMaxBufferResource());
	return newParticles.get();
}

GpuParticleField* GpuParticleManager::CreateField(const std::string& particlesFile) {
	auto& field = fileds_.emplace_back(std::make_unique<GpuParticleField>());
	field->Init(instance_);
	field->SetName(particlesFile);
	field->SetParticleResourceHandle(renderer_->GetResourceHandle());
	field->SetMaxParticleResource(renderer_->GetMaxBufferResource());
	if (sceneWorld_) {
		sceneWorld_->AddExternalObject(*field, particlesFile);
		sceneWorld_->SetParent(field->GetHandle(), GetHandle());
	}
	return field.get();
}

void GpuParticleManager::DeleteEmitter(GpuParticleEmitter* _emitter) {
	for (auto it = emitterList_.begin(); it != emitterList_.end(); ) {
		if (it->get() == _emitter) {
			it = emitterList_.erase(it); // 要素の削除とイテレータ更新
		} else {
			++it;
		}
	}
}

void GpuParticleManager::AddEmitter(GpuParticleEmitter* _emitter) {
	_emitter->SetParticleResourceHandle(renderer_->GetResourceHandle());
	_emitter->SetFreeListIndexHandle(renderer_->GetFreeListIndexHandle());
	_emitter->SetFreeListHandle(renderer_->GetFreeListHandle());
	_emitter->SetMaxParticleResource(renderer_->GetMaxBufferResource());
}

void GpuParticleManager::RegisterSceneObjects(SceneWorld& sceneWorld) {
	sceneWorld_ = &sceneWorld;
	if (!sceneWorld.IsValid(GetHandle()) || sceneWorld.FindObject(GetHandle()) != this) {
		sceneWorld.AddExternalObject(*this, "GpuParticleManager");
	}

	for (const auto& field : fileds_) {
		if (!sceneWorld.IsValid(field->GetHandle()) || sceneWorld.FindObject(field->GetHandle()) != field.get()) {
			sceneWorld.AddExternalObject(*field, field->GetName());
		}
		sceneWorld.SetParent(field->GetHandle(), GetHandle());
	}
}
