#pragma once
#include <cstdint>
#include <vector>
#include <array>
#include <span>
#include <map>
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
#include "Engine/DirectX/Pipeline/Pipeline.h"
#include "Engine/Module/Components/Rigging/SkinCluster.h"
#include "Engine/Module/Components/Rigging/Skeleton.h"

const uint32_t kNumMaxInfluence = 4;

namespace AOENGINE {

class Model;
class Mesh;

/// <summary>
/// skinningの情報を管理しているクラス
/// </summary>
class Skinning {
public:

	/// <summary>
	/// 頂点に対して影響を与えるパラメータ群
	/// </summary>
	struct VertexInfluence {
		std::array<float, kNumMaxInfluence> weights;
		std::array<uint32_t, kNumMaxInfluence> jointIndices;
	};

	/// <summary>
	/// skeleton空間で行列
	/// </summary>
	struct WellForGPU {
		Math::Matrix4x4 skeletonSpaceMatrix;
		Math::Matrix4x4 skeletonSpaceInverseTransposeMat;
	};

	/// <summary>
	/// skeletonの情報
	/// </summary>
	struct SkinningInformation {
		uint32_t numVertices;
	};

public:

	Skinning();
	~Skinning();

public:

	// 終了
	void Finalize();
	// 更新
	void Update(AOENGINE::Skeleton* skeleton);
	// 行列計算用のクラスを生成
	void CreateSkinCluster(ID3D12Device* device, AOENGINE::Skeleton* skeleton, Mesh* mesh, AOENGINE::DescriptorHeap* heap, const std::map<std::string, JointWeightData>& skinClusterData);
	// CSを走らせる
	void RunCs(AOENGINE::Pipeline* _pipeline, ID3D12GraphicsCommandList* commandList) const;
	// CS後の処理
	void EndCS(ID3D12GraphicsCommandList* commandList);

public:

	const D3D12_VERTEX_BUFFER_VIEW& GetVBV() { return vertexBufferView_; }

private:
	UINT vertices_;

	Mesh* mehs_;

	// palette
	ComPtr<ID3D12Resource> paletteResource_;
	std::span<WellForGPU> mappedPalette_;
	DescriptorHandles paletteSrvHandle_;

	// influence
	ComPtr<ID3D12Resource> influenceResource_;
	D3D12_VERTEX_BUFFER_VIEW influenceBuffeView_;
	std::span<VertexInfluence> mappedInfluence_;
	DescriptorHandles influenceSrvHandle_;

	// well
	std::vector<Math::Matrix4x4> inverseBindPoseMatrices_;

	// skinningInformation
	SkinningInformation* skinningInformation_;
	ComPtr<ID3D12Resource> skinningInformationResource_;
	// output
	ComPtr<ID3D12Resource> outputResource_;
	DescriptorHandles outputHandle_;
	// MeshInput
	DescriptorHandles inputHandle_;

	// vertex
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};

	// copy
	ComPtr<ID3D12Resource> copyResource_;

};

}