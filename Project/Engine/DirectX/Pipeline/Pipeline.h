#pragma once
// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <set>
#include <memory>
#include <vector>
#include <string>
// PSO
#include "Engine/DirectX/RootSignature/RootSignature.h"
#include "InputLayout.h"
#include "Engine/DirectX/DirectXCompiler/DirectXCompiler.h"
#include "Engine/DirectX/RTV/RenderTarget.h"
#include "Engine/Utilities/Shader.h"
#include "Blend.h"
// Json
#include "Engine/Lib/Json/IJsonConverter.h"



class Pipeline {
public:

	struct PipelineParameter : public IJsonConverter {
		std::string vs;			// vsのシェーダー名
		std::string ps;			// psのシェーダー名
		std::string blendMode;	// blendModeの名前
		bool culling;			// カリングを行うか
		bool depth;				// 深度を書くか
		bool colorCorrection;	// 色の補正を行うか

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("vs", vs)
				.Add("ps", ps)
				.Add("blendMode", blendMode)
				.Add("culling", culling)
				.Add("depth", depth)
				.Add("colorCorrection", colorCorrection)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "vs", vs);
			fromJson(jsonData, "ps", ps);
			fromJson(jsonData, "blendMode", blendMode);
			fromJson(jsonData, "culling", culling);
			fromJson(jsonData, "depth", depth);
			fromJson(jsonData, "colorCorrection", colorCorrection);
		}
	};

	struct BindingKey {
		D3D_SHADER_INPUT_TYPE type;
		UINT bindPoint;
		UINT space;
		D3D12_SHADER_VISIBILITY visibility;
		bool operator<(const BindingKey& rhs) const {
			return std::tie(type, bindPoint, space, visibility) <
				std::tie(rhs.type, rhs.bindPoint, rhs.space, rhs.visibility);
		}
	};

public:

	Pipeline();
	~Pipeline();

	/*void Initialize(ID3D12Device* device, DirectXCompiler* dxCompiler, 
					const Shader::ShaderData& shaderData, const RootSignatureType& rootSignatureType,
					const std::vector<D3D12_INPUT_ELEMENT_DESC>& desc, const Blend::BlendMode& blendMode,
					bool isCulling, bool isDepth, bool isSRGB
	);*/

	void Init(ID3D12Device* device, DirectXCompiler* dxCompiler, const json& jsonData);

	void Draw(ID3D12GraphicsCommandList* commandList);

	void Finalize();

public:

	/// <summary>
	/// inputLayoutの生成
	/// </summary>
	/// <param name="elementDesc">要素の配列</param>
	/// <returns></returns>
	D3D12_INPUT_LAYOUT_DESC CreateInputLayout(const std::vector<D3D12_INPUT_ELEMENT_DESC>& elementDesc);

	/// <summary>
	/// Shaderをcompileする
	/// </summary>
	void ShaderCompile();

	/// <summary>
	/// RasterizerStateの設定
	/// </summary>
	D3D12_RASTERIZER_DESC SetRasterizerState(bool isCulling);

	/// <summary>
	/// DepthStencilStateの設定
	/// </summary>
	/// <returns></returns>
	D3D12_DEPTH_STENCIL_DESC SetDepthStencilState(bool isDepth);

	/// <summary>
	/// PSOの生成
	/// </summary>
	void CreatePSO();


	std::vector<D3D12_INPUT_ELEMENT_DESC> CreateInputLayout();

	DXGI_FORMAT ReturnFormat(LPCSTR name);

	ComPtr<ID3D12RootSignature> CreateRootSignature();

public:

	const UINT GetRootSignatureIndex(const std::string& name) const { return rootSignatureIndexMap_.at(name); }

private:
	// rootSignature
	std::unique_ptr<RootSignature> rootSignature_ = nullptr;
	// inputLayout
	InputLayout inputLayout_;
	std::vector<D3D12_INPUT_ELEMENT_DESC> elementDescs = {};

	// Shader
	ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;

	ComPtr<ID3D12ShaderReflection> vsReflection_;
	ComPtr<ID3D12ShaderReflection> psReflection_;

	ComPtr<ID3D12RootSignature> rootSig_;

	// PSO
	ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_;

	// DXCで使う
	DirectXCompiler* dxCompiler_ = nullptr;

	// device
	ID3D12Device* device_ = nullptr;

	// Blend
	Blend blend_;

	PipelineParameter parameter_;

	std::map<std::string, UINT> rootSignatureIndexMap_;

	std::vector<std::vector<D3D12_DESCRIPTOR_RANGE>> descriptorRangeTables;

};