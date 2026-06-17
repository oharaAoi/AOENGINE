#include "ModelInstancingRenderer.h"

#include <algorithm>
#include <cstring>

#include "Engine/Core/Engine.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Render/Render.h"
#include "Engine/System/Manager/TextureManager.h"

using namespace AOENGINE;

namespace {

// 現在の通常モデルInstancing用Pipeline。
// 将来的にBlendMode別Instancingへ広げる場合は、batchごとにPipeline名を持たせます。
constexpr const char* kNormalInstancingPipeline = "Object_Normal_Instancing.json";

// bufferを頻繁に作り直さないように、必要数以上の2倍単位で容量を確保します。
uint32_t CalculateCapacity(uint32_t required) {
	uint32_t capacity = 1;
	while (capacity < required) {
		capacity *= 2;
	}
	return capacity;
}

}

void ModelInstancingRenderer::Finalize() {
	AOENGINE::DescriptorHeap* heap = AOENGINE::GraphicsContext::GetInstance()->GetDxHeap();
	if (heap) {
		// Transform/Material buffer用に確保したSRVだけをDescriptorHeapへ返します。
		for (const DescriptorHandles& handle : allocatedSrvHandles_) {
			heap->FreeSRV(static_cast<uint32_t>(handle.assignIndex_));
		}
	}
	allocatedSrvHandles_.clear();
	retiredResources_.clear();

	for (TransformBuffer& buffer : transformBuffers_) {
		if (buffer.resource && buffer.mapped) {
			buffer.resource->Unmap(0, nullptr);
		}
		buffer.resource.Reset();
		buffer.mapped = nullptr;
	}
	transformBuffers_.clear();

	for (MaterialBuffer& buffer : materialBuffers_) {
		if (buffer.resource && buffer.mapped) {
			buffer.resource->Unmap(0, nullptr);
		}
		buffer.resource.Reset();
		buffer.mapped = nullptr;
	}
	materialBuffers_.clear();

	usedTransformBuffers_ = 0;
	usedMaterialBuffers_ = 0;
}

void ModelInstancingRenderer::BeginFrame() {
	usedTransformBuffers_ = 0;
	usedMaterialBuffers_ = 0;
}

void ModelInstancingRenderer::DrawNormalBatches(const std::vector<NormalBatch>& batches) {
	for (const NormalBatch& batch : batches) {
		const uint32_t instanceCount = static_cast<uint32_t>(batch.instances.size());
		if (!batch.mesh || instanceCount == 0) {
			continue;
		}

		// batchごとに必要なinstance数だけGPUへ送る一時bufferを取得します。
		TransformBuffer& transformBuffer = AcquireTransformBuffer(instanceCount);
		MaterialBuffer& materialBuffer = AcquireMaterialBuffer(instanceCount);

		for (uint32_t index = 0; index < instanceCount; ++index) {
			const InstanceSource& source = batch.instances[index];
			if (!source.transform || !source.material) {
				continue;
			}

			// Transformは前フレーム行列も含めて渡し、MotionVectorをInstancing側でも生成できるようにします。
			const AOENGINE::WorldTransformData& transformData = source.transform->GetData();
			transformBuffer.mapped[index] = TransformInstanceData{
				.matWorld = transformData.matWorld,
				.matWorldPrev = transformData.matWorldPrev,
				.worldInverseTranspose = transformData.worldInverseTranspose
			};

			// Material差分はinstance bufferへコピーし、TextureだけはSRV heap indexで参照します。
			const AOENGINE::Material::MaterialData& materialData = *source.material;
			NormalInstanceMaterialData& instanceMaterial = materialBuffer.mapped[index];
			instanceMaterial.color = materialData.color;
			instanceMaterial.enableLighting = materialData.enableLighting;
			instanceMaterial.albedoTextureIndex = source.albedoTextureIndex;
			instanceMaterial.uvTransform = materialData.uvTransform;
			instanceMaterial.shininess = materialData.shininess;
			instanceMaterial.discardValue = materialData.discardValue;
			instanceMaterial.iblScale = materialData.iblScale;
			instanceMaterial.padding0[0] = 0.0f;
			instanceMaterial.padding0[1] = 0.0f;
			instanceMaterial.padding1 = 0.0f;
		}

		ID3D12GraphicsCommandList* commandList = AOENGINE::GraphicsContext::GetInstance()->GetCommandList();
		Pipeline* pipeline = Engine::SetPipeline(PSOType::Object3d, kNormalInstancingPipeline);

		AOENGINE::Render::DrawLightGroup(pipeline);

		commandList->IASetVertexBuffers(0, 1, &batch.mesh->GetVBV());
		commandList->IASetIndexBuffer(&batch.mesh->GetIBV());

		UINT rootIndex = pipeline->GetRootSignatureIndex("gInstanceTransforms");
		commandList->SetGraphicsRootDescriptorTable(rootIndex, transformBuffer.srv.handleGPU);

		rootIndex = pipeline->GetRootSignatureIndex("gViewProjectionMatrix");
		AOENGINE::Render::GetViewProjection()->BindCommandList(commandList, rootIndex);

		rootIndex = pipeline->GetRootSignatureIndex("gViewProjectionMatrixPrev");
		AOENGINE::Render::GetViewProjection()->BindCommandListPrev(commandList, rootIndex);

		rootIndex = pipeline->GetRootSignatureIndex("gInstanceMaterials");
		commandList->SetGraphicsRootDescriptorTable(rootIndex, materialBuffer.srv.handleGPU);

		// TextureはSRV heap全体を渡し、PixelShader側でalbedoTextureIndexから個別に参照します。
		rootIndex = pipeline->GetRootSignatureIndex("gTextures");
		commandList->SetGraphicsRootDescriptorTable(rootIndex, AOENGINE::GraphicsContext::GetInstance()->GetDxHeap()->GetSRVHeapStartGpuHandle());

		rootIndex = pipeline->GetRootSignatureIndex("gShadowMap");
		commandList->SetGraphicsRootDescriptorTable(rootIndex, AOENGINE::Render::GetShadowMap()->GetDeptSrvHandle().handleGPU);

		rootIndex = pipeline->GetRootSignatureIndex("gEnviromentTexture");
		AOENGINE::TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, AOENGINE::Render::GetSkyboxTexture(), rootIndex);

		commandList->DrawIndexedInstanced(batch.mesh->GetIndexNum(), instanceCount, 0, 0, 0);
	}
}

ModelInstancingRenderer::TransformBuffer& ModelInstancingRenderer::AcquireTransformBuffer(uint32_t instanceCount) {
	if (usedTransformBuffers_ >= transformBuffers_.size()) {
		transformBuffers_.emplace_back();
	}

	TransformBuffer& buffer = transformBuffers_[usedTransformBuffers_];
	++usedTransformBuffers_;
	EnsureTransformBuffer(buffer, instanceCount);
	return buffer;
}

ModelInstancingRenderer::MaterialBuffer& ModelInstancingRenderer::AcquireMaterialBuffer(uint32_t instanceCount) {
	if (usedMaterialBuffers_ >= materialBuffers_.size()) {
		materialBuffers_.emplace_back();
	}

	MaterialBuffer& buffer = materialBuffers_[usedMaterialBuffers_];
	++usedMaterialBuffers_;
	EnsureMaterialBuffer(buffer, instanceCount);
	return buffer;
}

void ModelInstancingRenderer::EnsureTransformBuffer(TransformBuffer& buffer, uint32_t instanceCount) {
	if (buffer.resource && buffer.capacity >= instanceCount) {
		return;
	}

	if (buffer.resource && buffer.mapped) {
		// GPUが参照中の可能性があるため、古いResourceは即破棄せず保持します。
		buffer.resource->Unmap(0, nullptr);
		retiredResources_.push_back(buffer.resource);
	}

	buffer.capacity = CalculateCapacity(instanceCount);
	buffer.resource = CreateBufferResource(AOENGINE::GraphicsContext::GetInstance()->GetDevice(), sizeof(TransformInstanceData) * buffer.capacity);
	buffer.resource->Map(0, nullptr, reinterpret_cast<void**>(&buffer.mapped));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = CreateSrvDesc(buffer.capacity, sizeof(TransformInstanceData));
	buffer.srv = AOENGINE::GraphicsContext::GetInstance()->GetDxHeap()->AllocateSRV();
	AOENGINE::GraphicsContext::GetInstance()->GetDevice()->CreateShaderResourceView(buffer.resource.Get(), &srvDesc, buffer.srv.handleCPU);
	buffer.hasSrv = true;
	allocatedSrvHandles_.push_back(buffer.srv);
}

void ModelInstancingRenderer::EnsureMaterialBuffer(MaterialBuffer& buffer, uint32_t instanceCount) {
	if (buffer.resource && buffer.capacity >= instanceCount) {
		return;
	}

	if (buffer.resource && buffer.mapped) {
		// Transform bufferと同じく、拡張時の古いResourceはフレーム中の参照を避けるため保持します。
		buffer.resource->Unmap(0, nullptr);
		retiredResources_.push_back(buffer.resource);
	}

	buffer.capacity = CalculateCapacity(instanceCount);
	buffer.resource = CreateBufferResource(AOENGINE::GraphicsContext::GetInstance()->GetDevice(), sizeof(NormalInstanceMaterialData) * buffer.capacity);
	buffer.resource->Map(0, nullptr, reinterpret_cast<void**>(&buffer.mapped));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = CreateSrvDesc(buffer.capacity, sizeof(NormalInstanceMaterialData));
	buffer.srv = AOENGINE::GraphicsContext::GetInstance()->GetDxHeap()->AllocateSRV();
	AOENGINE::GraphicsContext::GetInstance()->GetDevice()->CreateShaderResourceView(buffer.resource.Get(), &srvDesc, buffer.srv.handleCPU);
	buffer.hasSrv = true;
	allocatedSrvHandles_.push_back(buffer.srv);
}
