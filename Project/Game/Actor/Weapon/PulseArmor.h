#pragma once
// Engine
#include "Engine/Module/Geometry/Polygon/SphereGeometry.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

class PulseArmor :
	public AttributeGui {
public:

	struct DissolveSetting {
		Matrix4x4 uvTransform;
		Vector4 color;
		Vector4 edgeColor;
		float threshold = 0.5f;
	};

public:

	PulseArmor() = default;
	~PulseArmor();

	void Init();

	void Update();

	void Draw() const;

#ifdef _DEBUG
	void Debug_Gui();
#endif

private:

	SphereGeometry geometry_;

	std::shared_ptr<Mesh> mesh_;
	std::unique_ptr<Material> material_ = nullptr;
	std::unique_ptr<WorldTransform> worldTransform_ = nullptr;

	std::unique_ptr<DxResource> settingBuffer_;
	DissolveSetting* setting_;

	std::string noiseTexture_;

	SRT uvSrt_;

};

