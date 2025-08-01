#pragma once
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"


class GpuParticleField :
	public AttributeGui {
public:

	struct AccelerationField {
		Vector3 acceleration;
		float pad1;
		Vector3 min;
		float pad2;
		Vector3 max;
		float pad3;
	};

	struct PerFrame {
		float time;
		float deltaTime;
	};

public:

	GpuParticleField() = default;
	~GpuParticleField();

	void Debug_Gui() override;

	void Init(uint32_t _instanceNum);

	void Update();

	void DrawShape() const;

	void Execute(ID3D12GraphicsCommandList* commandList);

public:

	void SetParticleResourceHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& _handle) { particleResourceHandle_ = _handle; }
	void SetMaxParticleResource(ID3D12Resource* _resource) { maxParticleResource_ = _resource; }

private:

	bool isEnable_;

	float timer_;

	// resource
	D3D12_GPU_DESCRIPTOR_HANDLE particleResourceHandle_;

	ComPtr<ID3D12Resource> fieldBuffer_;
	AccelerationField* fieldData_;

	ComPtr<ID3D12Resource> perFrameBuffer_;
	PerFrame* perFrame_;

	ID3D12Resource* maxParticleResource_;

	uint32_t kInstanceNum_;
};

