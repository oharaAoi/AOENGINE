#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <cassert>
#include <string>
#include "Engine/DirectX/Utilities/DirectXUtils.h"

namespace AOENGINE {

/// <summary>
/// Compilerクラス
/// </summary>
class DirectXCompiler {
public: // コンストラクタ

	DirectXCompiler() = default;
	~DirectXCompiler() = default;

public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	ComPtr<IDxcBlob> VsShaderCompile(const std::string& _shader);
	ComPtr<IDxcBlob> PsShaderCompile(const std::string& _shader);
	ComPtr<IDxcBlob> CsShaderCompile(const std::string& _shader);

	/// <summary>
	/// ShaderReflectionを返す
	/// </summary>
	/// <param name="blob"></param>
	/// <returns></returns>
	ComPtr<ID3D12ShaderReflection> ReadShaderReflection(IDxcBlob* _blob);

private:

	ComPtr<IDxcUtils> dxcUtils_ = nullptr;
	ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;
	ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;

};

}