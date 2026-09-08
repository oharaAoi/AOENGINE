#include "ExplosionEffectComponent.h"

#include <algorithm>
#include "Engine/Core/Engine.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Geometry/Polygon/SphereGeometry.h"
#include "Engine/Render/Render.h"

using namespace AOENGINE;

ExplosionEffectComponent::~ExplosionEffectComponent() { Finalize(); }

void ExplosionEffectComponent::Init(BaseGameObject& owner) {
	Finalize();
	SphereGeometry geometry;
	geometry.Init({ 1.0f, 1.0f }, 24, "explosionSphere");
	sphereMesh_ = std::make_shared<Mesh>();
	sphereMesh_->Init(GraphicsContext::GetInstance()->GetDevice(), geometry.GetVertex(), geometry.GetIndex());
	for (uint32_t i = 0; i < transforms_.size(); ++i) {
		transforms_[i] = std::make_unique<WorldTransform>();
		transforms_[i]->Init();
		parameterBuffers_[i] = CreateBufferResource(GraphicsContext::GetInstance()->GetDevice(), sizeof(ShaderParameters));
		parameterBuffers_[i]->Map(0, nullptr, reinterpret_cast<void**>(&parameters_[i]));
		*parameters_[i] = ShaderParameters{};
	}
	autoPlayPending_ = settings_.autoPlay;
	UpdateTransforms(owner);
}

void ExplosionEffectComponent::Play() {
	elapsedTime_ = 0.0f;
	playing_ = true;
	finished_ = false;
	autoPlayPending_ = false;
	if (particleHandle_.IsAlive()) { particleHandle_.Destroy(); }
	if (!settings_.particleEffect.empty()) {
		particleHandle_ = ParticleEffectManager::GetInstance()->Play(settings_.particleEffect, worldPosition_);
	}
}

void ExplosionEffectComponent::Stop() {
	playing_ = false;
	finished_ = true;
	if (particleHandle_.IsAlive()) { particleHandle_.Destroy(); }
	particleHandle_ = {};
}

void ExplosionEffectComponent::Update(const BaseGameObject& owner) {
	if (!settings_.enabled || !sphereMesh_) { return; }
	UpdateTransforms(owner);
	if (autoPlayPending_) { Play(); }
	if (!playing_) { return; }
	elapsedTime_ += GameTimer::DeltaTime();
	if (elapsedTime_ >= settings_.duration) {
		elapsedTime_ = settings_.duration;
		playing_ = false;
		finished_ = true;
	}
	UpdateTransforms(owner);
}

void ExplosionEffectComponent::EditorUpdate(const BaseGameObject& owner) {
	if (!settings_.enabled || !sphereMesh_) { return; }
	if (playing_) {
		elapsedTime_ += GameTimer::DeltaTime();
		if (elapsedTime_ >= settings_.duration) {
			elapsedTime_ = settings_.duration;
			playing_ = false;
			finished_ = true;
		}
	}
	UpdateTransforms(owner);
}

void ExplosionEffectComponent::UpdateTransforms(const BaseGameObject& owner) {
	const WorldTransform* parent = owner.GetTransform();
	if (!parent) { return; }
	worldPosition_ = parent->GetWorldPos();
	const float duration = (std::max)(settings_.duration, 0.01f);
	const float progress = std::clamp(elapsedTime_ / duration, 0.0f, 1.0f);
	const float eased = 1.0f - (1.0f - progress) * (1.0f - progress);
	const float coreScale = std::lerp(settings_.coreStartScale, settings_.coreEndScale, eased);
	const float shockScale = std::lerp(settings_.coreStartScale, settings_.shockwaveEndScale, eased);
	const float scales[] = { coreScale, shockScale };
	for (uint32_t i = 0; i < transforms_.size(); ++i) {
		transforms_[i]->PostUpdate();
		transforms_[i]->SetParent(parent->GetWorldMatrix());
		transforms_[i]->SetScale({ scales[i], scales[i], scales[i] });
		transforms_[i]->SetTranslate(CVector3::ZERO);
		transforms_[i]->Update();
		UpdateParameter(i, static_cast<float>(i));
	}
}

void ExplosionEffectComponent::UpdateParameter(uint32_t index, float mode) {
	if (!parameters_[index]) { return; }
	const float duration = (std::max)(settings_.duration, 0.01f);
	parameters_[index]->progress = std::clamp(elapsedTime_ / duration, 0.0f, 1.0f);
	parameters_[index]->elapsedTime = elapsedTime_;
	parameters_[index]->noiseStrength = settings_.noiseStrength;
	parameters_[index]->noiseScale = settings_.noiseScale;
	parameters_[index]->innerColor = settings_.innerColor;
	parameters_[index]->outerColor = settings_.outerColor;
	parameters_[index]->eyePosition = Render::GetEyePos();
	parameters_[index]->mode = mode;
	parameters_[index]->emissiveIntensity = settings_.emissiveIntensity;
	parameters_[index]->shockwaveWidth = settings_.shockwaveWidth;
	parameters_[index]->alpha = 1.0f - parameters_[index]->progress;
	parameters_[index]->animationSpeed = settings_.animationSpeed;
	parameters_[index]->turbulence = settings_.turbulence;
	parameters_[index]->density = settings_.density;
	parameters_[index]->rimIntensity = settings_.rimIntensity;
}

void ExplosionEffectComponent::Draw() const {
	if (!settings_.enabled || (!playing_ && !autoPlayPending_) || !sphereMesh_) { return; }
	DrawLayer(0);
	DrawLayer(1);
}

void ExplosionEffectComponent::DrawLayer(uint32_t index) const {
	Pipeline* pipeline = Engine::SetPipeline(PSOType::Object3d, "Object_Explosion.json");
	ID3D12GraphicsCommandList* commandList = GraphicsContext::GetInstance()->GetCommandList();
	sphereMesh_->Bind(commandList);
	transforms_[index]->BindCommandList(commandList, pipeline->GetRootSignatureIndex("gWorldTransformMatrix"));
	Render::GetViewProjection()->BindCommandList(commandList, pipeline->GetRootSignatureIndex("gViewProjectionMatrix"));
	Render::GetViewProjection()->BindCommandListPrev(commandList, pipeline->GetRootSignatureIndex("gViewProjectionMatrixPrev"));
	const D3D12_GPU_VIRTUAL_ADDRESS parameterAddress = parameterBuffers_[index]->GetGPUVirtualAddress();
	commandList->SetGraphicsRootConstantBufferView(
		pipeline->GetRootSignatureIndex("gExplosionVertex"), parameterAddress);
	commandList->SetGraphicsRootConstantBufferView(
		pipeline->GetRootSignatureIndex("gExplosionPixel"), parameterAddress);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->DrawIndexedInstanced(sphereMesh_->GetIndexNum(), 1, 0, 0, 0);
}

void ExplosionEffectComponent::SetSettings(const ExplosionEffectSettings& settings) {
	settings_ = settings;
	settings_.duration = (std::max)(settings_.duration, 0.01f);
	settings_.coreStartScale = (std::max)(settings_.coreStartScale, 0.001f);
	settings_.coreEndScale = (std::max)(settings_.coreEndScale, 0.001f);
	settings_.shockwaveEndScale = (std::max)(settings_.shockwaveEndScale, 0.001f);
	settings_.shockwaveWidth = std::clamp(settings_.shockwaveWidth, 0.001f, 1.0f);
	settings_.noiseScale = (std::max)(settings_.noiseScale, 0.01f);
	settings_.noiseStrength = (std::max)(settings_.noiseStrength, 0.0f);
	settings_.turbulence = (std::max)(settings_.turbulence, 0.0f);
	settings_.animationSpeed = (std::max)(settings_.animationSpeed, 0.0f);
	settings_.density = (std::max)(settings_.density, 0.01f);
	settings_.rimIntensity = (std::max)(settings_.rimIntensity, 0.0f);
	autoPlayPending_ = settings_.autoPlay && !playing_ && !finished_;
}

void ExplosionEffectComponent::Finalize() {
	if (particleHandle_.IsAlive()) { particleHandle_.Destroy(); }
	particleHandle_ = {};
	for (uint32_t i = 0; i < transforms_.size(); ++i) {
		parameters_[i] = nullptr;
		parameterBuffers_[i].Reset();
		transforms_[i].reset();
	}
	sphereMesh_.reset();
	playing_ = false;
}
