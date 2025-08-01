#include "GpuParticleEmitter.h"
#include "Engine/Engine.h"
#include "Engine/Render.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/Utilities/DrawUtils.h"

GpuParticleEmitter::~GpuParticleEmitter() {
	emitterResource_.Reset();
	perFrameBuffer_.Reset();
}

void GpuParticleEmitter::Debug_Gui() {
	ImGui::Text("emitAccumulator : %f", emitAccumulator_);
	emitterItem_.Attribute_Gui();
	if (ImGui::Button("Save")) {
		JsonItems::Save("GPU", emitterItem_.ToJson(GetName()), "Effect");
	}
	if (ImGui::Button("Apply")) {
		emitterItem_.FromJson(JsonItems::GetData("GPU", GetName()));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化関数
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleEmitter::Init(const std::string& name) {
	GraphicsContext* ctx = GraphicsContext::GetInstance();
	dxDevice_ = ctx->GetDevice();
	commandList_ = ctx->GetCommandList();
	
	SetName(name.c_str());
	emitterItem_.FromJson(JsonItems::GetData("GPU", name));

	emitterResource_ = CreateBufferResource(dxDevice_, sizeof(GpuParticleEmitterData));
	emitterResource_->Map(0, nullptr, reinterpret_cast<void**>(&emitterData_));

	// preFrameの作成
	perFrameBuffer_ = CreateBufferResource(dxDevice_, sizeof(PerFrame));
	perFrameBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&perFrame_));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleEmitter::Update() {
	if (isStop_) { return; }
	perFrame_->deltaTime = GameTimer::DeltaTime();
	perFrame_->time = GameTimer::TotalTime();

	// -------------------------------------------------
	// ↓ 発射処理
	// -------------------------------------------------
	if (!emitterItem_.isLoop) {
		for (uint32_t count = 0; count < emitterItem_.rateOverTimeCout; ++count) {
			EmitCommand(commandList_);
		}
		isStop_ = true;
	}

	emitAccumulator_ += emitterItem_.rateOverTimeCout * GameTimer::DeltaTime();
	// 発射すべき個数を計算する
	int emitCout = static_cast<int>(emitAccumulator_);
	emitCount_ = emitCout;
	if (emitCount_ != 0) {
		EmitCommand(commandList_);
	}
	emitAccumulator_ -= emitCout;

	// -------------------------------------------------
	// ↓ 継続時間を進める
	// -------------------------------------------------
	currentTimer_ += GameTimer::DeltaTime();
	if (currentTimer_ > emitterItem_.duration) {
		if (!emitterItem_.isLoop) {
			isStop_ = true;
		}
	}

	SetItem();
}

void GpuParticleEmitter::EmitCommand(ID3D12GraphicsCommandList* commandList) {
	if (emitterItem_.shape == GpuEmitterShape::SPHERE) {
		Engine::SetPipelineCS("GpuParticleEmit.json");
	} else if(emitterItem_.shape == GpuEmitterShape::BOX) {
		Engine::SetPipelineCS("GpuParticleBoxEmit.json");
	} else if (emitterItem_.shape == GpuEmitterShape::CONE) {
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

void GpuParticleEmitter::DrawShape() const {
	if (emitterItem_.shape == GpuEmitterShape::SPHERE) {
		DrawSphere(emitterItem_.pos, emitterItem_.radius, Render::GetViewProjectionMat());
	} else if(emitterItem_.shape == GpuEmitterShape::BOX) {
		OBB obb{ .center = emitterItem_.pos, .size = emitterItem_.size };
		obb.MakeOBBAxis(Quaternion::EulerToQuaternion(emitterItem_.rotate));
		DrawOBB(obb, Render::GetViewProjectionMat());
	} else if (emitterItem_.shape == GpuEmitterShape::CONE) {
		Quaternion rotate = Quaternion::EulerToQuaternion(emitterItem_.rotate);
		DrawCone(emitterItem_.pos, rotate, emitterItem_.radius, emitterItem_.angle, emitterItem_.height, Render::GetViewProjectionMat());
	}
}

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
	emitterData_->count = emitterItem_.rateOverTimeCout;
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
}


void GpuParticleEmitter::SetParent(const Matrix4x4& parentMat) {
	parentWorldMat_ = &parentMat;
}