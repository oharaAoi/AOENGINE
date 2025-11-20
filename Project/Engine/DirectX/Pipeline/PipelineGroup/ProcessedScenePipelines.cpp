#include "ProcessedScenePipelines.h"

ProcessedScenePipelines::~ProcessedScenePipelines() {
	pipelineMap_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ProcessedScenePipelines::Init(ID3D12Device* _device, DirectXCompiler* _dxCompiler) {
	assert(_device);
	assert(_dxCompiler);

	device_ = _device;
	dxCompiler_ = _dxCompiler;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ パイプラインの生成
///////////////////////////////////////////////////////////////////////////////////////////////

void ProcessedScenePipelines::SetPipeline(ID3D12GraphicsCommandList* _commandList, const std::string& _typeName) {
	pipelineMap_[_typeName]->BindCommand(_commandList);
	_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	lastUsePipeline_ = pipelineMap_[_typeName].get();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ パイプラインの追加
///////////////////////////////////////////////////////////////////////////////////////////////

void ProcessedScenePipelines::AddPipeline(const std::string& _fileName, json _jsonData) {
	pipelineMap_[_fileName] = std::make_unique<Pipeline>();
	pipelineMap_[_fileName]->Init(device_, dxCompiler_, _jsonData);
}
