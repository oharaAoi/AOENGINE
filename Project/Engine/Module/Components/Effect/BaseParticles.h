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

	void Init(const std::string& name);

	void Update();

	void Emit(const Vector3& pos);

	void EmitUpdate();

	void Reset();

	void Debug_Gui() override;

public:		// json関連

	json GetJsonData() const { return emitter_.ToJson(particleName_); }

	void SetJsonData(const json& _jsonData) { 
		emitter_.FromJson(_jsonData);
		shape_->GetMaterial()->SetUseTexture(emitter_.useTexture);
	};
	
public:

	void SetIsStop(bool _stop) { isStop_ = _stop; }

	void SetPos(const Vector3& pos) { emitter_.translate = pos; }

	void SetParent(const Matrix4x4& parentMat);

	GeometryObject* GetGeometryObject() const { return shape_.get(); }

	void SetParticlesList(const std::shared_ptr<std::list<ParticleSingle>>& list) { particleArray_ = list; }

	const std::string& GetUseTexture() const { return emitter_.useTexture; }

	std::shared_ptr<Material> GetShareMaterial() { return shareMaterial_; }
	void SetShareMaterial(std::shared_ptr<Material> _material) { shareMaterial_ = _material; }

	bool GetIsAddBlend() const {return emitter_.isParticleAddBlend;}

protected:

	// 最大数
	const uint32_t kMaxParticles = 200;

	// groupの名前
	const std::string kGroupName = "Effect";
	// particleName
	std::string particleName_ = "new particles";

	bool isAddBlend_;

	// meshの形状
	std::unique_ptr<GeometryObject> shape_;
	std::shared_ptr<Material> shareMaterial_;

	// Particleの情報
	std::shared_ptr<std::list<ParticleSingle>> particleArray_;

	// emitter
	ParticleEmit emitter_;
	float emitAccumulator_;
	float currentTimer_;
	bool isStop_;

	// 親のMatrix
	const Matrix4x4* parentWorldMat_ = nullptr;
};

