#include "PostProcess.h"

PostProcess::~PostProcess() {
}

void PostProcess::Finalize() {
	pingPongBuff_.reset();
}

void PostProcess::Init(ID3D12Device* device, DescriptorHeap* descriptorHeap) {
	pingPongBuff_ = std::make_unique<PingPongBuffer>();

	pingPongBuff_->Init(device, descriptorHeap);

	grayscale_ = std::make_unique<Grayscale>();
	grayscale_->Init();
}

void PostProcess::Execute(ID3D12GraphicsCommandList* commandList, ShaderResource* shaderResource) {
	Copy(commandList, shaderResource);

	pingPongBuff_->SetRenderTarget(commandList);
	grayscale_->SetCommand(commandList, pingPongBuff_->GetPingResource());

	//pingPongBuff_->Swap();

	PostCopy(commandList, shaderResource);
}

void PostProcess::Copy(ID3D12GraphicsCommandList* commandList, ShaderResource* shaderResource) {
	shaderResource->Transition(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
	pingPongBuff_->Transition(commandList, D3D12_RESOURCE_STATE_COPY_DEST, BufferType::PING);
	commandList->CopyResource(pingPongBuff_->GetPingResource()->GetResource(), shaderResource->GetResource());

	pingPongBuff_->Transition(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, BufferType::PING);
}

void PostProcess::PostCopy(ID3D12GraphicsCommandList* commandList, ShaderResource* shaderResource) {
	pingPongBuff_->Transition(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE, BufferType::PONG);
	shaderResource->Transition(commandList, D3D12_RESOURCE_STATE_COPY_DEST);

	commandList->CopyResource(shaderResource->GetResource(), pingPongBuff_->GetPongResource()->GetResource());

	shaderResource->Transition(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	pingPongBuff_->Transition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, BufferType::PONG);
}
