#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/Lib/Math/Matrix4x4.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Lib/Math/Vector3.h"

namespace AOENGINE {

class BaseParticles;
class GpuParticleEmitter;
class WorldTransform;

enum class ParticleEffectNodeType {
	Cpu,
	Gpu,
};

struct ParticleEffectNode {
	std::string name;
	std::string asset;
	ParticleEffectNodeType type = ParticleEffectNodeType::Cpu;
	float delay = 0.0f;
	Math::Vector3 position = CVector3::ZERO;
	Math::Quaternion rotation{};
	Math::Vector3 scale{ 1.0f, 1.0f, 1.0f };
};

struct ParticleEffectAsset {
	static constexpr uint32_t kCurrentVersion = 1;

	uint32_t version = kCurrentVersion;
	std::string name;
	float duration = 0.0f; // 0以下は明示的にStopされるまで生存する
	std::vector<ParticleEffectNode> nodes;

	json ToJson() const;
	static ParticleEffectAsset FromJson(const json& source, const std::string& fallbackName = {});
};

class ParticleEffectInstance {
public:
	ParticleEffectInstance() = default;
	~ParticleEffectInstance();
	ParticleEffectInstance(const ParticleEffectInstance&) = delete;
	ParticleEffectInstance& operator=(const ParticleEffectInstance&) = delete;

	bool Init(const ParticleEffectAsset& asset, const Math::Vector3& position);
	void Update(float deltaTime);
	void Stop();
	void Restart();
	void SetPosition(const Math::Vector3& position);
	void SetParent(WorldTransform* parent);
	bool IsFinished() const { return finished_; }
	const std::string& GetName() const { return asset_.name; }

private:
	struct RuntimeNode {
		ParticleEffectNode definition;
		std::variant<BaseParticles*, GpuParticleEmitter*> emitter;
		bool started = false;
	};

	void StartNode(RuntimeNode& node);
	void ApplyTransforms();
	void ReleaseEmitters();

	ParticleEffectAsset asset_;
	std::vector<RuntimeNode> nodes_;
	Math::Vector3 position_ = CVector3::ZERO;
	Math::Matrix4x4 rootMatrix_ = Math::Matrix4x4::MakeUnit();
	WorldTransform* parent_ = nullptr;
	float elapsedTime_ = 0.0f;
	bool stopped_ = false;
	bool finished_ = false;
};

}
