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
public: // コンストラクタ

	Object3dPipelines() = default;
	~Object3dPipelines() override;

public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="device"></param>
	/// <param name="dxCompiler"></param>
	void Init(ID3D12Device* device, DirectXCompiler* dxCompiler) override;

	/// <summary>
	/// パイプラインの設定
	/// </summary>
	/// <param name="commandList">: コマンドリスト</param>
	/// <param name="typeName">: パイプラインのタイプ</param>
	void SetPipeline(ID3D12GraphicsCommandList* commandList, const std::string& typeName);

	/// <summary>
	/// パイプラインの追加
	/// </summary>
	/// <param name="fileName">: ファイル名</param>
	/// <param name="jsonData">: jsonデータ</param>
	void AddPipeline(const std::string& fileName, json jsonData);

private:



};

