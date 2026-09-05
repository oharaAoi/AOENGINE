#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "Engine/Lib/Math/Vector3.h"

namespace AOENGINE {

class ParticleEffectInstance;
class WorldTransform;

struct ParticleEffectHandle {
	uint32_t index = UINT32_MAX;
	uint32_t generation = 0;

	bool IsValid() const;
	bool IsAlive() const;
	void Stop() const;
	void Restart() const;
	void Destroy() const;
	void SetPosition(const Math::Vector3& position) const;
	void SetParent(WorldTransform* parent) const;
};

class ParticleEffectManager {
public:
	static ParticleEffectManager* GetInstance();

	ParticleEffectHandle Play(const std::string& effectName, const Math::Vector3& position = CVector3::ZERO);
	void Update();
	void Finalize();

	bool IsAlive(ParticleEffectHandle handle) const;
	void Stop(ParticleEffectHandle handle);
	void Restart(ParticleEffectHandle handle);
	void Destroy(ParticleEffectHandle handle);
	void SetPosition(ParticleEffectHandle handle, const Math::Vector3& position);
	void SetParent(ParticleEffectHandle handle, WorldTransform* parent);

private:
	struct Slot {
		std::unique_ptr<ParticleEffectInstance> instance;
		uint32_t generation = 1;
	};

	ParticleEffectInstance* Resolve(ParticleEffectHandle handle);
	const ParticleEffectInstance* Resolve(ParticleEffectHandle handle) const;

	std::vector<Slot> slots_;
};

}
