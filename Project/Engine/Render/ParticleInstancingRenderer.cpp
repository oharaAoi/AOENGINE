#include "ParticleInstancingRenderer.h"
#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/Lib/Math/Frustum.h"

#include <algorithm>
#include <cmath>
#include <limits>


using namespace AOENGINE;

namespace {

Math::Sphere CalculateLocalBoundingSphere(const AOENGINE::Mesh& mesh) {
	const std::vector<VertexData>& vertices = mesh.GetVerticesData();
	if (vertices.empty()) {
		return Math::Sphere{ .center = CVector3::ZERO, .radius = 0.0f };
	}

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

	for (const VertexData& vertex : vertices) {
		const Math::Vector3 position{ vertex.pos.x, vertex.pos.y, vertex.pos.z };
		min = Math::Vector3::Min(min, position);
		max = Math::Vector3::Max(max, position);
	}

	const Math::Vector3 center = (min + max) * 0.5f;
	float radius = 0.0f;
	for (const VertexData& vertex : vertices) {
		const Math::Vector3 position{ vertex.pos.x, vertex.pos.y, vertex.pos.z };
		radius = (std::max)(radius, (position - center).Length());
	}

	return Math::Sphere{ .center = center, .radius = radius };
}

void UpdateWorldBoundingSphere(AOENGINE::ParticleInstancingRenderer::Information& information) {
	information.hasWorldBounds = false;
	information.contains2dParticle = false;

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

	for (uint32_t index = 0; index < information.instanceCount; ++index) {
		const auto& particle = information.particleData[index];
		if (particle.draw2d != 0) {
			information.contains2dParticle = true;
			continue;
		}

		const Math::Vector3 scale = particle.worldMat.GetScale();
		const float maxScale = (std::max)({ std::fabs(scale.x), std::fabs(scale.y), std::fabs(scale.z) });
		const Math::Vector3 center = TransformCoord(information.localBoundingSphere.center, particle.worldMat);
		float radius = information.localBoundingSphere.radius * maxScale;

		// Stretch描画ではVSが速度方向へ頂点を延ばすため、その長さも境界へ含めます。
		if (particle.isStretch != 0) {
			radius += 0.5f * (1.0f + particle.velocity.Length());
		}

		const Math::Vector3 extent{ radius, radius, radius };
		min = Math::Vector3::Min(min, center - extent);
		max = Math::Vector3::Max(max, center + extent);
		information.hasWorldBounds = true;
	}

	if (!information.hasWorldBounds) {
		return;
	}

	const Math::Vector3 center = (min + max) * 0.5f;
	information.worldBoundingSphere = Math::Sphere{
		.center = center,
		.radius = (max - center).Length()
	};
}

}

ParticleInstancingRenderer::~ParticleInstancingRenderer() {
	for (auto& particle : particleMap_) {
		particle.second.particleResource.Reset();
		AOENGINE::DescriptorHeap::AddFreeSrvList(particle.second.srvHandle.assignIndex_);
	}
	particleMap_.clear();
	perViewBuffer_.Reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleInstancingRenderer::Init(uint32_t instanceNum) {
	maxInstanceNum_ = instanceNum;

	perViewBuffer_ = CreateBufferResource(AOENGINE::GraphicsContext::GetInstance()->GetDevice(), sizeof(PerView));
	perViewBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&perView_));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleInstancingRenderer::Update(const std::string& id, const std::vector<ParticleData>& particleData, bool anyParticleAlive, uint32_t blendType) {
	auto informationIt = particleMap_.find(id);
	if (informationIt == particleMap_.end()) {
		assert(false && "Particleの描画情報が登録されていません");
		return;
	}

	Information& information = informationIt->second;
	information.blendModeType = blendType;
	information.instanceCount = (std::min)(static_cast<uint32_t>(particleData.size()), maxInstanceNum_);
	information.anyParticleAlive = anyParticleAlive && information.instanceCount > 0;

	for (uint32_t index = 0; index < information.instanceCount; ++index) {
		information.particleData[index] = particleData[index];
		information.particleData[index].cameraPos = AOENGINE::Render::GetEyePos();
	}

	UpdateWorldBoundingSphere(information);
}

void ParticleInstancingRenderer::PostUpdate() {
	// instanceCountはDrawまで保持する必要があるため、ここではリセットしません。
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void ParticleInstancingRenderer::Draw(ID3D12GraphicsCommandList* commandList, const Math::Frustum* frustum) const {
	for (const auto& [id, information] : particleMap_) {
		if (!information.anyParticleAlive || information.instanceCount == 0) { continue; }
		if (frustum != nullptr && information.hasWorldBounds && !information.contains2dParticle &&
			!frustum->Intersects(information.worldBoundingSphere)) {
			continue;
		}

		if (information.blendModeType == AOENGINE::Blend::BlendMode::None) {
			Engine::SetPipeline(PSOType::Object3d, "Object_ParticleNone.json");
		} else if(information.blendModeType == AOENGINE::Blend::BlendMode::Normal) {
			Engine::SetPipeline(PSOType::Object3d, "Object_ParticleNormal.json");
		} else if (information.blendModeType == AOENGINE::Blend::BlendMode::Add) {
			Engine::SetPipeline(PSOType::Object3d, "Object_ParticleAdd.json");
		} else if (information.blendModeType == AOENGINE::Blend::BlendMode::Subtract) {
			Engine::SetPipeline(PSOType::Object3d, "Object_ParticleSubtract.json");
		} else if (information.blendModeType == AOENGINE::Blend::BlendMode::Multiply) {
			Engine::SetPipeline(PSOType::Object3d, "Object_ParticleMultiply.json");
		} else if (information.blendModeType == AOENGINE::Blend::BlendMode::Screen) {
			Engine::SetPipeline(PSOType::Object3d, "Object_ParticleScreen.json");
		}

		Pipeline* pso = Engine::GetLastUsedPipeline();

		commandList->IASetVertexBuffers(0, 1, &information.pMesh->GetVBV());
		commandList->IASetIndexBuffer(&information.pMesh->GetIBV());

		UINT index = pso->GetRootSignatureIndex("gMaterial");
		commandList->SetGraphicsRootConstantBufferView(index, information.materials->GetBufferAddress());
		index = pso->GetRootSignatureIndex("gParticles");
		commandList->SetGraphicsRootDescriptorTable(index, information.srvHandle.handleGPU);

		index = pso->GetRootSignatureIndex("gTexture");
		std::string textureName = information.materials->GetAlbedoTexture();
		AOENGINE::TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, textureName, index);
		index = pso->GetRootSignatureIndex("gPerView");
		commandList->SetGraphicsRootConstantBufferView(index, perViewBuffer_->GetGPUVirtualAddress());

		commandList->DrawIndexedInstanced(information.pMesh->GetIndexNum(), information.instanceCount, 0, 0, 0);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ particleを追加
///////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<Material> ParticleInstancingRenderer::AddParticle(const std::string& id, const std::string& textureName, std::shared_ptr<Mesh> _pMesh, uint32_t blendType) {
	auto it = particleMap_.find(id);
	if (it != particleMap_.end()) {
		return particleMap_[id].materials;		// 見つかったら早期リターン
	}
	AOENGINE::GraphicsContext* graphicsCtx = AOENGINE::GraphicsContext::GetInstance();

	ID3D12Device* device = graphicsCtx->GetDevice();
	AOENGINE::DescriptorHeap* dxHeap = graphicsCtx->GetDxHeap();

	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_HEAP_PROPERTIES defaultHeapProperties{};
	defaultHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// -----------------------------------------------------------------
	Information particles;
	particles.pMesh = _pMesh;
	particles.localBoundingSphere = CalculateLocalBoundingSphere(*_pMesh);
	particles.materials = std::make_shared<Material>();
	particles.materials->Init();
	particles.textureName = textureName;

	particles.particleResource = CreateBufferResource(device, sizeof(ParticleData) * maxInstanceNum_);
	particles.particleData = nullptr;
	particles.particleResource->Map(0, nullptr, reinterpret_cast<void**>(&particles.particleData));
	particles.srvHandle = dxHeap->AllocateSRV();

	D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	desc.Buffer.NumElements = maxInstanceNum_;
	desc.Buffer.StructureByteStride = sizeof(ParticleData);
	device->CreateShaderResourceView(particles.particleResource.Get(), &desc, particles.srvHandle.handleCPU);

	for (uint32_t index = 0; index < maxInstanceNum_; ++index) {
		particles.particleData[index].worldMat = Math::Matrix4x4::MakeUnit();
		particles.particleData[index].color = { 0,0,0,0 };
	}

	particles.blendModeType = blendType;

	particleMap_.emplace(id, std::move(particles));

	return particleMap_[id].materials;
}

void ParticleInstancingRenderer::ChangeMesh(const std::string& id, std::shared_ptr<Mesh> _mesh) {
	auto it = particleMap_.find(id);
	if (it == particleMap_.end()) {
		assert(false && "対応する名前が見つかりませんでした");
	}

	particleMap_[id].pMesh = _mesh;
	particleMap_[id].localBoundingSphere = CalculateLocalBoundingSphere(*_mesh);
}
