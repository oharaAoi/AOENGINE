#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Module/Components/IComponent.h"

namespace AOENGINE {

class BaseGameObject;
class Material;
class Mesh;
class WorldTransform;

enum class WorldTextBillboardMode {
	None,
	FaceCamera,
	YAxisOnly
};

struct WorldTextSettings {
	bool enabled = true;
	std::string text = "New Text";
	std::string fontPath;
	float fontSize = 64.0f;
	Color color = Colors::Linear::white;
	Math::Vector3 localPosition{ 0.0f, 1.0f, 0.0f };
	Math::Quaternion localRotation{};
	float height = 0.5f;
	WorldTextBillboardMode billboardMode = WorldTextBillboardMode::FaceCamera;
	bool depthTest = true;
};

class WorldTextComponent final : public IComponent {
public:
	WorldTextComponent();
	~WorldTextComponent() override;
	WorldTextComponent(const WorldTextComponent&) = delete;
	WorldTextComponent& operator=(const WorldTextComponent&) = delete;

	void Init(BaseGameObject& owner);
	void Update(const BaseGameObject& owner);
	void Draw() const;
	void Finalize();

	void SetText(const std::string& text);
	void SetFontPath(const std::string& path);
	void SetFontSize(float size);
	void SetColor(const Color& color) { settings_.color = color; }
	void SetLocalPosition(const Math::Vector3& position) { settings_.localPosition = position; }
	void SetLocalRotation(const Math::Quaternion& rotation) { settings_.localRotation = rotation; }
	void SetHeight(float height);
	void SetBillboardMode(WorldTextBillboardMode mode) { settings_.billboardMode = mode; }
	void SetDepthTest(bool enabled) { settings_.depthTest = enabled; }
	void SetEnabled(bool enabled) { settings_.enabled = enabled; }
	void SetSettings(const WorldTextSettings& settings);

	const WorldTextSettings& GetSettings() const { return settings_; }
	const std::string& GetText() const { return settings_.text; }
	bool IsEnabled() const { return settings_.enabled; }

private:
	void RebuildTexture();
	void UpdateTransform(const BaseGameObject& owner);

	WorldTextSettings settings_;
	std::shared_ptr<Mesh> planeMesh_;
	std::unique_ptr<Material> material_;
	std::unique_ptr<WorldTransform> transform_;
	std::string generatedTextureName_;
	Math::Vector2 generatedTextureSize_{ 1.0f, 1.0f };
	bool textureDirty_ = true;
	uint64_t runtimeId_ = 0;
	static uint64_t nextRuntimeId_;
};

}
