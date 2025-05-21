#include "InstancingRenderer.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/System/Manager/TextureManager.h"

void InstancingRender(const D3D12_VERTEX_BUFFER_VIEW& vertexView,
					  const D3D12_INDEX_BUFFER_VIEW& indexView,
					  const D3D12_GPU_VIRTUAL_ADDRESS& materialBuffer,
					  const D3D12_GPU_DESCRIPTOR_HANDLE& instancingHandleGpu,
					  const std::string& useTexture, uint32_t indexNum, uint32_t instanceNum) {

	ID3D12GraphicsCommandList* commandList = GraphicsContext::GetInstance()->GetCommandList();

	commandList->IASetVertexBuffers(0, 1, &vertexView);
	commandList->IASetIndexBuffer(&indexView);
	commandList->SetGraphicsRootConstantBufferView(0, materialBuffer);
	commandList->SetGraphicsRootDescriptorTable(1, instancingHandleGpu);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, useTexture, 2);
	commandList->DrawIndexedInstanced(indexNum, instanceNum, 0, 0, 0);
}
