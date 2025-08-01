#pragma once
#include <string>
#include <memory>
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Resource/ShaderResource.h"
#include "Engine/Lib/GpuParticlesData.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

class GpuParticleEmitter :
	public AttributeGui {
public:

	struct PerFrame {
		float time;
		float deltaTime;
	};
	
public:

	GpuParticleEmitter() = default;
	~GpuParticleEmitter();

	void Debug_Gui() override;

	void Init(const std::string& name);

	void Update();

	void EmitCommand(ID3D12GraphicsCommandList* commandList);

	void DrawShape() const;

public:

	void SetParticleResourceHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& _handle) { particleResourceHandle_ = _handle; }
	void SetFreeListIndexHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& _handle) { freeListIndexHandle_ = _handle; }
	void SetFreeListHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& _handle) { freeListHandle_ = _handle; }
	void SetMaxParticleResource(ID3D12Resource* _resource) { maxParticleResource_ = _resource; }

public:

	void SetIsStop(bool _isStop) { isStop_ = _isStop; }

	void SetParent(const Matrix4x4& parentMat);

	void SetLocalPos(const Vector3& _pos) { emitterItem_.pos = _pos; }

protected:

	void SetItem();

protected :

	ID3D12Device* dxDevice_;
	ID3D12GraphicsCommandList* commandList_;

	D3D12_GPU_DESCRIPTOR_HANDLE particleResourceHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE freeListIndexHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE freeListHandle_;

	ComPtr<ID3D12Resource> emitterResource_;
	GpuParticleEmitterData* emitterData_;

	ComPtr<ID3D12Resource> perFrameBuffer_;
	PerFrame* perFrame_;

	ID3D12Resource* maxParticleResource_;

	GpuParticleEmitterItem emitterItem_;
	
	// parameter
	bool isStop_ = false;
	float emitAccumulator_;
	float currentTimer_;

	int emitCount_;

	std::string emitCsName_;

	// 親のMatrix
	const Matrix4x4* parentWorldMat_ = nullptr;

};

