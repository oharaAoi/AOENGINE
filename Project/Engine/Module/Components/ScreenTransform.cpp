#include "ScreenTransform.h"

ScreenTransform::~ScreenTransform() {
}

void ScreenTransform::Init(ID3D12Device* _pDevice) {
	transformBuffer_ = CreateBufferResource(_pDevice, sizeof(TransformData));
	transformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&transformData_));

	transform_ = { {1.0f,1.0f,1.0f} , {0.0f, 0.0f, 0.0f}, {0, 0, 0} };
}

void ScreenTransform::Update(const Matrix4x4& _correctionMat, const Matrix4x4& _projMat) {
	Matrix4x4 affineMatrix = transform_.MakeAffine();
	transformData_->wvp = Matrix4x4(affineMatrix *  // ピボットによる変位を元に戻す
									_correctionMat *
									_projMat);
}

void ScreenTransform::BindCommand(ID3D12GraphicsCommandList* _cmd, uint32_t index) {
	_cmd->SetGraphicsRootConstantBufferView(index, transformBuffer_->GetGPUVirtualAddress());
}
