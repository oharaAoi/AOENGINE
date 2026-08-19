#pragma once
#include <vector>
#include "Engine/Module/Components/Meshes/SubMesh.h"
#include "Engine/Module/Geometry/Structs/Vertices.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Lib/Math/MyMath.h"

namespace AOENGINE {

/// <summary>
/// 3dMesh
/// </summary>
class Mesh final {
public:

	Mesh();
	~Mesh();

public:

	struct CreateInfo {
		std::vector<VertexData> vertices;
		std::vector<uint32_t> indices;
		std::vector<SubMesh> subMeshes;
		std::string name;
	};

public: // public methods

	void Init(ID3D12Device* device, const CreateInfo& createInfo);
	// 既存呼び出し向け。IndexBuffer全体を参照するSubMeshを1つ生成します。
	void Init(ID3D12Device* device, std::vector<VertexData> vertices, std::vector<uint32_t> indices);
	void Finalize();

	// Mesh共通のVB/IBを一度だけ設定
	void Bind(ID3D12GraphicsCommandList* commandList) const;
	void BindCommand(ID3D12GraphicsCommandList* commandList) const { Bind(commandList); }
	void IndexBindCommand(ID3D12GraphicsCommandList* commandList) const;

public: // accessor

	const SubMesh& GetSubMesh(uint32_t index) const;
	uint32_t GetSubMeshCount() const;

	const std::vector<VertexData>& GetVertices() const;
	const std::vector<uint32_t>& GetIndices() const;
	const std::vector<VertexData>& GetVerticesData() const { return GetVertices(); }

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() const;
	const D3D12_INDEX_BUFFER_VIEW& GetIBV() const;

	const Math::Sphere& GetLocalBounds() const;

	// 旧描画パスとの互換API。SubMesh描画への移行後に削除します。
	uint32_t GetIndexNum() const { return static_cast<uint32_t>(indices_.size()); }
	uint32_t GetVertexSize() const { return static_cast<uint32_t>(vertices_.size()); }
	ID3D12Resource* GetVertexBuffer() const { return vertexBuffer_.Get(); }
	ID3D12Resource* GetIndexBuffer() const { return indexBuffer_.Get(); }
	uint32_t* GetIndexData() { return mappedIndices_; }
	VertexData* GetOutputVertexData() { return outputVertices_.data(); }
	void SetInitVertex();
	void SetOutputVertexData(VertexData* vertexData);
	void SetVertexData(const std::vector<VertexData>& vertexData);
	void SetIndices(const std::vector<uint32_t>& indices);
	void SetVBV(const D3D12_VERTEX_BUFFER_VIEW& view) { vertexBufferView_ = view; }
	void SetUseMaterial(const std::string& materialName) { legacyMaterialName_ = materialName; }
	const std::string& GetUseMaterial() const { return legacyMaterialName_; }

private: // private variables

	ComPtr<ID3D12Resource> vertexBuffer_;
	ComPtr<ID3D12Resource> indexBuffer_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	// 再生成、Collider生成、Editor表示などに必要な場合のみ保持
	std::vector<VertexData> vertices_;
	std::vector<VertexData> initialVertices_;
	std::vector<VertexData> outputVertices_;
	std::vector<uint32_t> indices_;
	std::vector<SubMesh> subMeshes_;
	VertexData* mappedVertices_ = nullptr;
	uint32_t* mappedIndices_ = nullptr;

	Math::Sphere localBounds_{};
	std::string name_;
	std::string legacyMaterialName_;
};

}
