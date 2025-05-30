#pragma once
#include "Engine/Module/Geometry/Structs/Vertices.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"

class ParticleMesh final {
public:

	ParticleMesh() = default;
	~ParticleMesh();

	void Init(ID3D12Device* device, std::vector<VertexData> vertexData, std::vector<uint32_t> indices);

	void SetCommand(ID3D12GraphicsCommandList* commandList);

private:

	// vertexData
	ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
	ParticleVertexData* vertexData_ = nullptr;
	uint32_t vertexDataSize_;

	// indexData
	ComPtr<ID3D12Resource> indexBuffer_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};
	uint32_t* indexData_ = nullptr;
	uint32_t indexDataSize_;

};

