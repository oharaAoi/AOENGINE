#pragma once
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Pipeline/Pipeline.h"
#include "Engine/Module/Components/ViewProjection.h"
#include "Engine/Module/Components/GameObject/Model.h"
#include "Engine/Module/Components/Materials/BaseMaterial.h"
#include "Engine/Module/Components/Rigging/Skinning.h"

/// <summary>
/// レンダリング用のコマンドをまとめたクラス
/// </summary>
namespace RenderingCommands {

/// <summary>
/// 通常のモデルの描画
/// </summary>
/// <param name="_cmdList"></param>
/// <param name="_model"></param>
/// <param name="_pso"></param>
/// <param name="_worldTransform"></param>
/// <param name="_viewProjection"></param>
/// <param name="_materials"></param>
void DrawModel(ID3D12GraphicsCommandList* _cmdList, AOENGINE::Model* _model, const AOENGINE::Pipeline* _pso, 
			   const AOENGINE::WorldTransform* _worldTransform,
			   const AOENGINE::ViewProjection* _viewProjection,
			   const std::unordered_map<std::string, std::unique_ptr<AOENGINE::BaseMaterial>>& _materials);

/// <summary>
/// Skinningを行っているモデルの描画
/// </summary>
/// <param name="_cmdList"></param>
/// <param name="_model"></param>
/// <param name="_pso"></param>
/// <param name="_worldTransform"></param>
/// <param name="_viewProjection"></param>
/// <param name="_materials"></param>
/// <param name="_skinningArray"></param>
void DrawSkinningModel(ID3D12GraphicsCommandList* _cmdList, AOENGINE::Model* _model, const AOENGINE::Pipeline* _pso,
					   const AOENGINE::WorldTransform* _worldTransform,
					   const AOENGINE::ViewProjection* _viewProjection,
					   const std::unordered_map<std::string, std::unique_ptr<AOENGINE::BaseMaterial>>& _materials,
					   const std::vector<std::unique_ptr<AOENGINE::Skinning>>& _skinningArray);

}
