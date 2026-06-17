#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <d3d12.h>
#include <wrl.h>

#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/WorldTransform.h"

namespace AOENGINE {

class BaseMaterial;
class Mesh;

/// <summary>
/// 同一Meshを持つ通常3DモデルをInstancing描画するためのRenderer。
/// SceneRendererで集めたbatchを受け取り、instanceごとのTransform/MaterialをStructuredBufferとしてGPUへ送ります。
/// </summary>
class ModelInstancingRenderer {
public:
	/// <summary>
	/// 1instance分の参照元データ。
	/// 実体は各GameObjectが所有し、このRendererは描画時にGPU bufferへコピーします。
	/// </summary>
	struct InstanceSource {
		const AOENGINE::WorldTransform* transform = nullptr;
		const AOENGINE::Material::MaterialData* material = nullptr;
		uint32_t albedoTextureIndex = 0;
	};

	/// <summary>
	/// 同じMeshでまとめられる通常描画用batch。
	/// Textureや色などのMaterial差分はInstanceSource側で保持します。
	/// </summary>
	struct NormalBatch {
		AOENGINE::Mesh* mesh = nullptr;
		std::vector<InstanceSource> instances;
	};

	ModelInstancingRenderer() = default;
	~ModelInstancingRenderer() = default;
	ModelInstancingRenderer(const ModelInstancingRenderer&) = delete;
	const ModelInstancingRenderer& operator=(const ModelInstancingRenderer&) = delete;

	/// <summary>
	/// 確保したSRVやUpload bufferを解放します。
	/// </summary>
	void Finalize();

	/// <summary>
	/// 1フレーム内で使う一時bufferの使用数をリセットします。
	/// </summary>
	void BeginFrame();

	/// <summary>
	/// SceneRendererで収集した通常モデルのInstancing batchを描画します。
	/// </summary>
	void DrawNormalBatches(const std::vector<NormalBatch>& batches);

private:
	/// <summary>
	/// Instancing VSへ送る1instance分のTransform情報。
	/// </summary>
	struct TransformInstanceData {
		Math::Matrix4x4 matWorld;
		Math::Matrix4x4 matWorldPrev;
		Math::Matrix4x4 worldInverseTranspose;
	};

	/// <summary>
	/// Instancing PSへ送る1instance分のMaterial情報。
	/// HLSL側のInstanceMaterialと同じ並び・サイズに合わせます。
	/// </summary>
	struct NormalInstanceMaterialData {
		Math::Vector4 color;
		int32_t enableLighting = 0;
		uint32_t albedoTextureIndex = 0;
		float padding0[2] = {};
		Math::Matrix4x4 uvTransform;
		float shininess = 1.0f;
		float discardValue = 0.01f;
		float iblScale = 0.5f;
		float padding1 = 0.0f;
	};
	static_assert(sizeof(NormalInstanceMaterialData) == 112, "NormalInstanceMaterialData must match Object3DInstancing.PS.hlsl InstanceMaterial.");

	/// <summary>
	/// Transform instance bufferとSRVの組。
	/// capacityを超えない限りフレームを跨いで再利用します。
	/// </summary>
	struct TransformBuffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		DescriptorHandles srv{};
		TransformInstanceData* mapped = nullptr;
		uint32_t capacity = 0;
		bool hasSrv = false;
	};

	/// <summary>
	/// Material instance bufferとSRVの組。
	/// TextureはalbedoTextureIndexでSRV heap上の位置を参照します。
	/// </summary>
	struct MaterialBuffer {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		DescriptorHandles srv{};
		NormalInstanceMaterialData* mapped = nullptr;
		uint32_t capacity = 0;
		bool hasSrv = false;
	};

	TransformBuffer& AcquireTransformBuffer(uint32_t instanceCount);
	MaterialBuffer& AcquireMaterialBuffer(uint32_t instanceCount);

	/// <summary>
	/// 必要数を満たすTransform bufferを確保または拡張します。
	/// </summary>
	void EnsureTransformBuffer(TransformBuffer& buffer, uint32_t instanceCount);

	/// <summary>
	/// 必要数を満たすMaterial bufferを確保または拡張します。
	/// </summary>
	void EnsureMaterialBuffer(MaterialBuffer& buffer, uint32_t instanceCount);

private:
	std::vector<TransformBuffer> transformBuffers_;
	std::vector<MaterialBuffer> materialBuffers_;
	std::vector<DescriptorHandles> allocatedSrvHandles_;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> retiredResources_;

	uint32_t usedTransformBuffers_ = 0;
	uint32_t usedMaterialBuffers_ = 0;
};

}
