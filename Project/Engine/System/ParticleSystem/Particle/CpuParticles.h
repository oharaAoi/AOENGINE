#pragma once
#include <memory>
#include <list>
#include "Engine/Module/Components/GameObject/GeometryObject.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Lib/ParticlesData.h"

class CpuParticles :
	public AttributeGui {
public:

	CpuParticles() = default;
	virtual ~CpuParticles() override {};

	void Init(const std::string& name, bool isAddBlend = true);

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

	bool isAddBlend_;

	const std::string kGroupName = "Effect";
	std::string name_ = "new particles";

	std::unique_ptr<GeometryObject> shape_;

	std::list<ParticleSingle> particleArray_;

	ParticleEmit emitter_;
	float emitAccumulator_;
	float currentTimer_;
	bool isStop_;

	const Matrix4x4* parentWorldMat_ = nullptr;
};