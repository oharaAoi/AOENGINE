#include "PipelineFactory.h"


void PipelineFactory::Init(DirectXDevice* dxDevice, DirectXCompiler* dxCompiler) {
	dxDevice_ = dxDevice;
	dxCompiler_ = dxCompiler;
	
	ShaderCompile();
	vsReflection_ = dxCompiler->ReadShaderReflection(vertexShaderBlob_.Get());
	psReflection_ = dxCompiler->ReadShaderReflection(pixelShaderBlob_.Get());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ ShaderBlobの生成
///////////////////////////////////////////////////////////////////////////////////////////////

void PipelineFactory::ShaderCompile() {
	vertexShaderBlob_ = dxCompiler_->VsShaderCompile(vsName_);
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = dxCompiler_->PsShaderCompile(psName_);
	assert(pixelShaderBlob_ != nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ InputLayoutの生成
///////////////////////////////////////////////////////////////////////////////////////////////

std::vector<D3D12_INPUT_ELEMENT_DESC> PipelineFactory::CreateInputLayout() {
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

DXGI_FORMAT PipelineFactory::ReturnFormat(LPCSTR name) {
	if (name == static_cast<LPCSTR>("TEXCOORD")) {
		return DXGI_FORMAT_R32G32_FLOAT;
	} else if (name == static_cast<LPCSTR>("NORMAL") || name == static_cast<LPCSTR>("TANGENT")) {
		return DXGI_FORMAT_R32G32B32_FLOAT;
	} else if (name == static_cast<LPCSTR>("INDEX")) {
		return DXGI_FORMAT_R32G32B32A32_SINT;
	} else {
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ RootSignatureの生成
///////////////////////////////////////////////////////////////////////////////////////////////

ComPtr<ID3D12RootSignature> PipelineFactory::CreateRootSignature() {
	D3D12_SHADER_DESC shaderDesc;
	psReflection_->GetDesc(&shaderDesc);

	std::vector<D3D12_ROOT_PARAMETER> rootParameters;
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRangesSRV;
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRangesUAV;
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers;

	for (UINT i = 0; i < shaderDesc.BoundResources; ++i) {
		D3D12_SHADER_INPUT_BIND_DESC bindDesc = {};
		psReflection_->GetResourceBindingDesc(i, &bindDesc);

		switch (bindDesc.Type) {
		case D3D_SIT_CBUFFER: {
			// Constant Buffer View (CBV)
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			param.Descriptor.ShaderRegister = bindDesc.BindPoint;
			param.Descriptor.RegisterSpace = bindDesc.Space;
			rootParameters.push_back(param);
			break;
		}

		case D3D_SIT_TEXTURE:
		case D3D_SIT_STRUCTURED:
		case D3D_SIT_BYTEADDRESS:
		case D3D_SIT_TBUFFER: {
			// SRV
			D3D12_DESCRIPTOR_RANGE range = {};
			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			range.NumDescriptors = bindDesc.BindCount;
			range.BaseShaderRegister = bindDesc.BindPoint;
			range.RegisterSpace = bindDesc.Space;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			descriptorRangesSRV.push_back(range);
			break;
		}

		case D3D_SIT_UAV_RWTYPED:
		case D3D_SIT_UAV_RWSTRUCTURED:
		case D3D_SIT_UAV_RWBYTEADDRESS: {
			// UAV
			D3D12_DESCRIPTOR_RANGE range = {};
			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			range.NumDescriptors = bindDesc.BindCount;
			range.BaseShaderRegister = bindDesc.BindPoint;
			range.RegisterSpace = bindDesc.Space;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			descriptorRangesUAV.push_back(range);
			break;
		}

		case D3D_SIT_SAMPLER: {
			// Static Sampler
			D3D12_STATIC_SAMPLER_DESC sampler = {};
			sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler.ShaderRegister = bindDesc.BindPoint;
			sampler.RegisterSpace = bindDesc.Space;
			sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			staticSamplers.push_back(sampler);
			break;
		}
		}
	}

	// SRVテーブル追加
	if (!descriptorRangesSRV.empty()) {
		D3D12_ROOT_PARAMETER srvTable = {};
		srvTable.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		srvTable.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		srvTable.DescriptorTable.NumDescriptorRanges = (UINT)descriptorRangesSRV.size();
		srvTable.DescriptorTable.pDescriptorRanges = descriptorRangesSRV.data();
		rootParameters.push_back(srvTable);
	}

	// UAVテーブル追加
	if (!descriptorRangesUAV.empty()) {
		D3D12_ROOT_PARAMETER uavTable = {};
		uavTable.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		uavTable.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		uavTable.DescriptorTable.NumDescriptorRanges = (UINT)descriptorRangesUAV.size();
		uavTable.DescriptorTable.pDescriptorRanges = descriptorRangesUAV.data();
		rootParameters.push_back(uavTable);
	}

	// ルートシグネチャの記述
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.NumParameters = (UINT)rootParameters.size();
	rootSigDesc.pParameters = rootParameters.data();
	rootSigDesc.NumStaticSamplers = (UINT)staticSamplers.size();
	rootSigDesc.pStaticSamplers = staticSamplers.data();
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// シリアライズ
	ComPtr<ID3DBlob> serializedRootSig;
	ComPtr<ID3DBlob> errorBlob;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
											 &serializedRootSig, &errorBlob);
	if (FAILED(hr)) {
		if (errorBlob) {
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		return nullptr;
	}

	// ルートシグネチャ作成
	ComPtr<ID3D12RootSignature> rootSig;
	dxDevice_->GetDevice()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
								serializedRootSig->GetBufferSize(),
								IID_PPV_ARGS(&rootSig));

	return rootSig;
}
