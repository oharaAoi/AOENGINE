#pragma once

#include <array>
#include <memory>
#include <string>
#include <d3d12.h>
#include <wrl.h>

#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Module/Components/IComponent.h"
#include "Engine/System/Manager/ParticleEffectManager.h"

namespace AOENGINE {

class BaseGameObject;
class Mesh;
class WorldTransform;

struct ExplosionEffectSettings {
	bool enabled = true;
	bool autoPlay = true;
	float duration = 1.0f;
	float coreStartScale = 0.05f;
	float coreEndScale = 2.0f;
	float shockwaveEndScale = 3.0f;
	float noiseScale = 5.0f;
	float noiseStrength = 0.2f;
	float turbulence = 1.6f;
	float animationSpeed = 0.8f;
	float density = 1.15f;
	float rimIntensity = 0.65f;
	float emissiveIntensity = 5.0f;
	float shockwaveWidth = 0.2f;
	Color innerColor{ 1.0f, 0.9f, 0.35f, 1.0f };
	Color outerColor{ 1.0f, 0.08f, 0.0f, 1.0f };
	std::string particleEffect;
};

class ExplosionEffectComponent final : public IComponent {
public:
	ExplosionEffectComponent() = default;
	~ExplosionEffectComponent() override;
	ExplosionEffectComponent(const ExplosionEffectComponent&) = delete;
	ExplosionEffectComponent& operator=(const ExplosionEffectComponent&) = delete;

	void Init(BaseGameObject& owner);
	void Update(const BaseGameObject& owner);
	void EditorUpdate(const BaseGameObject& owner);
	void Draw() const;
	void Finalize();
	void Play();
	void Stop();

	bool IsPlaying() const { return playing_; }
	bool IsFinished() const { return finished_; }
	const ExplosionEffectSettings& GetSettings() const { return settings_; }
	void SetSettings(const ExplosionEffectSettings& settings);

private:
	struct alignas(16) ShaderParameters {
		float progress = 0.0f;
		float elapsedTime = 0.0f;
		float noiseStrength = 0.0f;
		float noiseScale = 1.0f;
		Color innerColor{};
		Color outerColor{};
		Math::Vector3 eyePosition{};
		float mode = 0.0f;
		float emissiveIntensity = 1.0f;
		float shockwaveWidth = 0.2f;
		float alpha = 1.0f;
		float animationSpeed = 0.8f;
		float turbulence = 1.6f;
		float density = 1.15f;
		float rimIntensity = 0.65f;
		float padding = 0.0f;
	};

	void UpdateTransforms(const BaseGameObject& owner);
	void UpdateParameter(uint32_t index, float mode);
	void DrawLayer(uint32_t index) const;

	ExplosionEffectSettings settings_;
	std::shared_ptr<Mesh> sphereMesh_;
	std::array<std::unique_ptr<WorldTransform>, 2> transforms_;
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> parameterBuffers_;
	std::array<ShaderParameters*, 2> parameters_{};
	ParticleEffectHandle particleHandle_{};
	Math::Vector3 worldPosition_{};
	float elapsedTime_ = 0.0f;
	bool playing_ = false;
	bool finished_ = false;
	bool autoPlayPending_ = false;
};

}
