#include "PrimitiveDrawer.h"
#include "Engine/Core/Engine.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/Lib/Math/MyMatrix.h"
#include "Engine/WinApp/WinApp.h"

using namespace AOENGINE;

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveDrawer::Finalize() {
	vertexBuffer_.Reset();
	indexBuffer_.Reset();
	wvpBuffer_.Reset();
	thickLineVertexBuffer_.Reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveDrawer::Init(ID3D12Device* device) {
	// ---------------------------------------------------------------
	// ↓Vertexの設定
	// ---------------------------------------------------------------
	// VertexBufferViewを作成する 
	vertexBuffer_ = CreateBufferResource(device, sizeof(PrimitiveData) * kMaxLineCount * kViewCount);
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
	//std::memcpy(vertexData_, , sizeof(Math::Vector4) * kMaxLineCount);

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
	wvpBuffer_ = CreateBufferResource(device, sizeof(Math::Matrix4x4) * kMaxLineCount);
	wvpData_ = nullptr;
	wvpBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	// Line頂点はViewごとにCPUでclip座標へ変換する。Shader側の行列は恒等行列で固定し、
	// 後から描画するEditor ViewがGame View用の行列を上書きしないようにする。
	wvpData_[0] = Math::Matrix4x4::MakeUnit();
	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;  // 頂点データなのでフォーマットはUNKNOWN
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = static_cast<UINT>(kMaxLineCount);  // 頂点の数
	srvDesc.Buffer.StructureByteStride = sizeof(Math::Matrix4x4);  // 頂点1つあたりのサイズ
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	wvpSRV_ = AOENGINE::GraphicsContext::GetInstance()->GetDxHeap()->AllocateSRV();
	// 生成
	device->CreateShaderResourceView(wvpBuffer_.Get(), &srvDesc, wvpSRV_.handleCPU);

	thickLineVertexBuffer_ = CreateBufferResource(device, sizeof(ThickLineData) * kMaxThickLineVertexCount * kViewCount);
	thickLineVertexBufferView_.BufferLocation = thickLineVertexBuffer_->GetGPUVirtualAddress();
	thickLineVertexBufferView_.SizeInBytes = UINT(sizeof(ThickLineData) * kMaxThickLineVertexCount);
	thickLineVertexBufferView_.StrideInBytes = sizeof(ThickLineData);
	thickLineVertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&thickLineData_));

	useIndex_ = 0;
	thickLineVertexCount_ = 0;
	lineRequests_.clear();
	thickLineRequests_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveDrawer::Update() {
	// 使用量を更新する
	useIndex_ = 0;
	preUseIndex_ = 0;
	thickLineVertexCount_ = 0;
	lineRequests_.clear();
	thickLineRequests_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveDrawer::Draw(const Math::Vector3& p1, const Math::Vector3& p2, const Color& color,
	LineView views, const Math::Matrix4x4* fixedVp) {
	LineRequest request{ .start = p1, .end = p2, .color = color, .views = views };
	if (fixedVp) { request.hasFixedVp = true; request.fixedVp = *fixedVp; }
	lineRequests_.push_back(request);
}

void PrimitiveDrawer::DrawThick(const Math::Vector3& p1, const Math::Vector3& p2, const Color& color,
	float thickness, LineView views, const Math::Matrix4x4* fixedVp) {
	if (thickness <= 0.0f) { return; }
	LineRequest request{ .start = p1, .end = p2, .color = color, .thickness = thickness, .views = views };
	if (fixedVp) { request.hasFixedVp = true; request.fixedVp = *fixedVp; }
	thickLineRequests_.push_back(request);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ instance描画を行う
///////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveDrawer::DrawCall(ID3D12GraphicsCommandList* commandList, LineView viewTarget, const Math::Matrix4x4& vpMat) {
	const uint8_t viewBit = static_cast<uint8_t>(viewTarget);
	const uint32_t viewIndex = viewTarget == LineView::Editor ? 1u : 0u;
	const uint32_t lineBufferOffset = viewIndex * kMaxLineCount;
	const uint32_t thickLineBufferOffset = viewIndex * kMaxThickLineVertexCount;
	useIndex_ = 0;
	for (LineRequest& request : lineRequests_) {
		if ((static_cast<uint8_t>(request.views) & viewBit) == 0 || (request.drawnViews & viewBit) != 0 || useIndex_ + 2 > kMaxLineCount) { continue; }
		const Math::Matrix4x4& matrix = request.hasFixedVp ? request.fixedVp : vpMat;
		primitiveData_[lineBufferOffset + useIndex_] = { Math::Vector4(request.start, 1.0f) * matrix, request.color };
		primitiveData_[lineBufferOffset + useIndex_ + 1] = { Math::Vector4(request.end, 1.0f) * matrix, request.color };
		useIndex_ += 2;
		request.drawnViews |= viewBit;
	}
	if (useIndex_ > 0) {
		Pipeline* pso = Engine::SetPipeline(PSOType::Primitive, "Primitive_Line.json");
		D3D12_VERTEX_BUFFER_VIEW view = vertexBufferView_;
		view.BufferLocation += static_cast<UINT64>(lineBufferOffset) * sizeof(PrimitiveData);
		commandList->IASetVertexBuffers(0, 1, &view);
		commandList->IASetIndexBuffer(&indexBufferView_);
		uint32_t roorIndex = pso->GetRootSignatureIndex("gTransformationMatrix");
		commandList->SetGraphicsRootDescriptorTable(roorIndex, wvpSRV_.handleGPU);
		commandList->DrawIndexedInstanced(useIndex_, 1, 0, 0, 0);
	}

	thickLineVertexCount_ = 0;
	const Math::Vector2 corners[kVertexCountThickLine] = {
		{ 0.0f, -0.5f }, { 0.0f, 0.5f }, { 1.0f, -0.5f },
		{ 1.0f, -0.5f }, { 0.0f, 0.5f }, { 1.0f, 0.5f }
	};
	for (LineRequest& request : thickLineRequests_) {
		if ((static_cast<uint8_t>(request.views) & viewBit) == 0 || (request.drawnViews & viewBit) != 0 || thickLineVertexCount_ + kVertexCountThickLine > kMaxThickLineVertexCount) { continue; }
		const Math::Matrix4x4& matrix = request.hasFixedVp ? request.fixedVp : vpMat;
		const Math::Vector4 start = Math::Vector4(request.start, 1.0f) * matrix;
		const Math::Vector4 end = Math::Vector4(request.end, 1.0f) * matrix;
		for (uint32_t vertex = 0; vertex < kVertexCountThickLine; ++vertex) {
			ThickLineData& data = thickLineData_[thickLineBufferOffset + thickLineVertexCount_ + vertex];
			data = { start, end, request.color, { corners[vertex].x, corners[vertex].y, request.thickness },
				{ static_cast<float>(WinApp::sClientWidth), static_cast<float>(WinApp::sClientHeight) } };
		}
		thickLineVertexCount_ += kVertexCountThickLine;
		request.drawnViews |= viewBit;
	}
	if (thickLineVertexCount_ == 0) { return; }

	Engine::SetPipeline(PSOType::Primitive, "Primitive_ThickLine.json");
	D3D12_VERTEX_BUFFER_VIEW view = thickLineVertexBufferView_;
	view.BufferLocation += static_cast<UINT64>(thickLineBufferOffset) * sizeof(ThickLineData);
	commandList->IASetVertexBuffers(0, 1, &view);
	commandList->DrawInstanced(thickLineVertexCount_, 1, 0, 0);
}
