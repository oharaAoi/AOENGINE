#include "PrimitiveDrawer.h"
#include "Engine/Engine.h"
#include "Engine/Core/GraphicsContext.h"

using namespace AOENGINE;

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveDrawer::Finalize() {
	vertexBuffer_.Reset();
	indexBuffer_.Reset();
	wvpBuffer_.Reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveDrawer::Init(ID3D12Device* device) {
	// ---------------------------------------------------------------
	// ↓Vertexの設定
	// ---------------------------------------------------------------
	// VertexBufferViewを作成する 
	vertexBuffer_ = CreateBufferResource(device, sizeof(PrimitiveData) * kMaxLineCount);
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(PrimitiveData) * kMaxLineCount);
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(PrimitiveData);
	// Resourceにデータを書き込む 
	primitiveData_ = nullptr;
	// アドレスを取得
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&primitiveData_));
	//std::memcpy(vertexData_, , sizeof(Vector4) * kMaxLineCount);

	// ---------------------------------------------------------------
	// ↓Indexの設定
	// ---------------------------------------------------------------
	indexBuffer_ = CreateBufferResource(device, sizeof(uint32_t) * kMaxLineCount);
	indexBufferView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * kMaxLineCount);
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexData_ = nullptr;
	indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	for (uint32_t oi = 0; oi < kMaxLineCount; oi++) {
		indexData_[oi] = oi;
	}

	// ---------------------------------------------------------------
	// ↓wvpの設定
	// ---------------------------------------------------------------
	wvpBuffer_ = CreateBufferResource(device, sizeof(Matrix4x4) * kMaxLineCount);
	wvpData_ = nullptr;
	wvpBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;  // 頂点データなのでフォーマットはUNKNOWN
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = static_cast<UINT>(kMaxLineCount);  // 頂点の数
	srvDesc.Buffer.StructureByteStride = sizeof(Matrix4x4);  // 頂点1つあたりのサイズ
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	wvpSRV_ = AOENGINE::GraphicsContext::GetInstance()->GetDxHeap()->AllocateSRV();
	// 生成
	device->CreateShaderResourceView(wvpBuffer_.Get(), &srvDesc, wvpSRV_.handleCPU);

	useIndex_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveDrawer::Update() {
	// 使用量を更新する
	useIndex_ = 0;
	preUseIndex_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveDrawer::Draw(const Vector3& p1, const Vector3& p2, const Color& color, const Matrix4x4& vpMat) {
	// 使用する頂点のインデックスの更新
	size_t materialIndex = (useIndex_) - 1;

	if (useIndex_ == 0) {
		materialIndex = 0;
	}

	// 頂点の設定
	primitiveData_[useIndex_].pos = { p1.x, p1.y, p1.z, 1 };
	primitiveData_[useIndex_ + 1].pos = { p2.x, p2.y, p2.z, 1.0f };

	// 色の設定
	primitiveData_[useIndex_].color = color;
	primitiveData_[useIndex_ + 1].color = color;

	Matrix4x4 mat1 = Matrix4x4::MakeUnit();
	Matrix4x4 mat2 = Matrix4x4::MakeUnit();

	mat1 = Multiply(Multiply(mat1, Vector3(p1.x, p1.y, p1.z).MakeTranslateMat()), vpMat);
	mat2 = Multiply(Multiply(mat2, Vector3(p2.x, p2.y, p2.z).MakeTranslateMat()), vpMat);

	wvpData_[useIndex_] = vpMat;
	wvpData_[useIndex_ + 1] = vpMat;
	
	useIndex_ += 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ instance描画を行う
///////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveDrawer::DrawCall(ID3D12GraphicsCommandList* commandList) {
	Pipeline* pso = Engine::SetPipeline(PSOType::Primitive, "Primitive_Line.json");

	uint32_t indeices = useIndex_ - preUseIndex_;
	if (indeices == 0) {
		return; // 描画するものがないので早期リターン
	}
	// コマンドリストの設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBufferView_);
	uint32_t roorIndex = 0;
	roorIndex = pso->GetRootSignatureIndex("gTransformationMatrix");
	commandList->SetGraphicsRootDescriptorTable(roorIndex, wvpSRV_.handleGPU);

	// インデックスを使用して線を描画
	commandList->DrawIndexedInstanced(indeices, indeices / 2, preUseIndex_, 0, 0);

	preUseIndex_ = useIndex_;
}
