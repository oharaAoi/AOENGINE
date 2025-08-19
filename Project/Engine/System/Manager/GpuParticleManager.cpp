#include "GpuParticleManager.h"
#include "Render.h"
#include "Engine/Core/GraphicsContext.h"
//#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/Lib/Json/JsonItems.h"

GpuParticleManager::~GpuParticleManager() {
	emitterList_.clear();
	fileds_.clear();
	renderer_.reset();
}

GpuParticleManager* GpuParticleManager::GetInstance() {
	static GpuParticleManager instance;
	return &instance;
}

void GpuParticleManager::Finalize() {
	emitterList_.clear();
	fileds_.clear();
	renderer_.reset();
}

void GpuParticleManager::Debug_Gui() {
}

void GpuParticleManager::Init() {
	SetName("GpuParticleManager");

	renderer_ = std::make_unique<GpuParticleRenderer>();
	renderer_->Init(102400);

	//EditorWindows::AddObjectWindow(this, "GpuParticleManager");
}

void GpuParticleManager::Update() {
	GraphicsContext* ctx = GraphicsContext::GetInstance();
	ID3D12GraphicsCommandList*  commandList = ctx->GetCommandList();
	for (auto& field : fileds_) {
		field->Update();
		field->Execute(commandList);
	}

	renderer_->SetView(Render::GetViewProjectionMat(), Render::GetCameraRotate().MakeMatrix());
	renderer_->Update();

	for (auto& emitter : emitterList_) {
		emitter->Update();
	}
}

void GpuParticleManager::Draw() const {
	renderer_->Draw();

	/*for (auto& emitter : emitterList_) {
		emitter->DrawShape();
	}*/
}

GpuParticleEmitter* GpuParticleManager::CreateEmitter(const std::string& particlesFile) {
	auto& newParticles = emitterList_.emplace_back(std::make_unique<GpuParticleEmitter>());
	newParticles->Init(particlesFile);
	newParticles->SetParticleResourceHandle(renderer_->GetResourceHandle());
	newParticles->SetFreeListIndexHandle(renderer_->GetFreeListIndexHandle());
	newParticles->SetFreeListHandle(renderer_->GetFreeListHandle());
	newParticles->SetMaxParticleResource(renderer_->GetMaxBufferResource());
	AddChild(newParticles.get());
	return newParticles.get();
}

GpuParticleField* GpuParticleManager::CreateField(const std::string& particlesFile) {
	auto& field = fileds_.emplace_back(std::make_unique<GpuParticleField>());
	field->Init(10240);
	field->SetName(particlesFile);
	field->SetParticleResourceHandle(renderer_->GetResourceHandle());
	field->SetMaxParticleResource(renderer_->GetMaxBufferResource());
	AddChild(field.get());
	return field.get();
}

void GpuParticleManager::AddEmitter(GpuParticleEmitter* _emitter) {
	_emitter->SetParticleResourceHandle(renderer_->GetResourceHandle());
	_emitter->SetFreeListIndexHandle(renderer_->GetFreeListIndexHandle());
	_emitter->SetFreeListHandle(renderer_->GetFreeListHandle());
	_emitter->SetMaxParticleResource(renderer_->GetMaxBufferResource());
	AddChild(_emitter);
}
