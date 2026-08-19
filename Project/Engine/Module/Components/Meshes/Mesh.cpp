#include "Mesh.h"

#include <cassert>
#include <cmath>
#include <cstring>
#include <limits>
#include <utility>

using namespace AOENGINE;

Mesh::Mesh() = default;

Mesh::~Mesh() {
	Finalize();
}

void Mesh::Init(ID3D12Device* device, const CreateInfo& createInfo) {
	assert(device != nullptr);

	Finalize();
	vertices_ = createInfo.vertices;
	initialVertices_ = vertices_;
	outputVertices_ = vertices_;
	indices_ = createInfo.indices;
	subMeshes_ = createInfo.subMeshes;
	name_ = createInfo.name;

	if (subMeshes_.empty() && !indices_.empty()) {
		subMeshes_.push_back(SubMesh{
			.firstIndex = 0,
			.indexCount = static_cast<uint32_t>(indices_.size()),
			.baseVertex = 0,
			.materialSlot = 0,
			.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			.name = "Default"
		});
	}

	for (const SubMesh& subMesh : subMeshes_) {
		assert(subMesh.firstIndex <= indices_.size());
		assert(subMesh.indexCount <= indices_.size() - subMesh.firstIndex);
	}

	if (!vertices_.empty()) {
		vertexBuffer_ = CreateBufferResource(device, vertices_.size() * sizeof(VertexData));
		vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
		vertexBufferView_.SizeInBytes = static_cast<UINT>(vertices_.size() * sizeof(VertexData));
		vertexBufferView_.StrideInBytes = sizeof(VertexData);
		vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertices_));
		std::memcpy(mappedVertices_, vertices_.data(), vertices_.size() * sizeof(VertexData));
	}

	if (!indices_.empty()) {
		indexBuffer_ = CreateBufferResource(device, indices_.size() * sizeof(uint32_t));
		indexBufferView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
		indexBufferView_.SizeInBytes = static_cast<UINT>(indices_.size() * sizeof(uint32_t));
		indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
		indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndices_));
		std::memcpy(mappedIndices_, indices_.data(), indices_.size() * sizeof(uint32_t));
	}

	if (!vertices_.empty()) {
		Math::Vector3 min{
			(std::numeric_limits<float>::max)(),
			(std::numeric_limits<float>::max)(),
			(std::numeric_limits<float>::max)()
		};
		Math::Vector3 max{
			std::numeric_limits<float>::lowest(),
			std::numeric_limits<float>::lowest(),
			std::numeric_limits<float>::lowest()
		};
		for (const VertexData& vertex : vertices_) {
			const Math::Vector3 position{ vertex.pos.x, vertex.pos.y, vertex.pos.z };
			min = Math::Vector3::Min(min, position);
			max = Math::Vector3::Max(max, position);
		}
		const Math::Vector3 center = (min + max) * 0.5f;
		float radius = 0.0f;
		for (const VertexData& vertex : vertices_) {
			const Math::Vector3 position{ vertex.pos.x, vertex.pos.y, vertex.pos.z };
			radius = (std::max)(radius, (position - center).Length());
		}
		localBounds_ = Math::Sphere{ .center = center, .radius = radius };
	}
}

void Mesh::Init(ID3D12Device* device, std::vector<VertexData> vertices, std::vector<uint32_t> indices) {
	CreateInfo createInfo;
	createInfo.vertices = std::move(vertices);
	createInfo.indices = std::move(indices);
	Init(device, createInfo);
}

void Mesh::Finalize() {
	mappedVertices_ = nullptr;
	mappedIndices_ = nullptr;
	vertexBuffer_.Reset();
	indexBuffer_.Reset();
	vertexBufferView_ = {};
	indexBufferView_ = {};
}

void Mesh::Bind(ID3D12GraphicsCommandList* commandList) const {
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBufferView_);
}

void Mesh::IndexBindCommand(ID3D12GraphicsCommandList* commandList) const {
	commandList->IASetIndexBuffer(&indexBufferView_);
}

const SubMesh& Mesh::GetSubMesh(uint32_t index) const {
	return subMeshes_.at(index);
}

uint32_t Mesh::GetSubMeshCount() const {
	return static_cast<uint32_t>(subMeshes_.size());
}

const std::vector<VertexData>& Mesh::GetVertices() const {
	return vertices_;
}

const std::vector<uint32_t>& Mesh::GetIndices() const {
	return indices_;
}

const D3D12_VERTEX_BUFFER_VIEW& Mesh::GetVBV() const {
	return vertexBufferView_;
}

const D3D12_INDEX_BUFFER_VIEW& Mesh::GetIBV() const {
	return indexBufferView_;
}

const Math::Sphere& Mesh::GetLocalBounds() const {
	return localBounds_;
}

void Mesh::SetInitVertex() {
	if (!mappedVertices_ || initialVertices_.empty()) {
		return;
	}
	std::memcpy(mappedVertices_, initialVertices_.data(), initialVertices_.size() * sizeof(VertexData));
}

void Mesh::SetOutputVertexData(VertexData* vertexData) {
	if (!vertexData || outputVertices_.empty()) {
		return;
	}
	std::memcpy(outputVertices_.data(), vertexData, outputVertices_.size() * sizeof(VertexData));
}

void Mesh::SetVertexData(const std::vector<VertexData>& vertexData) {
	if (vertexData.size() != vertices_.size()) {
		return;
	}
	vertices_ = vertexData;
	if (mappedVertices_) {
		std::memcpy(mappedVertices_, vertices_.data(), vertices_.size() * sizeof(VertexData));
	}
}

void Mesh::SetIndices(const std::vector<uint32_t>& indices) {
	if (indices.size() != indices_.size()) {
		return;
	}
	indices_ = indices;
	if (mappedIndices_) {
		std::memcpy(mappedIndices_, indices_.data(), indices_.size() * sizeof(uint32_t));
	}
}
