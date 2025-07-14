#include "TrailRenderer.h"
#include <DirectXTex/d3dx12.h>

void TrailRenderer::Init(ID3D12Device* _device, UINT _maxVerticesPerFrame, UINT _maxIndicesPerFrame, UINT _frameCount) {
	maxVerticesPerFrame_ = _maxVerticesPerFrame;
	maxIndicesPerFrame_ = _maxIndicesPerFrame;
	maxFrameCount_ = _frameCount;

	UINT vbSize = sizeof(TrailVertex) * maxVerticesPerFrame_ * _frameCount;
	UINT ibSize = sizeof(uint16_t) * maxIndicesPerFrame_ * _frameCount;

	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC vbDesc = CD3DX12_RESOURCE_DESC::Buffer(vbSize);
	CD3DX12_RESOURCE_DESC ibDesc = CD3DX12_RESOURCE_DESC::Buffer(ibSize);

	_device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vbDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer_));

	vertexBuffer_->Map(0, nullptr, (void**)&mappedVB_);

	_device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ibDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuffer_));

	indexBuffer_->Map(0, nullptr, (void**)&mappedIB_);
}

void TrailRenderer::Update(const std::vector<TrailPoint>& _trailPoints, float _maxLifetime) {
	// フレーム更新（リング）
	frameIndex_ = (frameIndex_ + 1) % maxFrameCount_;
	// メッシュ生成
	std::vector<TrailVertex> vertices;
	std::vector<uint16_t> indices;
	GenerateTrailMesh(_trailPoints, _maxLifetime, vertices, indices);

	// 現在のリングオフセット
	UINT vbOffset = frameIndex_ * maxVerticesPerFrame_;
	UINT ibOffset = frameIndex_ * maxIndicesPerFrame_;

	memcpy(mappedVB_ + vbOffset, vertices.data(), sizeof(TrailVertex) * vertices.size());
	memcpy(mappedIB_ + ibOffset, indices.data(), sizeof(uint16_t) * indices.size());

	currentVertexCount_ = (UINT)vertices.size();
	currentIndexCount_ = (UINT)indices.size();
}

void TrailRenderer::Draw(ID3D12GraphicsCommandList* _commandList) const {
	UINT vbOffset = frameIndex_ * maxVerticesPerFrame_;
	UINT ibOffset = frameIndex_ * maxIndicesPerFrame_;

	D3D12_VERTEX_BUFFER_VIEW vbView = {
		.BufferLocation = vertexBuffer_->GetGPUVirtualAddress() + vbOffset * sizeof(TrailVertex),
		.SizeInBytes = currentVertexCount_ * sizeof(TrailVertex),
		.StrideInBytes = sizeof(TrailVertex),
	};

	D3D12_INDEX_BUFFER_VIEW ibView = {
		.BufferLocation = indexBuffer_->GetGPUVirtualAddress() + ibOffset * sizeof(uint16_t),
		.SizeInBytes = currentIndexCount_ * sizeof(uint16_t),
		.Format = DXGI_FORMAT_R16_UINT,
	};

	_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_commandList->IASetVertexBuffers(0, 1, &vbView);
	_commandList->IASetIndexBuffer(&ibView);
	_commandList->DrawIndexedInstanced(currentIndexCount_, 1, 0, 0, 0);
}

void TrailRenderer::GenerateTrailMesh(const std::vector<TrailPoint>& points, float maxLifetime, std::vector<TrailVertex>& outVertices, std::vector<uint16_t>& outIndices) {
	if (points.size() < 2) return;

	for (size_t i = 0; i < points.size() - 1; ++i) {
		Vector3 p0 = points[i].position;
		Vector3 p1 = points[i + 1].position;

		Vector3 dir = (p1 - p0).Normalize();
		Vector3 side = (Vector3::Cross(dir, CVector3::UP)).Normalize() * 0.05f;

		float alpha0 = points[i].lifetime / maxLifetime;
		float alpha1 = points[i + 1].lifetime / maxLifetime;

		size_t base = outVertices.size();

		outVertices.push_back({ p0 + side, {0,0}, alpha0 });
		outVertices.push_back({ p0 - side, {1,0}, alpha0 });
		outVertices.push_back({ p1 + side, {0,1}, alpha1 });
		outVertices.push_back({ p1 - side, {1,1}, alpha1 });

		// 三角形2つ
		outIndices.push_back((uint16_t)(base + 0));
		outIndices.push_back((uint16_t)(base + 1));
		outIndices.push_back((uint16_t)(base + 2));

		outIndices.push_back((uint16_t)(base + 2));
		outIndices.push_back((uint16_t)(base + 1));
		outIndices.push_back((uint16_t)(base + 3));
	}
}
