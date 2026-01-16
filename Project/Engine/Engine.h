#pragma once
#include <memory>
#include "Engine/Core/GraphicsContext.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Audio/Audio.h"
#include "Engine/Module/Components/GameObject/Model.h"
#include "Engine/Module/Components/2d/Canvas2d.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Rigging/Skinning.h"
#include "Engine/Module/PostEffect/PostProcess.h"

class EffectSystem;
class EffectSystemEditer;

/// <summary>
/// GameEngine
/// </summary>
class Engine {
public:

	Engine();
	~Engine();

	static void Initialize(uint32_t _backBufferWidth, uint32_t _backBufferHeight, const char* _windowTitle);

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

	/// <summary>
	/// Frameを描画する
	/// </summary>
	static void RenderFrame();

	/// <summary>
	/// Textureの合成
	/// </summary>
	/// <param name="renderTargetType"></param>
	static void BlendFinalRender(RenderTargetType renderTargetType);

	/// <summary>
	/// world上の座標をGame画面に合わせた座標で返す
	/// </summary>
	/// <param name="_worldPos"></param>
	/// <param name="_outScreenPos"></param>
	/// <returns></returns>
	static bool WorldToGameImagePos(const Math::Vector3& _worldPos, ImVec2& _outScreenPos);

private:

	/// <summary>
	/// bufferのサイズを作り変える
	/// </summary>
	static void PendingResize();

public:

	/////////////////////////////////////////////////////////////////////////////////////////////
	// 生成系
	/////////////////////////////////////////////////////////////////////////////////////////////
	// モデルのポインタを作成
	static std::unique_ptr<AOENGINE::Model> CreateModel(const std::string& directoryPath, const std::string& filePath);
	// ワールドトランスフォームを作成
	static std::unique_ptr<AOENGINE::WorldTransform> CreateWorldTransform();
	// Skinningの作成
	static std::unique_ptr<AOENGINE::Skinning> CreateSkinning(AOENGINE::Skeleton* skeleton, AOENGINE::Model* model, uint32_t index);

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Pipelineの設定
	/////////////////////////////////////////////////////////////////////////////////////////////

	static AOENGINE::Pipeline* SetPipeline(PSOType type, const std::string& typeName);
	static AOENGINE::Pipeline* SetPipelineCS(const std::string& jsonFile);

	static AOENGINE::Pipeline* GetLastUsedPipeline();
	static AOENGINE::Pipeline* GetLastUsedPipelineCS();

	static void SetSkinning(AOENGINE::Skinning* skinning);

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Getter/Setter
	/////////////////////////////////////////////////////////////////////////////////////////////

	static AOENGINE::Audio* GetAudio();

	static AOENGINE::Canvas2d* GetCanvas2d();

	static AOENGINE::PostProcess* GetPostProcess();
};