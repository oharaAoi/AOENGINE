#pragma once
// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <memory>
#include <vector>

class InputLayout {
public:

	InputLayout();
	~InputLayout();

	void Initialize();

	void Finalize();

	std::vector<D3D12_INPUT_ELEMENT_DESC> Create();

	std::vector<D3D12_INPUT_ELEMENT_DESC> CreatePrimitive();

	std::vector<D3D12_INPUT_ELEMENT_DESC> CreateParticle();

	std::vector<D3D12_INPUT_ELEMENT_DESC> CreateSprite();

	std::vector<D3D12_INPUT_ELEMENT_DESC> CreateSkinning();

	std::vector<D3D12_INPUT_ELEMENT_DESC> CreateRenderTexture();

	std::vector<D3D12_INPUT_ELEMENT_DESC> CreateSkybox();

};