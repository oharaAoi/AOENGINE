#include "Pipeline.h"
#include "Engine/Utilities/Logger.h"

Pipeline::Pipeline() {}
Pipeline::~Pipeline() {}

void Pipeline::Init(ID3D12Device* device, DirectXCompiler* dxCompiler, const json& jsonData) {
	parameter_.FromJson(jsonData);

	device_ = device;
	dxCompiler_ = dxCompiler;

	if (parameter_.cs == "") {
		ShaderCompile();
		vsReflection_ = dxCompiler->ReadShaderReflection(vertexShaderBlob_.Get());
		psReflection_ = dxCompiler->ReadShaderReflection(pixelShaderBlob_.Get());

		elementDescs = CreateInputLayout();
		rootSig_ = CreateRootSignature();
		CreatePSO();
	} else {
		ComPtr<IDxcBlob> computeShaderBlob_ = dxCompiler_->CsShaderCompile(parameter_.cs);
		csReflection_ = dxCompiler->ReadShaderReflection(computeShaderBlob_.Get());
		rootSig_ = CreateRootSignature();

		D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
		desc.pRootSignature = rootSig_.Get();
		desc.CS = { reinterpret_cast<BYTE*>(computeShaderBlob_->GetBufferPointer()),computeShaderBlob_->GetBufferSize() };

		HRESULT hr = device_->CreateComputePipelineState(&desc, IID_PPV_ARGS(&graphicsPipelineState_));
		assert(SUCCEEDED(hr));
	}
}

void Pipeline::Draw(ID3D12GraphicsCommandList* commandList) {
	commandList->SetGraphicsRootSignature(rootSig_.Get());
	commandList->SetPipelineState(graphicsPipelineState_.Get());
}

void Pipeline::SetComputeState(ID3D12GraphicsCommandList* commandList) {
	commandList->SetPipelineState(graphicsPipelineState_.Get());
	commandList->SetComputeRootSignature(rootSig_.Get());
}

void Pipeline::Finalize() {
	rootSig_.Reset();
	vertexShaderBlob_.Reset();
	pixelShaderBlob_.Reset();
	graphicsPipelineState_.Reset();
}

//------------------------------------------------------------------------------------------------------
// ↓Iputlayoutの生成
//------------------------------------------------------------------------------------------------------
D3D12_INPUT_LAYOUT_DESC Pipeline::CreateInputLayout(const std::vector<D3D12_INPUT_ELEMENT_DESC>& elementDesc) {
	D3D12_INPUT_LAYOUT_DESC result{};
	result.pInputElementDescs = elementDesc.data();
	result.NumElements = static_cast<UINT>(elementDesc.size());
	return result;
}

//------------------------------------------------------------------------------------------------------
// ↓shaderを読む
//------------------------------------------------------------------------------------------------------
void Pipeline::ShaderCompile() {
	vertexShaderBlob_ = dxCompiler_->VsShaderCompile(parameter_.vs);
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = dxCompiler_->PsShaderCompile(parameter_.ps);
	assert(pixelShaderBlob_ != nullptr);
}

//------------------------------------------------------------------------------------------------------
// ↓ラスタライズの設定
//------------------------------------------------------------------------------------------------------
D3D12_RASTERIZER_DESC Pipeline::SetRasterizerState(bool isCulling) {
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面を表示しない
	if (isCulling) {
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	} else {
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	}
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	return rasterizerDesc;
}

/// <summary>
/// DepthStencilStateの設定
/// </summary>
/// <returns></returns>
D3D12_DEPTH_STENCIL_DESC Pipeline::SetDepthStencilState(bool isDepth) {
	D3D12_DEPTH_STENCIL_DESC desc{};
	// Depthの機能を有効化する
	desc.DepthEnable = true;
	// 書き込み
	//desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	if (isDepth) {
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	} else {
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	}

	// 地下駆ければ描画
	desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	return desc;
}

//------------------------------------------------------------------------------------------------------
// ↓PSOの追加
//------------------------------------------------------------------------------------------------------
void Pipeline::CreatePSO() {
	// PSOの生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
	desc.pRootSignature = rootSig_.Get();
	if (elementDescs.empty()) {
		desc.InputLayout.NumElements = 0;
		desc.InputLayout.pInputElementDescs = nullptr;
	} else {
		desc.InputLayout = CreateInputLayout(elementDescs);
	}
	desc.VS = { vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize() };
	desc.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() };
	desc.BlendState = blend_.SetBlend(parameter_.blendMode);
	desc.RasterizerState = SetRasterizerState(parameter_.culling);
	desc.DepthStencilState = SetDepthStencilState(parameter_.depth);
	desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 書き込むRTVの情報
	desc.NumRenderTargets = static_cast<UINT>(parameter_.rtvFormats.size());
	for (uint32_t oi = 0; oi < parameter_.rtvFormats.size(); ++oi) {
		if (parameter_.rtvFormats[oi] == "16_RGB") {
			desc.RTVFormats[oi] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		} else if (parameter_.rtvFormats[oi] == "8_sRGB") {
			desc.RTVFormats[oi] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		} else if (parameter_.rtvFormats[oi] == "32_R") {
			desc.RTVFormats[oi] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		}

	}
	// 利用するトポロジ(形状)のタイプ
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定
	desc.SampleDesc.Count = 1;
	desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	HRESULT hr = device_->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ InputLayoutの生成
///////////////////////////////////////////////////////////////////////////////////////////////

std::vector<D3D12_INPUT_ELEMENT_DESC> Pipeline::CreateInputLayout() {
	D3D12_SHADER_DESC shaderDesc;
	vsReflection_->GetDesc(&shaderDesc);

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
	for (UINT i = 0; i < shaderDesc.InputParameters; ++i) {
		D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
		vsReflection_->GetInputParameterDesc(i, &paramDesc);

		D3D12_INPUT_ELEMENT_DESC elementDesc = {};
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.Format = ReturnFormat(paramDesc.SemanticName);
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		inputLayout.push_back(elementDesc);
	}

	return inputLayout;
}

DXGI_FORMAT Pipeline::ReturnFormat(LPCSTR name) {
	if (strcmp(name, "TEXCOORD") == 0) {
		return DXGI_FORMAT_R32G32_FLOAT;
	} else if (strcmp(name, "NORMAL") == 0 || strcmp(name, "TANGENT") == 0) {
		return DXGI_FORMAT_R32G32B32_FLOAT;
	} else if (strcmp(name, "INDEX") == 0) {
		return DXGI_FORMAT_R32G32B32A32_SINT;
	} else {
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ RootSignatureの生成
///////////////////////////////////////////////////////////////////////////////////////////////

ComPtr<ID3D12RootSignature> Pipeline::CreateRootSignature() {
	std::vector<D3D12_ROOT_PARAMETER> rootParameters;
	std::vector<D3D12_DESCRIPTOR_RANGE> srvRangesVertex;
	std::vector<D3D12_DESCRIPTOR_RANGE> srvRangesPixel;
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers;
	std::set<BindingKey> processed;

	UINT baseDescriptorIndex = 0;

	auto ProcessReflection = [&](ID3D12ShaderReflection* reflection, D3D12_SHADER_VISIBILITY visibility) {
		if (!reflection) return;

		D3D12_SHADER_DESC desc;
		reflection->GetDesc(&desc);

		for (UINT i = 0; i < desc.BoundResources; ++i) {
			D3D12_SHADER_INPUT_BIND_DESC bindDesc;
			reflection->GetResourceBindingDesc(i, &bindDesc);

			BindingKey key{ bindDesc.Type, bindDesc.BindPoint, bindDesc.Space, visibility };
			//if (processed.count(key)) continue;
			processed.insert(key);

			switch (bindDesc.Type) {
			case D3D_SIT_CBUFFER: {
				D3D12_ROOT_PARAMETER param = {};
				param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
				param.ShaderVisibility = visibility;
				param.Descriptor.ShaderRegister = bindDesc.BindPoint;
				param.Descriptor.RegisterSpace = bindDesc.Space;

				UINT index = (UINT)rootParameters.size();
				rootParameters.push_back(param);

				rootSignatureIndexMap_[bindDesc.Name] = index;
				break;
			}

			case D3D_SIT_TEXTURE:
			case D3D_SIT_STRUCTURED:
			case D3D_SIT_BYTEADDRESS:
			case D3D_SIT_TBUFFER: {
				descriptorRangeTables.emplace_back(); // 新しいテーブル
				auto& currentTable = descriptorRangeTables.back();

				D3D12_DESCRIPTOR_RANGE range = {};
				range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				range.NumDescriptors = bindDesc.BindCount;
				range.BaseShaderRegister = bindDesc.BindPoint;
				range.RegisterSpace = bindDesc.Space;
				range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

				if (visibility == D3D12_SHADER_VISIBILITY_VERTEX) {
					srvRangesVertex.push_back(range);
				} else if (visibility == D3D12_SHADER_VISIBILITY_PIXEL) {
					srvRangesPixel.push_back(range);
				}

				currentTable.push_back(range);

				D3D12_ROOT_PARAMETER param = {};
				param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				param.ShaderVisibility = visibility;
				param.DescriptorTable.NumDescriptorRanges = (UINT)currentTable.size();
				param.DescriptorTable.pDescriptorRanges = currentTable.data();

				UINT index = (UINT)rootParameters.size();
				rootParameters.push_back(param);

				rootSignatureIndexMap_[bindDesc.Name] = index;

				baseDescriptorIndex++;

				break;
			}

			case D3D_SIT_SAMPLER: {
				D3D12_STATIC_SAMPLER_DESC sampler = {};
				auto it = samplerOverrides_.find(bindDesc.Name);
				if (it != samplerOverrides_.end()) {
					sampler = it->second; // コピー
				} else {
					// 名前未登録 → デフォルト Linear Wrap
					sampler = MakeStaticSampler(D3D12_FILTER_MIN_MAG_MIP_LINEAR);
				}

				sampler.ShaderRegister = bindDesc.BindPoint;
				sampler.RegisterSpace = bindDesc.Space;
				sampler.ShaderVisibility = visibility;

				staticSamplers.push_back(sampler);
				break;
			}

			default:
				descriptorRangeTables.emplace_back(); // 新しいテーブル
				auto& currentTable = descriptorRangeTables.back();

				D3D12_DESCRIPTOR_RANGE range = {};
				range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
				range.NumDescriptors = bindDesc.BindCount;
				range.BaseShaderRegister = bindDesc.BindPoint;
				range.RegisterSpace = bindDesc.Space;
				range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

				currentTable.push_back(range);

				D3D12_ROOT_PARAMETER param = {};
				param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				param.ShaderVisibility = visibility; // たいてい D3D12_SHADER_VISIBILITY_ALL を使う
				param.DescriptorTable.NumDescriptorRanges = (UINT)currentTable.size();
				param.DescriptorTable.pDescriptorRanges = currentTable.data();

				UINT index = (UINT)rootParameters.size();
				rootParameters.push_back(param);

				rootSignatureIndexMap_[bindDesc.Name] = index;

				baseDescriptorIndex++;
				break;
			}
		}
		};

	// シェーダー反映（可視性ごとに処理）
	if (parameter_.cs == "") {
		ProcessReflection(vsReflection_.Get(), D3D12_SHADER_VISIBILITY_VERTEX);
		ProcessReflection(psReflection_.Get(), D3D12_SHADER_VISIBILITY_PIXEL);
	} else {
		ProcessReflection(csReflection_.Get(), D3D12_SHADER_VISIBILITY_ALL);
	}

	// RootSignature 記述と作成
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.NumParameters = (UINT)rootParameters.size();
	desc.pParameters = rootParameters.data();
	desc.NumStaticSamplers = (UINT)staticSamplers.size();
	desc.pStaticSamplers = staticSamplers.data();
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> sigBlob, errBlob;
	HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
	if (FAILED(hr)) {
		if (errBlob) OutputDebugStringA((char*)errBlob->GetBufferPointer());
		assert(SUCCEEDED(hr));
		return nullptr;
	}

	ComPtr<ID3D12RootSignature> rootSig;
	hr = device_->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSig));
	if (FAILED(hr)) {
		assert(SUCCEEDED(hr));
		return nullptr;
	}

	return rootSig;
}

void Pipeline::SamplerOverrides() {
	samplerOverrides_["gSampler"] = MakeStaticSampler(D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	samplerOverrides_["gSamplerPoint"] = MakeStaticSampler(D3D12_FILTER_MIN_MAG_MIP_POINT);
	samplerOverrides_["gSamplerAnisoWrap"] = MakeStaticSampler(D3D12_FILTER_ANISOTROPIC);
}

D3D12_STATIC_SAMPLER_DESC Pipeline::MakeStaticSampler(D3D12_FILTER filter, D3D12_TEXTURE_ADDRESS_MODE addr, UINT maxAniso) {
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = filter;
	sampler.AddressU = sampler.AddressV = sampler.AddressW = addr;
	sampler.MipLODBias = 0.f;
	sampler.MaxAnisotropy = maxAniso;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	sampler.MinLOD = 0.f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	return sampler;
}

const UINT Pipeline::GetRootSignatureIndex(const std::string& name) const {
	auto it = rootSignatureIndexMap_.find(name);
	if (it == rootSignatureIndexMap_.end()) {
		Logger::Log("class : Pipeline");
		Logger::AssertLog(name + "が見つかりません");
		return UINT_MAX; // 見つからないことを示す値
	}
	return it->second;
}
