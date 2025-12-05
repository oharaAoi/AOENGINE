#include "DirectXCompiler.h"
#pragma comment(lib, "dxcompiler.lib")

using namespace AOENGINE;

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void DirectXCompiler::Init() {
	HRESULT hr = S_FALSE;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));

	// includeに対応するための設定
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void DirectXCompiler::Finalize() {
	dxcUtils_.Reset();
	dxcCompiler_.Reset();
	includeHandler_.Reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ vertex shaderのコンパイル
///////////////////////////////////////////////////////////////////////////////////////////////

ComPtr<IDxcBlob> DirectXCompiler::VsShaderCompile(const std::string& _shader) {
	ComPtr<IDxcBlob> result{};
	result = CompilerShader(ConvertWString(_shader), L"main", L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(result != nullptr);

	return result.Get();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ pixel shaderのコンパイル
///////////////////////////////////////////////////////////////////////////////////////////////

ComPtr<IDxcBlob> DirectXCompiler::PsShaderCompile(const std::string& _shader) {
	ComPtr<IDxcBlob> result{};
	result = CompilerShader(ConvertWString(_shader), L"main", L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(result != nullptr);

	return result.Get();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ compute shaderのコンパイル
///////////////////////////////////////////////////////////////////////////////////////////////

ComPtr<IDxcBlob> DirectXCompiler::CsShaderCompile(const std::string& _shader) {
	ComPtr<IDxcBlob> result{};
	result = CompilerShader(ConvertWString(_shader), L"CSmain", L"cs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(result != nullptr);

	return result.Get();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ shaderからパイプライン生成
///////////////////////////////////////////////////////////////////////////////////////////////

ComPtr<ID3D12ShaderReflection> DirectXCompiler::ReadShaderReflection(IDxcBlob* _blob) {
	ComPtr<IDxcUtils> dxcUtils;
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));

	DxcBuffer dxcBuffer = {};
	dxcBuffer.Ptr = _blob->GetBufferPointer();
	dxcBuffer.Size = _blob->GetBufferSize();
	dxcBuffer.Encoding = DXC_CP_ACP; // ANSI, 通常これでOK

	ComPtr<ID3D12ShaderReflection> pReflector;
	hr = dxcUtils->CreateReflection(
		&dxcBuffer,
		IID_PPV_ARGS(&pReflector)
	);
	assert(SUCCEEDED(hr));

	return pReflector;
}
