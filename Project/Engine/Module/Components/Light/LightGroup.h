#pragma once
#include <memory>
// light
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Engine/DirectX/Pipeline/Pipeline.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"


class LightGroup : 
	public AttributeGui {
public:

	enum LightKind {
		Directional,
		PBR,
	};

public:

	LightGroup();
	~LightGroup();

	void Init(ID3D12Device* device);

	void Finalize();

	void Update();

	void Draw(const Pipeline* pso, ID3D12GraphicsCommandList* commandList);
	
	void Debug_Gui() override;

	DirectionalLight* GetDirectionalLight() { return directionalLight_.get(); }
	PointLight* GetPointLight() { return pointLight_.get(); }
	SpotLight* GetSpotLight() { return spotLight_.get(); }

public:

	void SetEyePos(const Vector3& pos) { eyePos_ = pos; }

private:

	std::unique_ptr<DirectionalLight> directionalLight_ = nullptr;
	std::unique_ptr<PointLight> pointLight_ = nullptr;
	std::unique_ptr<SpotLight> spotLight_ = nullptr;
	
	Vector3 eyePos_;
};