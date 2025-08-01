#pragma once
#include <string>
#include <memory>
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Resource/ShaderResource.h"
#include "Engine/Module/Components/GameObject/GeometryObject.h"

class GpuParticleRenderer {
public:	// メンバ構造体

	struct Particle {
		Vector4 color;			// 色
		Vector3 scale;			// 拡縮
		Vector3 targetScale;
		Vector3 rotate;			// 回転
		Vector3 translate;		// 座標
		Vector3 velocity;		// 速度
		Vector3 acceleration;
		// 加速度
		float lifeTime;			// 生存時間
		float currentTime;		// 現在の時間
		float damping;			// 減衰
		float gravity;			// 重力

		int emitType;
		int lifeOfScaleDown;
		int lifeOfScaleUp;
		int lifeOfAlpha;
	};

	struct PerView {
		Matrix4x4 viewProjection;
		Matrix4x4 billboardMat;
	};

	struct PerFrame {
		float time;
		float deltaTime;
	};

	struct MaxParticles {
		int count;
	};

public:

	GpuParticleRenderer() = default;
	~GpuParticleRenderer();

	void Init(uint32_t _instanceNum);

	void Update();

	void Draw() const;

private:

	void CreateResource(DescriptorHeap* _dxHeap, ID3D12Device* _dxDevice);

public:

	D3D12_GPU_DESCRIPTOR_HANDLE GetResourceHandle() const { return particleResource_->GetUAV().handleGPU; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetFreeListIndexHandle() const { return freeListIndexResource_->GetUAV().handleGPU; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetFreeListHandle() const { return freeListResource_->GetUAV().handleGPU; }
	ID3D12Resource* GetMaxBufferResource() const { return maxParticleBuffer_.Get(); }

	void SetView(const Matrix4x4& view, const Matrix4x4& bill) {
		perView_->viewProjection = view;
		perView_->billboardMat = Multiply(Quaternion::AngleAxis(kPI, CVector3::UP).MakeMatrix(), bill);
	}

private:

	uint32_t kInstanceNum_;

	std::unique_ptr<ShaderResource> particleResource_;
	std::unique_ptr<ShaderResource> freeListIndexResource_;
	std::unique_ptr<ShaderResource> freeListResource_;

	ComPtr<ID3D12Resource> perViewBuffer_;
	PerView* perView_;

	ComPtr<ID3D12Resource> perFrameBuffer_;
	PerFrame* perFrame_;

	ComPtr<ID3D12Resource> maxParticleBuffer_;
	MaxParticles* maxBuffer_;

	// meshの形状
	std::unique_ptr<GeometryObject> shape_;
	std::unique_ptr<Material> material_;

};

