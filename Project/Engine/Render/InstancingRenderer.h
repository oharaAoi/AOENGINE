#pragma once
#include <string>
#include "Engine/DirectX/Utilities/DirectXUtils.h"

void InstancingRender(const D3D12_VERTEX_BUFFER_VIEW& vertexView, 
					  const D3D12_INDEX_BUFFER_VIEW& indexView,
					  const D3D12_GPU_VIRTUAL_ADDRESS& materialBuffer,
					  const D3D12_GPU_DESCRIPTOR_HANDLE& instancingHandleGpu,
					  const std::string& useTexture,
					  uint32_t indexNum,
					  uint32_t instanceNum);