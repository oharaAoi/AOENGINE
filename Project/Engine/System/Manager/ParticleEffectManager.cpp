#include "ParticleEffectManager.h"

#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/ParticleSystem/Effect/ParticleEffect.h"

using namespace AOENGINE;

ParticleEffectManager* ParticleEffectManager::GetInstance() {
	static ParticleEffectManager instance;
	return &instance;
}

ParticleEffectHandle ParticleEffectManager::Play(const std::string& effectName, const Math::Vector3& position) {
	ParticleEffectAsset asset;
	try {
		asset = ParticleEffectAsset::FromJson(JsonItems::GetData("Composite", effectName), effectName);
	} catch (...) {
		return {};
	}

	uint32_t index = 0;
	for (; index < slots_.size(); ++index) {
		if (!slots_[index].instance) { break; }
	}
	if (index == slots_.size()) { slots_.emplace_back(); }

	auto instance = std::make_unique<ParticleEffectInstance>();
	if (!instance->Init(asset, position)) { return {}; }
	slots_[index].instance = std::move(instance);
	return { index, slots_[index].generation };
}

void ParticleEffectManager::Update() {
	for (Slot& slot : slots_) {
		if (!slot.instance) { continue; }
		slot.instance->Update(GameTimer::DeltaTime());
		if (slot.instance->IsFinished()) {
			slot.instance.reset();
			++slot.generation;
			if (slot.generation == 0) { slot.generation = 1; }
		}
	}
}

void ParticleEffectManager::Finalize() {
	for (Slot& slot : slots_) {
		slot.instance.reset();
		++slot.generation;
	}
	slots_.clear();
}

ParticleEffectInstance* ParticleEffectManager::Resolve(ParticleEffectHandle handle) {
	if (handle.index >= slots_.size()) { return nullptr; }
	Slot& slot = slots_[handle.index];
	return slot.generation == handle.generation ? slot.instance.get() : nullptr;
}

const ParticleEffectInstance* ParticleEffectManager::Resolve(ParticleEffectHandle handle) const {
	if (handle.index >= slots_.size()) { return nullptr; }
	const Slot& slot = slots_[handle.index];
	return slot.generation == handle.generation ? slot.instance.get() : nullptr;
}

bool ParticleEffectManager::IsAlive(ParticleEffectHandle handle) const { return Resolve(handle) != nullptr; }
void ParticleEffectManager::Stop(ParticleEffectHandle handle) { if (auto* value = Resolve(handle)) { value->Stop(); } }
void ParticleEffectManager::Restart(ParticleEffectHandle handle) { if (auto* value = Resolve(handle)) { value->Restart(); } }
void ParticleEffectManager::Destroy(ParticleEffectHandle handle) {
	if (handle.index >= slots_.size() || slots_[handle.index].generation != handle.generation) { return; }
	Slot& slot = slots_[handle.index];
	slot.instance.reset();
	++slot.generation;
	if (slot.generation == 0) { slot.generation = 1; }
}
void ParticleEffectManager::SetPosition(ParticleEffectHandle handle, const Math::Vector3& position) { if (auto* value = Resolve(handle)) { value->SetPosition(position); } }
void ParticleEffectManager::SetParent(ParticleEffectHandle handle, WorldTransform* parent) { if (auto* value = Resolve(handle)) { value->SetParent(parent); } }

bool ParticleEffectHandle::IsValid() const { return index != UINT32_MAX && generation != 0; }
bool ParticleEffectHandle::IsAlive() const { return ParticleEffectManager::GetInstance()->IsAlive(*this); }
void ParticleEffectHandle::Stop() const { ParticleEffectManager::GetInstance()->Stop(*this); }
void ParticleEffectHandle::Restart() const { ParticleEffectManager::GetInstance()->Restart(*this); }
void ParticleEffectHandle::Destroy() const { ParticleEffectManager::GetInstance()->Destroy(*this); }
void ParticleEffectHandle::SetPosition(const Math::Vector3& position) const { ParticleEffectManager::GetInstance()->SetPosition(*this, position); }
void ParticleEffectHandle::SetParent(WorldTransform* parent) const { ParticleEffectManager::GetInstance()->SetParent(*this, parent); }
