#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <cassert>
#include <string>
#include "Engine/DirectX/Utilities/DirectXUtils.h"

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr <T>;

class DirectXCompiler {
public:

	DirectXCompiler() = default;
	~DirectXCompiler() = default;

	void Init();

	void Finalize();

	ComPtr<IDxcBlob> VsShaderCompile(const std::string& shader);
	ComPtr<IDxcBlob> PsShaderCompile(const std::string& shader);
	ComPtr<IDxcBlob> CsShaderCompile(const std::string& shader);

	ComPtr<ID3D12ShaderReflection> ReadShaderReflection(IDxcBlob* blob);

private:

	ComPtr<IDxcUtils> dxcUtils_ = nullptr;
	ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;
	ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;
	
};