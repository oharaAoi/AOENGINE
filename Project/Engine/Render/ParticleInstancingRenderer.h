#pragma once
#include <vector>
#include <memory>
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Components/Meshes/Mesh.h"
#include "Engine/Components/Materials/Material.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"

/// <summary>
/// Particleをインスタンシング描画するためのクラス
/// </summary>
class ParticleInstancingRenderer {
public:		// 構造体

	struct ParticleData {
		Vector4 color;
		Matrix4x4 worldMat;
	};

	struct Information {
		Mesh* pMesh;
		Material* materials;
		ComPtr<ID3D12Resource> particleBuffer;
		ParticleData* particleData;
		DescriptorHeap::DescriptorHandles instancingSrvHandle;
	};

public:

	ParticleInstancingRenderer() = default;
	~ParticleInstancingRenderer();

	void Init(uint32_t instanceNum);

public:

	void AddParticle(Mesh* _pMesh, Material* _pMaterial);

private:

	uint32_t maxInstanceNum_;

	std::vector<Information> particles_;

};

