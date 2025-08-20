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

	void ChangeMesh();

public:		// json関連

	json GetJsonData() const { return emitter_.ToJson(particleName_); }

	void SetJsonData(const json& _jsonData) { 
		emitter_.FromJson(_jsonData);
	};
	
public:

	void SetIsStop(bool _stop) { isStop_ = _stop; }

	void SetPos(const Vector3& pos) { emitter_.translate = pos; }

	void SetParent(const Matrix4x4& parentMat);

	std::shared_ptr<Mesh> GetMesh() const { return shape_; }

	void SetParticlesList(const std::shared_ptr<std::list<ParticleSingle>>& list) { particleArray_ = list; }

	const std::string& GetUseTexture() const { return emitter_.useTexture; }

	std::shared_ptr<Material> GetShareMaterial() { return shareMaterial_; }
	void SetShareMaterial(std::shared_ptr<Material> _material) { shareMaterial_ = _material; }

	bool GetIsAddBlend() const {return emitter_.isParticleAddBlend;}

	void SetLoop(bool _loop) { emitter_.isLoop = _loop; }

	bool GetChangeMesh() const { return changeMesh_; }

protected:

	// 最大数
	const uint32_t kMaxParticles = 200;
	// groupの名前
	const std::string kGroupName = "CPU";
	// particleName
	std::string particleName_ = "new particles";

	bool isAddBlend_;

	// meshの形状
	std::shared_ptr<Mesh> shape_;
	std::shared_ptr<Material> shareMaterial_;

	// Particleの情報
	std::shared_ptr<std::list<ParticleSingle>> particleArray_;

	// emitter
	ParticleEmit emitter_;
	float emitAccumulator_;
	float currentTimer_;
	bool isStop_;

	bool changeMesh_ = false;

	std::string meshName_;

	// 親のMatrix
	const Matrix4x4* parentWorldMat_ = nullptr;
};

