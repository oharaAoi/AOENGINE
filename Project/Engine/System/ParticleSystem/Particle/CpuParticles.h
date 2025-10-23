#pragma once
#include <memory>
#include <list>
#include "Engine/Module/Components/GameObject/GeometryObject.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Lib/ParticlesData.h"

/// <summary>
/// CpuParticleのクラス
/// </summary>
class CpuParticles :
	public AttributeGui {
public:

	CpuParticles() = default;
	virtual ~CpuParticles() override {};

public:

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="name"></param>
	/// <param name="isAddBlend"></param>
	void Init(const std::string& name, bool isAddBlend = true);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="bill"></param>
	void Update(const Quaternion& bill);

	/// <summary>
	/// 射出処理
	/// </summary>
	/// <param name="pos"></param>
	void Emit(const Vector3& pos);

	/// <summary>
	/// 射出更新処理
	/// </summary>
	void EmitUpdate();

#ifdef _DEBUG
	/// <summary>
	/// 編集処理
	/// </summary>
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