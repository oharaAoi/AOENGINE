#pragma once
#include <unordered_map>
#include "Engine/DirectX/Pipeline/Pipeline.h"

/// <summary>
/// Piplelineをまとめたクラスの原型となるもの
/// </summary>
class IPipelineGroup {
public: // コンストラクタ

	IPipelineGroup() = default;
	virtual ~IPipelineGroup() = default;

public:

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="device">: デバイス</param>
	/// <param name="dxCompiler">: コンパイラ</param>
	virtual void Init(ID3D12Device* _device, DirectXCompiler* _dxCompiler) = 0;

	/// <summary>
	/// 最後に使用したパイプラインを返す
	/// </summary>
	/// <returns></returns>
	Pipeline* GetLastUsedPipeline() const { return lastUsePipeline_; };

protected:

	std::unordered_map<std::string, std::unique_ptr<Pipeline>> pipelineMap_;

	ID3D12Device* device_;
	DirectXCompiler* dxCompiler_;

	Pipeline* lastUsePipeline_;

};

