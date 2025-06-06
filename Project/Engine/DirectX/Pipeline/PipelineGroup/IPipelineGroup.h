#pragma once
#include <unordered_map>
#include "Engine/DirectX/Pipeline/Pipeline.h"

/// <summary>
/// Piplelineをまとめたクラスの原型となるもの
/// </summary>
class IPipelineGroup {
public:

	IPipelineGroup() = default;
	virtual ~IPipelineGroup() = default;

	virtual void Init(ID3D12Device* device, DirectXCompiler* dxCompiler) = 0;

	Pipeline* GetLastUsedPipeline() const { return lastUsePipeline_; };

protected:

	std::unordered_map<std::string, std::unique_ptr<Pipeline>> pipelineMap_;

	ID3D12Device* device_;
	DirectXCompiler* dxCompiler_;

	Pipeline* lastUsePipeline_;

};

