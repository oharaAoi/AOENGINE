#include "RenderingCommands.h"
#include "Engine/Module/Components/Materials/BaseMaterial.h"

void RenderingCommands::DrawModel(ID3D12GraphicsCommandList* _cmdList, AOENGINE::Model* _model, const AOENGINE::Pipeline* _pso, const AOENGINE::WorldTransform* _worldTransform, const AOENGINE::ViewProjection* _viewProjection, const std::vector<AOENGINE::BaseMaterial*>& _materialSlots) {
	UINT index = 0;
	auto meshArray = _model->GetMeshArray();

	for (uint32_t oi = 0; oi < meshArray.size(); oi++) {
		_cmdList->IASetVertexBuffers(0, 1, &meshArray[oi]->GetVBV());
		_cmdList->IASetIndexBuffer(&meshArray[oi]->GetIBV());
		index = _pso->GetRootSignatureIndex("gWorldTransformMatrix");
		_worldTransform->BindCommandList(_cmdList, index);
		index = _pso->GetRootSignatureIndex("gViewProjectionMatrix");
		_viewProjection->BindCommandList(_cmdList, index);
		index = _pso->GetRootSignatureIndex("gViewProjectionMatrixPrev");
		_viewProjection->BindCommandListPrev(_cmdList, index);

		for (uint32_t subMeshIndex = 0; subMeshIndex < meshArray[oi]->GetSubMeshCount(); ++subMeshIndex) {
			const AOENGINE::SubMesh& subMesh = meshArray[oi]->GetSubMesh(subMeshIndex);
			if (subMesh.materialSlot >= _materialSlots.size() || !_materialSlots[subMesh.materialSlot]) { continue; }
			AOENGINE::BaseMaterial* material = _materialSlots[subMesh.materialSlot];
			material->BindCommand(_cmdList, _pso);
			_cmdList->IASetPrimitiveTopology(subMesh.topology);
			_cmdList->DrawIndexedInstanced(subMesh.indexCount, 1, subMesh.firstIndex, subMesh.baseVertex, 0);
		}
	}
}

void RenderingCommands::DrawSkinningModel(ID3D12GraphicsCommandList* _cmdList, AOENGINE::Model* _model, const AOENGINE::Pipeline* _pso,
										  const AOENGINE::WorldTransform* _worldTransform, const AOENGINE::ViewProjection* _viewProjection,
										  const std::vector<AOENGINE::BaseMaterial*>& _materialSlots,
										  const std::vector<std::unique_ptr<AOENGINE::Skinning>>& _skinningArray) {
	UINT index = 0;
	auto meshArray = _model->GetMeshArray();

	for (uint32_t oi = 0; oi < meshArray.size(); oi++) {
		_cmdList->IASetVertexBuffers(0, 1, &_skinningArray[oi]->GetVBV());
		_cmdList->IASetIndexBuffer(&meshArray[oi]->GetIBV());
		index = _pso->GetRootSignatureIndex("gWorldTransformMatrix");
		_worldTransform->BindCommandList(_cmdList, index);
		index = _pso->GetRootSignatureIndex("gViewProjectionMatrix");
		_viewProjection->BindCommandList(_cmdList, index);
		index = _pso->GetRootSignatureIndex("gViewProjectionMatrixPrev");
		_viewProjection->BindCommandListPrev(_cmdList, index);

		for (uint32_t subMeshIndex = 0; subMeshIndex < meshArray[oi]->GetSubMeshCount(); ++subMeshIndex) {
			const AOENGINE::SubMesh& subMesh = meshArray[oi]->GetSubMesh(subMeshIndex);
			if (subMesh.materialSlot >= _materialSlots.size() || !_materialSlots[subMesh.materialSlot]) { continue; }
			AOENGINE::BaseMaterial* material = _materialSlots[subMesh.materialSlot];
			material->BindCommand(_cmdList, _pso);
			_cmdList->IASetPrimitiveTopology(subMesh.topology);
			_cmdList->DrawIndexedInstanced(subMesh.indexCount, 1, subMesh.firstIndex, subMesh.baseVertex, 0);
		}
	}
}


