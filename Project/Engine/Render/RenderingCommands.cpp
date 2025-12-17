#include "RenderingCommands.h"
#include "Engine/Module/Components/Materials/BaseMaterial.h"

void RenderingCommands::DrawModel(ID3D12GraphicsCommandList* _cmdList, AOENGINE::Model* _model, const AOENGINE::Pipeline* _pso, const AOENGINE::WorldTransform* _worldTransform, const AOENGINE::ViewProjection* _viewProjection, const std::unordered_map<std::string, std::unique_ptr<AOENGINE::BaseMaterial>>& _materials) {
	UINT index = 0;
	auto meshArray = _model->GetMeshArray();

	for (uint32_t oi = 0; oi < meshArray.size(); oi++) {
		std::string useMaterial = meshArray[oi]->GetUseMaterial();
		AOENGINE::BaseMaterial* material = _materials.at(useMaterial).get();
		_cmdList->IASetVertexBuffers(0, 1, &meshArray[oi]->GetVBV());
		_cmdList->IASetIndexBuffer(&meshArray[oi]->GetIBV());
		index = _pso->GetRootSignatureIndex("gWorldTransformMatrix");
		_worldTransform->BindCommandList(_cmdList, index);
		index = _pso->GetRootSignatureIndex("gViewProjectionMatrix");
		_viewProjection->BindCommandList(_cmdList, index);
		index = _pso->GetRootSignatureIndex("gViewProjectionMatrixPrev");
		_viewProjection->BindCommandListPrev(_cmdList, index);

		// マテリアルの設定
		material->BindCommand(_cmdList, _pso);

		_cmdList->DrawIndexedInstanced(meshArray[oi]->GetIndexNum(), 1, 0, 0, 0);
	}
}

void RenderingCommands::DrawSkinningModel(ID3D12GraphicsCommandList* _cmdList, AOENGINE::Model* _model, const AOENGINE::Pipeline* _pso,
										  const AOENGINE::WorldTransform* _worldTransform, const AOENGINE::ViewProjection* _viewProjection,
										  const std::unordered_map<std::string, std::unique_ptr<AOENGINE::BaseMaterial>>& _materials,
										  const std::vector<std::unique_ptr<AOENGINE::Skinning>>& _skinningArray) {
	UINT index = 0;
	auto meshArray = _model->GetMeshArray();

	for (uint32_t oi = 0; oi < meshArray.size(); oi++) {
		std::string useMaterial = meshArray[oi]->GetUseMaterial();
		AOENGINE::BaseMaterial* material = _materials.at(useMaterial).get();
		_cmdList->IASetVertexBuffers(0, 1, &_skinningArray[oi]->GetVBV());
		_cmdList->IASetIndexBuffer(&meshArray[oi]->GetIBV());
		index = _pso->GetRootSignatureIndex("gWorldTransformMatrix");
		_worldTransform->BindCommandList(_cmdList, index);
		index = _pso->GetRootSignatureIndex("gViewProjectionMatrix");
		_viewProjection->BindCommandList(_cmdList, index);

		// マテリアルの設定
		material->BindCommand(_cmdList, _pso);

		_cmdList->DrawIndexedInstanced(meshArray[oi]->GetIndexNum(), 1, 0, 0, 0);
	}
}


