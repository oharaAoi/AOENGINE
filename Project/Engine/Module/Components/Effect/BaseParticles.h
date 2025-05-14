#pragma once
#include <memory>
#include <list>
#include "Engine/Module/Components/GameObject/GeometryObject.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Lib/ParticlesData.h"
#include "Engine/Render/ParticleInstancingRenderer.h"

class BaseParticles :
	public AttributeGui {
public:

	BaseParticles() = default;
	virtual ~BaseParticles() override {};

	void Init(const std::string& name, bool isAddBlend = true);

	void Update(const Quaternion& bill);

	void Emit(const Vector3& pos);

	void EmitUpdate();

#ifdef _DEBUG
	void Debug_Gui() override;
#endif

public:

	virtual void SetParent(const Matrix4x4& parentMat);

	bool GetIsAddBlend() const { return isAddBlend_; }

	GeometryObject* GetGeometryObject() const { return shape_.get(); }

	const std::vector<ParticleInstancingRenderer::ParticleData>& GetData() const { return data_; }

protected:

	// 最大数
	const uint32_t kMaxParticles = 200;

	// groupの名前
	const std::string kGroupName = "Effect";
	// particleName
	std::string name_ = "new particles";

	bool isAddBlend_;

	// meshの形状
	std::unique_ptr<GeometryObject> shape_;

	// Particleの情報
	std::list<ParticleSingle> particleArray_;

	std::vector<ParticleInstancingRenderer::ParticleData> data_;

	// emitter
	ParticleEmit emitter_;
	float emitAccumulator_;

	// 親のMatrix
	const Matrix4x4* parentWorldMat_ = nullptr;
};

