#include "ViewProjection.h"

ViewProjection::ViewProjection() {
}

ViewProjection::~ViewProjection() {
}

void ViewProjection::Finalize() {
	cBuffer_.Reset();
	cBufferPrev_.Reset();
}

void ViewProjection::Init(ID3D12Device* device) {
	cBuffer_ = CreateBufferResource(device, sizeof(ViewProjectionData));
	// データをマップ
	cBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

	cBufferPrev_ = CreateBufferResource(device, sizeof(ViewProjectionData));
	// データをマップ
	cBufferPrev_->Map(0, nullptr, reinterpret_cast<void**>(&dataPrev_));
}

void ViewProjection::BindCommandList(ID3D12GraphicsCommandList* commandList, UINT index) const {
	commandList->SetGraphicsRootConstantBufferView(index, cBuffer_->GetGPUVirtualAddress());
}

void ViewProjection::BindCommandListPrev(ID3D12GraphicsCommandList* commandList, UINT index) const {
 	commandList->SetGraphicsRootConstantBufferView(index, cBufferPrev_->GetGPUVirtualAddress());
}