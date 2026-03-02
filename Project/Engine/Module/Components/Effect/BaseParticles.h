#pragma once
#include <memory>
#include <list>
#include "Engine/Module/Components/GameObject/GeometryObject.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Lib/ParticlesData.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Lib/Math/Matrix4x4.h"

namespace AOENGINE {

/// <summary>
/// Particleを射出する
/// </summary>
class BaseParticles :
	public AOENGINE::AttributeGui {
public: // コンストラクタ

	BaseParticles() = default;
	virtual ~BaseParticles() override = default;

public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="name"></param>
	void Init(const std::string& name);

	// 更新処理
	void Update();

	// 描画処理
	void DrawShape();

	// 放出
	void Emit(const Math::Vector3& pos);

	// 放出時間の更新
	void EmitUpdate();

	// リセット
	void Reset();

	// 編集処理
	void Debug_Gui() override;

	// particleのMesh変更
	void ChangeMesh();

public:	// json関連

	json GetJsonData() const { return emitter_.ToJson(particleName_); }

	void SetJsonData(const json& _jsonData) {
		emitter_.FromJson(_jsonData);
	};

public:

	void SetIsStop(bool _stop) { isStop_ = _stop; }
	bool GetIsStop() const { return isStop_; }

	void SetPos(const Math::Vector3& pos) { emitter_.translate = pos; }

	void SetParent(WorldTransform* parentTransform);

	void SetParentMatrix(const Math::Matrix4x4& parentMat);

	std::shared_ptr<AOENGINE::Mesh> GetMesh() const { return shape_; }

	void SetParticlesList(const std::shared_ptr<std::list<AOENGINE::ParticleSingle>>& list) { particleArray_ = list; }

	const std::string& GetUseTexture() const { return emitter_.useTexture; }

	std::shared_ptr<AOENGINE::Material> GetShareMaterial() { return shareMaterial_; }
	void SetShareMaterial(std::shared_ptr<AOENGINE::Material> _material) { shareMaterial_ = _material; }

	bool GetIsAddBlend() const { return emitter_.isParticleAddBlend; }

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
	std::shared_ptr<AOENGINE::Mesh> shape_;
	std::shared_ptr<AOENGINE::Material> shareMaterial_;

	std::string meshName_;

	// Particleの情報
	std::shared_ptr<std::list<AOENGINE::ParticleSingle>> particleArray_;

	// emitter
	AOENGINE::ParticleEmit emitter_;
	float emitAccumulator_;
	float currentTimer_;
	bool isStop_;

	bool changeMesh_ = false;

	Math::Vector3 preWorldPos_;

	// Transform
	std::unique_ptr<WorldTransform> worldTransform_;
	WorldTransform* parentTransform_ = nullptr;

	const Math::Matrix4x4* parentMatrix_ = nullptr;
};

}