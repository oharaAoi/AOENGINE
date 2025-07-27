#pragma once
// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <set>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
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
		std::string cs;			// csのシェーダー名
		std::string blendMode;	// blendModeの名前
		bool culling;			// カリングを行うか
		bool depth;				// 深度を書くか
		std::vector<std::string> rtvFormats;	// 色の補正を行うか

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("vs", vs)
				.Add("ps", ps)
				.Add("cs", cs)
				.Add("blendMode", blendMode)
				.Add("culling", culling)
				.Add("depth", depth)
				.Add("rtvFormat", rtvFormats)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			const auto& pipeline = jsonData.at("Pipeline");
			fromJson(jsonData, "vs", vs);
			fromJson(jsonData, "ps", ps);
			fromJson(jsonData, "cs", cs);
			fromJson(jsonData, "blendMode", blendMode);
			fromJson(jsonData, "culling", culling);
			fromJson(jsonData, "depth", depth);
			fromJson(pipeline, "rtvFormat", rtvFormats);
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

	void Init(ID3D12Device* device, DirectXCompiler* dxCompiler, const json& jsonData);

	void Draw(ID3D12GraphicsCommandList* commandList);

	void SetComputeState(ID3D12GraphicsCommandList* commandList);

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

private:

	void SamplerOverrides();

	D3D12_STATIC_SAMPLER_DESC MakeStaticSampler(D3D12_FILTER filter, D3D12_TEXTURE_ADDRESS_MODE addr = D3D12_TEXTURE_ADDRESS_MODE_WRAP, UINT maxAniso = 16);

public:

	const UINT GetRootSignatureIndex(const std::string& name) const;

private:
	
	// inputLayout
	InputLayout inputLayout_;
	std::vector<D3D12_INPUT_ELEMENT_DESC> elementDescs = {};

	// Shader
	ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;

	ComPtr<ID3D12ShaderReflection> vsReflection_;
	ComPtr<ID3D12ShaderReflection> psReflection_;
	ComPtr<ID3D12ShaderReflection> csReflection_;

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

	// samplerMap
	std::unordered_map<std::string, D3D12_STATIC_SAMPLER_DESC> samplerOverrides_;


};