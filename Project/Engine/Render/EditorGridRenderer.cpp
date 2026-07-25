#include "EditorGridRenderer.h"

#include "Engine/Core/Engine.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Lib/Math/MyMatrix.h"

using namespace AOENGINE;

EditorGridRenderer::~EditorGridRenderer() {
	Finalize();
}

void EditorGridRenderer::Init(ID3D12Device* device) {
	assert(device);
	assert(!parameterResource_);

	// CBVのGPU仮想アドレス制約に合わせて256byte確保する。
	parameterResource_ = CreateBufferResource(device, 256);
	const HRESULT result = parameterResource_->Map(
		0, nullptr, reinterpret_cast<void**>(&parameter_));
	assert(SUCCEEDED(result));
}

void EditorGridRenderer::Finalize() {
	if (parameterResource_ && parameter_) {
		parameterResource_->Unmap(0, nullptr);
	}
	parameter_ = nullptr;
	parameterResource_.Reset();
}

void EditorGridRenderer::Draw(
	ID3D12GraphicsCommandList* commandList,
	const Math::Matrix4x4& viewMatrix,
	const Math::Matrix4x4& projectionMatrix,
	bool hasMotionVectorTarget) {
	assert(commandList);
	assert(parameterResource_);
	assert(parameter_);

	const Math::Matrix4x4 viewProjection = Multiply(viewMatrix, projectionMatrix);
	const Math::Matrix4x4 inverseView = viewMatrix.Inverse();

	parameter_->inverseViewProjection = viewProjection.Inverse();
	parameter_->viewProjection = viewProjection;
	parameter_->cameraPosition = inverseView.GetPosition();
	parameter_->baseGridSize = 1.0f;
	parameter_->fadeStart = 50.0f;
	parameter_->fadeEnd = 500.0f;
	parameter_->padding[0] = 0.0f;
	parameter_->padding[1] = 0.0f;

	Pipeline* pipeline = Engine::SetPipeline(
		PSOType::ProcessedScene,
		hasMotionVectorTarget ? "EditorGrid.json" : "EditorGridSingleTarget.json");

	const UINT parameterIndex =
		pipeline->GetRootSignatureIndex("gGridParameter");
	commandList->SetGraphicsRootConstantBufferView(
		parameterIndex, parameterResource_->GetGPUVirtualAddress());
	commandList->DrawInstanced(3, 1, 0, 0);
}
