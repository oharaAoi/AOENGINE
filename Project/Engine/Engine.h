#pragma once
#include <memory>
#include "Enviroment.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/Core/GraphicsContext.h"

#include "Engine/DirectX/Pipeline/PipelineGroup/ComputeShaderPipelines.h"
#include "Engine/Module/ComputeShader/BlendTexture.h"

#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/System/Input/Input.h"
#include "Engine/System/Audio/Audio.h"

#include "Engine/Module/Components/GameObject/Model.h"
#include "Engine/Module/Components/Materials/PBRMaterial.h"
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/2d/Canvas2d.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/ProcessedSceneFrame.h"
#include "Engine/Module/Components/Rigging/Skinning.h"

#include "Engine/Module/PostEffect/PostProcess.h"

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

#ifdef _DEBUG
	ImGuiManager* imguiManager_ = nullptr;
#endif
	Input* input_ = nullptr;
	TextureManager* textureManager_ = nullptr;

	GraphicsContext* graphicsCxt_ = nullptr;

	ID3D12Device* dxDevice_ = nullptr;
	ID3D12GraphicsCommandList* dxCmdList_ = nullptr;
	DescriptorHeap* dxHeap_ = nullptr;

	DirectXCommon* dxCommon_ = nullptr;

	GraphicsPipelines* graphicsPipeline_ = nullptr;
	PrimitivePipeline* primitivePipeline_ = nullptr;

	RenderTarget* renderTarget_ = nullptr;

	// CS
	std::unique_ptr<ComputeShaderPipelines> computeShaderPipelines_ = nullptr;
	std::unique_ptr<BlendTexture> blendTexture_ = nullptr;
	// audio
	std::unique_ptr<Audio> audio_ = nullptr;
	// shaderファイルのパスをまとめたクラス
	std::shared_ptr<Shader> shaders_;

	std::unique_ptr<PostProcess> postProcess_;

	std::unique_ptr<Canvas2d> canvas2d_;

	// オフスクリーンレンダリングで生成したTextureを描画するクラス
	std::unique_ptr<ProcessedSceneFrame> processedSceneFrame_ = nullptr;

	EditorWindows* editorWindows_ = nullptr;

	bool isFullScreen_;

	bool runGame_;

	Pipeline* lastUsedPipeline_;
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

	static void RenderFrame();

	static void BlendFinalTexture(RenderTargetType renderTargetType);

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

	static void SetPSOPrimitive();

	static void SetPipeline(PSOType type, const std::string& typeName);

	static Pipeline* GetLastUsedPipeline();
	static Pipeline* GetLastUsedPipelineCS();

	static void SetSkinning(Skinning* skinning);

	static void SetPipelineCS(const std::string& jsonFile);

	/// <summary>
	/// 深度バッファをリセットする
	/// </summary>
	static void ClearDepth();

	static Canvas2d* GetCanvas2d();

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

	static bool GetRunGame();

	static PostProcess* GetPostProcess();

	static void SetRenderTarget();

private:

};

