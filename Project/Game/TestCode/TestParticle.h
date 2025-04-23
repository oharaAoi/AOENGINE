#pragma once
#include <memory>
#include <vector>
#include "Engine/Components/GameObject/GeometryObject.h"
#include "Engine/Components/Attribute/AttributeGui.h"
#include "Engine/Lib/ParticlesData.h"

class TestParticle :
	public AttributeGui {

public:

	TestParticle() = default;
	~TestParticle() override {};

	void Init(float distance);

	void Update(const Quaternion& bill);

	void Emit(const Vector3& pos);

#ifdef _DEBUG
	void Debug_Gui() override;
#endif

private:

	std::string name_ = "TestParticle";

	std::unique_ptr<GeometryObject> shape_;

	std::vector<ParticleSingle> particleArray_;

	ParticleEmit emitter_;

};

