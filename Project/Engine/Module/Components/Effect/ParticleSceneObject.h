#pragma once

#include "Engine/Module/Components/GameObject/ISceneObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/System/Manager/ParticleEffectManager.h"

namespace AOENGINE {

class BaseParticles;

enum class ParticleSceneAssetType { Cpu, Effect };

/// Scene上に配置するパーティクルのアンカー。実体の所有は各ParticleManagerが行う。
class ParticleSceneObject final : public ISceneObject {
public:
	ParticleSceneObject() = default;
	~ParticleSceneObject() override;

	void Init() override;
	void Update() override;
	void PostUpdate() override {}
	void PreDraw() const override {}
	void Draw() const override {}
	void Manipulate(const ImVec2&, const ImVec2&) override {}
	ScenePersistence GetScenePersistence() const override { return ScenePersistence::SceneData; }
	const char* GetSceneTypeName() const override { return "ParticleSceneObject"; }

	WorldTransform* GetTransform() { return &transform_; }
	const WorldTransform* GetTransform() const { return &transform_; }
	void SetAsset(ParticleSceneAssetType type, const std::string& asset);
	/// ParticleEditor/AssetsWindowから渡されたJSONパスをアセット参照へ変換します。
	void SetAssetFromPath(const std::string& path);
	ParticleSceneAssetType GetAssetType() const { return assetType_; }
	const std::string& GetAsset() const { return asset_; }
	void SetAutoPlay(bool value) { autoPlay_ = value; }
	bool GetAutoPlay() const { return autoPlay_; }
	void SetParentTransform(WorldTransform* parent);
	void Play();
	void Stop();

private:
	void EnsureRuntime();
	void ApplyParent();
	ParticleSceneAssetType assetType_ = ParticleSceneAssetType::Cpu;
	std::string asset_;
	WorldTransform transform_;
	BaseParticles* cpu_ = nullptr;
	ParticleEffectHandle effect_{};
	bool autoPlay_ = true;
	bool played_ = false;
	WorldTransform* parentTransform_ = nullptr;
};
}
