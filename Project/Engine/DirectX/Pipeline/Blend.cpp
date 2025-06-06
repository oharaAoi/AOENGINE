#include "Blend.h"

Blend::Blend() {
}

Blend::~Blend() {
}

void Blend::Init() {
}

D3D12_BLEND_DESC Blend::SetBlend(const BlendMode& mode) {

	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	switch (mode) {
	case BlendMode::ModeNone:
		blendDesc.RenderTarget[0].BlendEnable = FALSE;
		break;
	case BlendMode::ModeNormal:
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		SetNormalBlend(blendDesc);
		break;
	case BlendMode::ModeAdd:
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		SetAddBlend(blendDesc);
		break;
	case BlendMode::ModeSubtract:
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		SetSubtractBlend(blendDesc);
		break;
	case BlendMode::ModeMultily:
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		SetMultiplyBlend(blendDesc);
		break;
	case BlendMode::ModeScreen:
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		SetScreenBlend(blendDesc);
		break;
	}

	return blendDesc;
}

D3D12_BLEND_DESC Blend::SetBlend(const std::string& modeName) {
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	if (modeName == "None") {
		blendDesc.RenderTarget[0].BlendEnable = FALSE;
	} else if (modeName == "Normal") {
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		SetNormalBlend(blendDesc);
	} else if (modeName == "Add") {
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		SetAddBlend(blendDesc);
	} else if (modeName == "Subtract") {
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		SetSubtractBlend(blendDesc);
	} else if (modeName == "Multily") {
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		SetMultiplyBlend(blendDesc);
	} else if (modeName == "Screen") {
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		SetScreenBlend(blendDesc);
	}

	return blendDesc;
}

void Blend::SetNormalBlend(D3D12_BLEND_DESC& blendDesc) {
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void Blend::SetAddBlend(D3D12_BLEND_DESC& blendDesc) {
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
}

void Blend::SetSubtractBlend(D3D12_BLEND_DESC& blendDesc) {
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
}

void Blend::SetMultiplyBlend(D3D12_BLEND_DESC& blendDesc) {
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
}

void Blend::SetScreenBlend(D3D12_BLEND_DESC& blendDesc) {
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
}
