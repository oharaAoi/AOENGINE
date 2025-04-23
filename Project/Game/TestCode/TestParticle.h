#pragma once
#include <memory>
#include <vector>
#include <list>
#include "Engine/Components/GameObject/GeometryObject.h"
#include "Engine/Components/Attribute/AttributeGui.h"
#include "Engine/Lib/ParticlesData.h"

class TestParticle :
	public AttributeGui {

	struct ParticleSRT {
		QuaternionSRT transform;
		Vector4 color;
	};

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

	std::list<ParticleSingle> particleArray_;

	ParticleEmit emitter_;

};

