#pragma once
#include "Engine/DirectX/Pipeline/PipelineGroup/IPipelineGroup.h"

enum class Object3dPSO {
	Normal,
	NormalCut,
	NormalEnviroment,
	Add,
	TextureLess,
	Particle,
	SubParticle,
	PBR,
	Skybox,
	TextureBlendNone,
	TextureBlendNormal,
	TextureBlendAdd,
};

/// <summary>
/// 3dObjectを描画するためのPipelineをまとめたクラス
/// </summary>
class Object3dPipelines :
	public IPipelineGroup {
public:

	Object3dPipelines() = default;
	~Object3dPipelines() override;

	void Init(ID3D12Device* device, DirectXCompiler* dxCompiler) override;

	void SetPipeline(ID3D12GraphicsCommandList* commandList, const std::string& typeName);

	void AddPipeline(const std::string& fileName, json jsonData);

private:



};

