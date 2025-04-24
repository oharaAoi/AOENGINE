#pragma once
#include <memory>
#include <list>
#include "Engine/Components/GameObject/GeometryObject.h"
#include "Engine/Components/Attribute/AttributeGui.h"
#include "Engine/Lib/ParticlesData.h"

class BaseParticles :
	public AttributeGui {
public:

	BaseParticles() = default;
	virtual ~BaseParticles() override {};

	void Init(const std::string& name);

	void Update(const Quaternion& bill);

	void Emit(const Vector3& pos);

	void EmitUpdate();

#ifdef _DEBUG
	void Debug_Gui() override;
#endif

public:

	virtual void SetParent(const Matrix4x4& parentMat);

protected:

	const uint32_t kMaxParticles = 200;

	const std::string kGroupName = "Effect";
	std::string name_ = "new particles";

	std::unique_ptr<GeometryObject> shape_;

	std::list<ParticleSingle> particleArray_;

	ParticleEmit emitter_;

	const Matrix4x4* parentWorldMat_ = nullptr;
};

