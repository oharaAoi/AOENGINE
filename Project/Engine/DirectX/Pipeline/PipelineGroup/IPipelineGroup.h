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

	virtual void Init(ID3D12Device* device, DirectXCompiler* dxCompiler, Shader* shaders) = 0;

protected:

	// inputLayout
	InputLayout inputLayout_;

};

