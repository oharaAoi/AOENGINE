#include "GpuParticleEmitter.h"
#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/Utilities/DrawUtils.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

using namespace AOENGINE;

GpuParticleEmitter::~GpuParticleEmitter() {
	emitterResource_.Reset();
	perFrameBuffer_.Reset();
}

void GpuParticleEmitter::Debug_Gui() {
	InputTextWithString("Name.", "##cpuParticle", name_);
	ImGui::Separator();

	ImGui::Text("emitAccumulator : %f", emitAccumulator_);
	ImGui::Text("distanceAccumulator : %f", distanceAccumulator_);
	ImGui::Checkbox("isStop", &isStop_);
	emitterItem_.Attribute_Gui();
	emitterItem_.SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化関数
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleEmitter::Init(const std::string& name) {
	AOENGINE::GraphicsContext* ctx = AOENGINE::GraphicsContext::GetInstance();
	dxDevice_ = ctx->GetDevice();
	commandList_ = ctx->GetCommandList();
	
	SetName(name.c_str());
	emitterItem_.SetGroupName("GPU");
	emitterItem_.SetName(name);
	emitterItem_.SetRootField(JsonItems::GetDirectoryPath() + "Effect/");
	emitterItem_.Load();

	emitterResource_ = CreateBufferResource(dxDevice_, sizeof(GpuParticleEmitterData));
	emitterResource_->Map(0, nullptr, reinterpret_cast<void**>(&emitterData_));
	*emitterData_ = GpuParticleEmitterData{};

	// preFrameの作成
	perFrameBuffer_ = CreateBufferResource(dxDevice_, sizeof(PerFrame));
	perFrameBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&perFrame_));
	perFrame_->time = 0.0f;
	perFrame_->deltaTime = 0.0f;

	emitAccumulator_ = 0.0f;
	distanceAccumulator_ = 0.0f;
	currentTimer_ = 0.0f;
	emitCount_ = 0;
	SetItem();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleEmitter::Update() {
	if (isStop_) { return; }
	perFrame_->deltaTime = AOENGINE::GameTimer::DeltaTime();
	perFrame_->time = AOENGINE::GameTimer::TotalTime();
	SetItem();

	Math::Vector3 worldPos = emitterItem_.pos;
	if (parentWorldMat_ != nullptr) {
		worldPos += parentWorldMat_->GetPosition();
	}
	if (!hasPreWorldPos_) {
		preWorldPos_ = worldPos;
		hasPreWorldPos_ = true;
	}

	// -------------------------------------------------
	// ↓ 発射処理
	// -------------------------------------------------
	if (!emitterItem_.isLoop) {
		emitterData_->prePos = worldPos;
		emitterData_->pos = worldPos;
		emitterData_->count = emitterItem_.rateOverTimeCout;
		EmitCommand(commandList_);
		isStop_ = true;
		preWorldPos_ = worldPos;
		return;
	}

	const Math::Vector3 movement = worldPos - preWorldPos_;
	const float distance = Length(movement);
	const float spacing = emitterItem_.emitSpacing > 0.001f ? emitterItem_.emitSpacing : 0.001f;
	emitAccumulator_ += emitterItem_.rateOverTimeCout * perFrame_->deltaTime;
	const uint32_t timeEmitCount = static_cast<uint32_t>(emitAccumulator_);
	emitAccumulator_ -= static_cast<float>(timeEmitCount);

	uint32_t distanceEmitCount = 0;
	float firstDistance = 0.0f;
	const bool isTeleport = emitterItem_.teleportThreshold > 0.0f && distance > emitterItem_.teleportThreshold;
	if (distance > 0.0001f && !isTeleport) {
		firstDistance = spacing - distanceAccumulator_;
		distanceEmitCount = firstDistance <= distance
			? static_cast<uint32_t>(std::floor((distance - firstDistance) / spacing)) + 1
			: 0;
		distanceAccumulator_ = std::fmod(distanceAccumulator_ + distance, spacing);
	}

	const uint32_t frameLimit = (std::max)(emitterItem_.maxEmitPerFrame, 1u);
	emitCount_ = static_cast<int>((std::min)((std::max)(timeEmitCount, distanceEmitCount), frameLimit));
	if (emitCount_ > 0) {
		if (distance > 0.0001f && !isTeleport && distanceEmitCount >= timeEmitCount) {
			const float lastDistance = firstDistance + spacing * static_cast<float>(emitCount_ - 1);
			emitterData_->prePos = Math::Vector3::Lerp(preWorldPos_, worldPos, firstDistance / distance);
			emitterData_->pos = Math::Vector3::Lerp(preWorldPos_, worldPos, lastDistance / distance);
		} else if (distance > 0.0001f && !isTeleport) {
			const float firstT = 1.0f / static_cast<float>(emitCount_);
			emitterData_->prePos = Math::Vector3::Lerp(preWorldPos_, worldPos, firstT);
			emitterData_->pos = worldPos;
		} else {
			emitterData_->prePos = worldPos;
			emitterData_->pos = worldPos;
		}
		emitterData_->count = static_cast<uint32_t>(emitCount_);
		EmitCommand(commandList_);
	}
	if (isTeleport) {
		distanceAccumulator_ = 0.0f;
	}
	preWorldPos_ = worldPos;

	// -------------------------------------------------
	// ↓ 継続時間を進める
	// -------------------------------------------------
	currentTimer_ += AOENGINE::GameTimer::DeltaTime();
	if (currentTimer_ > emitterItem_.duration) {
		if (!emitterItem_.isLoop) {
			isStop_ = true;
		}
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 射出処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleEmitter::EmitCommand(ID3D12GraphicsCommandList* commandList) {
	if (emitterItem_.shape == (int)GpuEmitterShape::Sphere) {
		Engine::SetPipelineCS("GpuParticleEmit.json");
	} else if(emitterItem_.shape == (int)GpuEmitterShape::Box) {
		Engine::SetPipelineCS("GpuParticleBoxEmit.json");
	} else if (emitterItem_.shape == (int)GpuEmitterShape::Cone) {
		Engine::SetPipelineCS("GpuParticleBoxEmit.json");
	}
	Pipeline* pso = Engine::GetLastUsedPipelineCS();
	UINT index = 0;
	index = pso->GetRootSignatureIndex("gParticles");
	commandList->SetComputeRootDescriptorTable(index, particleResourceHandle_);
	index = pso->GetRootSignatureIndex("gFreeListIndex");
	commandList->SetComputeRootDescriptorTable(index, freeListIndexHandle_);
	index = pso->GetRootSignatureIndex("gFreeList");
	commandList->SetComputeRootDescriptorTable(index, freeListHandle_);
	index = pso->GetRootSignatureIndex("gPerFrame");
	commandList->SetComputeRootConstantBufferView(index, perFrameBuffer_->GetGPUVirtualAddress());
	index = pso->GetRootSignatureIndex("gMaxParticles");
	commandList->SetComputeRootConstantBufferView(index, maxParticleResource_->GetGPUVirtualAddress());
	index = pso->GetRootSignatureIndex("gEmitter");
	commandList->SetComputeRootConstantBufferView(index, emitterResource_->GetGPUVirtualAddress());
	commandList->Dispatch(1, 1, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 形状の描画
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleEmitter::DrawShape(const Math::Matrix4x4& viewProjectionMatrix) const {
	if (emitterItem_.shape == (int)GpuEmitterShape::Sphere) {
		DrawSphere(emitterItem_.pos, emitterItem_.radius, viewProjectionMatrix);
	} else if(emitterItem_.shape == (int)GpuEmitterShape::Box) {
		Math::OBB obb{ .center = emitterItem_.pos, .size = emitterItem_.size };
		obb.MakeOBBAxis(Math::Quaternion::EulerToQuaternion(emitterItem_.rotate));
		DrawOBB(obb, viewProjectionMatrix);
	} else if (emitterItem_.shape == (int)GpuEmitterShape::Cone) {
		Math::Quaternion rotate = Math::Quaternion::EulerToQuaternion(emitterItem_.rotate);
		DrawCone(emitterItem_.pos, rotate, emitterItem_.radius, emitterItem_.angle, emitterItem_.height, viewProjectionMatrix);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存項目の設定
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleEmitter::SetItem() {
	emitterData_->color = emitterItem_.color;
	emitterData_->minScale = emitterItem_.minScale;
	emitterData_->maxScale = emitterItem_.maxScale;
	emitterData_->targetScale = emitterItem_.targetScale;
	emitterData_->rotate = emitterItem_.rotate;
	if (parentWorldMat_ == nullptr) {
		emitterData_->pos = emitterItem_.pos;
	} else {
		emitterData_->pos = emitterItem_.pos + parentWorldMat_->GetPosition();
	}
	emitterData_->count = 0;
	emitterData_->emitType = emitterItem_.emitType;
	emitterData_->emitOrigin = emitterItem_.emitOrigin;
	emitterData_->lifeOfScaleDown = emitterItem_.lifeOfScaleDown;
	emitterData_->lifeOfScaleUp = emitterItem_.lifeOfScaleUp;
	emitterData_->lifeOfAlpha = emitterItem_.lifeOfAlpha;

	emitterData_->separateByAxisScale = emitterItem_.separateByAxisScale;
	emitterData_->scaleMinScaler = emitterItem_.scaleMinScaler;
	emitterData_->scaleMaxScaler = emitterItem_.scaleMaxScaler;

	emitterData_->speed = emitterItem_.speed;
	emitterData_->lifeTime = emitterItem_.lifeTime;
	emitterData_->gravity = emitterItem_.gravity;
	emitterData_->damping = emitterItem_.damping;
	emitterData_->size = emitterItem_.size;
	emitterData_->radius = emitterItem_.radius;
	emitterData_->angle = emitterItem_.angle;
	emitterData_->height = emitterItem_.height;
	emitterData_->beAffectedByField = (uint32_t)emitterItem_.beAffectedByField;
}


void GpuParticleEmitter::SetParent(const Math::Matrix4x4& parentMat) {
	parentWorldMat_ = &parentMat;
	emitterData_->pos = emitterItem_.pos + parentWorldMat_->GetPosition();
	emitterData_->prePos = emitterData_->pos;
	preWorldPos_ = emitterData_->pos;
	hasPreWorldPos_ = true;
}
