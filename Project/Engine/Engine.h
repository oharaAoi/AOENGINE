#pragma once
#include <memory>
#include "Enviroment.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/DirectX/DirectXCommon/DirectXCommon.h"
#include "Engine/DirectX/DirectXDevice/DirectXDevice.h"
#include "Engine/DirectX/DirectXCommands/DirectXCommands.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
#include "Engine/DirectX/RTV/RenderTarget.h"
#include "Engine/DirectX/DirectXCompiler/DirectXCompiler.h"
#include "Engine/DirectX/Pipeline/PipelineGroup/GraphicsPipelines.h"
#include "Engine/DirectX/Pipeline/PipelineGroup/PrimitivePipeline.h"

#include "Engine/ComputeShader/ComputeShader.h"

#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/System/Input/Input.h"
#include "Engine/System/Audio/Audio.h"

#include "Engine/System/ParticleSystem/Tool/ParticleSystemEditor.h"

#include "Engine/Components/GameObject/Model.h"
#include "Engine/Components/Materials/PBRMaterial.h"
#include "Engine/Components/2d/Sprite.h"
#include "Engine/Components/WorldTransform.h"
#include "Engine/Components/ProcessedSceneFrame.h"
#include "Engine/Components/Rigging/Skinning.h"

#include "Engine/PostEffect/PostProcess.h"

#include "Engine/Geometry/GeometryFactory.h"

#include "Engine/Utilities/Shader.h"

#include "Render.h"

class EffectSystem;
class EffectSystemEditer;

// ======================================================== //
// 無名名前空間で内部リンゲージする
// ======================================================== //
namespace {
	int32_t kClientWidth_;
	int32_t kClientHeight_;

	Render* render_ = nullptr;

	WinApp* winApp_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

	EffectSystem* effectSystem_;

#ifdef _DEBUG
	ImGuiManager* imguiManager_ = nullptr;
#endif
	Input* input_ = nullptr;
	TextureManager* textureManager_ = nullptr;
	// dxDevice
	std::shared_ptr<DirectXDevice> dxDevice_ = nullptr;
	// descriptorHeap
	std::shared_ptr<DescriptorHeap> descriptorHeap_ = nullptr;
	// dxCommand
	std::unique_ptr<DirectXCommands> dxCommands_ = nullptr;
	// renderTarget
	std::shared_ptr<RenderTarget> renderTarget_ = nullptr;
	// dxCompiler
	std::shared_ptr<DirectXCompiler> dxCompiler_ = nullptr;
	// pipeline
	std::unique_ptr<GraphicsPipelines> graphicsPipelines_ = nullptr;
	std::shared_ptr<PrimitivePipeline> primitivePipeline_ = nullptr;
	// CS
	std::unique_ptr<ComputeShader> computeShader_ = nullptr;
	// audio
	std::unique_ptr<Audio> audio_ = nullptr;
	// shaderファイルのパスをまとめたクラス
	std::shared_ptr<Shader> shaders_;

	std::unique_ptr<PostProcess> postProcess_;

	// オフスクリーンレンダリングで生成したTextureを描画するクラス
	std::unique_ptr<ProcessedSceneFrame> processedSceneFrame_ = nullptr;

	std::unique_ptr<ParticleSystemEditor> particleSystemEditor_;

	EditerWindows* editerWindows_ = nullptr;

	bool isFullScreen_;

	bool isEffectEditer_;

	bool runGame_;

	bool openParticleEditer_ = false;	// 後で直す
}

class Engine {
public:

	Engine();
	~Engine();

	static void Initialize(uint32_t backBufferWidth, int32_t backBufferHeight);

	static void Finalize();

public:

	/// <summary>
	/// ループの判定
	/// </summary>
	/// <returns></returns>
	static bool ProcessMessage();

	/// <summary>
	/// フレームを開始する
	/// </summary>
	static void BeginFrame();

	/// <summary>
	/// フレームを終了する
	/// </summary>
	static void EndFrame();

	static void UpdateEditerWindow();
	
	static void RenderFrame();

	static void BlendFinalTexture();


public:

	/////////////////////////////////////////////////////////////////////////////////////////////
	// 生成系
	/////////////////////////////////////////////////////////////////////////////////////////////
	// スプライトのポインタを作成
	static std::unique_ptr<Sprite> CreateSprite(const std::string& fileName);
	// モデルのポインタを作成
	static std::unique_ptr<Model> CreateModel(const std::string& directoryPath, const std::string& filePath);
	// ワールドトランスフォームを作成
	static std::unique_ptr<WorldTransform> CreateWorldTransform();
	// Skinningの作成
	static std::unique_ptr<Skinning> CreateSkinning(Skeleton* skeleton, Model* model, uint32_t index);

	static std::unique_ptr<Material> CreateMaterial(const Model::ModelMaterialData data);
	static std::unique_ptr<PBRMaterial> CreatePBRMaterial(const Model::ModelMaterialData data);


	/////////////////////////////////////////////////////////////////////////////////////////////
	// 描画系
	/////////////////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// CSを行う
	/// </summary>
	static void RunCS();

	/////////////////////////////////////////////////////////////////////////////////////////////
	// 設定系
	/////////////////////////////////////////////////////////////////////////////////////////////

	static void SetPSOObj(Object3dPSO kind);
	static void SetPSOSprite(SpritePSO kind);
	static void SetPSOProcessed(ProcessedScenePSO kind);
	static void SetPSOPrimitive();

	/// <summary>
	/// パイプラインの設定
	/// </summary>
	/// <param name="kind">設定するパイプライン</param>
	static void SetCsPipeline(const CsPipelineType& kind);

	static void SetSkinning(Skinning* skinning, Mesh* mesh);

	/// <summary>
	/// 深度バッファをリセットする
	/// </summary>
	static void ClearDepth();

	/// <summary>
	/// CSをResetする
	/// </summary>
	static void ResetComputeShader();

	/////////////////////////////////////////////////////////////////////////////////////////////
	// sound系
	/////////////////////////////////////////////////////////////////////////////////////////////

	static SoundData LoadAudio(const std::string filePath);

	static AudioData LoadAudio(const SoundData& loadAudioData);

	/// <summary>
	/// 音を鳴らす関数
	/// </summary>
	/// <param name="soundData">音声データをまとめた構造体</param>
	static void PlayAudio(const AudioData& soundData, bool isLoop, float volume, bool checkPlaying = false);

	/// <summary>
	/// BGMを一時停止させる関数
	/// </summary>
	/// <param name="soundData">音声データをまとめた構造体</param>
	static void Pause(const AudioData& soundData);

	/// <summary>
	/// BGMを再生する関数
	/// </summary>
	/// <param name="soundData">音声データをまとめた構造体</param>
	static void ReStart(const AudioData& soundData);

	/// <summary>
	/// 音の再生を止める関数
	/// </summary>
	/// <param name="soundData">音声データをまとめた構造体</param>
	static void Stop(const AudioData& soundData);

	/// <summary>
	/// 音量を調整する
	/// </summary>
	/// <param name="soundData"></param>
	/// <param name="volume"></param>
	static void SetVolume(const AudioData& soundData, float volume);

	static bool GetIsPlaying(const AudioData& soundData);

	/// <summary>
	/// 一度だけ再生し、他でも鳴って欲しい物
	/// </summary>
	/// <param name="fileName"></param>
	/// <param name="volume"></param>
	static void SingleShotPlay(const SoundData& loadAudioData, float volume);

	/////////////////////////////////////////////////////////////////////////////////////////////
	// 苦肉の策
	/////////////////////////////////////////////////////////////////////////////////////////////

	static ID3D12Device* GetDevice();
	static ID3D12GraphicsCommandList* GetCommandList();
	static DescriptorHeap* GetDxHeap();

	static bool GetIsOpenEffectEditer();

	static bool GetRunGame();

	static GraphicsPipelines* GetGraphicsPipelines();

	static PrimitivePipeline* GetPrimitivePipeline();

private:

};

