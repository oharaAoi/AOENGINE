#include "TransformationMatrix.h"

TransformationMatrix::TransformationMatrix() {
}

TransformationMatrix::~TransformationMatrix() {
	Finalize();
}

void TransformationMatrix::Finalize() {
	cBuffer_.Reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void TransformationMatrix::Init(ID3D12Device* device, const uint32_t& instanceSize) {
	instanceSize_ = instanceSize;
	cBuffer_ = CreateBufferResource(device, sizeof(TransformationData) * instanceSize);
	// データをマップ
	cBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&TransformationData_));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void TransformationMatrix::Update(const Math::Matrix4x4& world, const Math::Matrix4x4& view, const Math::Matrix4x4& projection) {
	for (uint32_t oi = 0; oi < instanceSize_; oi++) {
		TransformationData_[oi].world = world;
		TransformationData_[oi].view = view;
		TransformationData_[oi].projection = projection;
		TransformationData_[oi].worldInverseTranspose = Inverse(world).Transpose();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンド処理
///////////////////////////////////////////////////////////////////////////////////////////////

void TransformationMatrix::BindCommand(ID3D12GraphicsCommandList* commandList) {
	commandList->SetGraphicsRootConstantBufferView(1, cBuffer_->GetGPUVirtualAddress());
}

