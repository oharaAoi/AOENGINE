#pragma once
#include <memory>
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
#include "Engine/Module/Components/2d/Canvas2d.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/ProcessedSceneFrame.h"
#include "Engine/Module/Components/Rigging/Skinning.h"

#include "Engine/Module/PostEffect/PostProcess.h"

#include "Render.h"

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

	static void RenderFrame();

	static void BlendFinalTexture(RenderTargetType renderTargetType);

public:

	/////////////////////////////////////////////////////////////////////////////////////////////
	// 生成系
	/////////////////////////////////////////////////////////////////////////////////////////////
	// モデルのポインタを作成
	static std::unique_ptr<AOENGINE::Model> CreateModel(const std::string& directoryPath, const std::string& filePath);
	// ワールドトランスフォームを作成
	static std::unique_ptr<WorldTransform> CreateWorldTransform();
	// Skinningの作成
	static std::unique_ptr<AOENGINE::Skinning> CreateSkinning(AOENGINE::Skeleton* skeleton, AOENGINE::Model* model, uint32_t index);

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Pipelineの設定
	/////////////////////////////////////////////////////////////////////////////////////////////

	static AOENGINE::Pipeline* SetPipeline(PSOType type, const std::string& typeName);

	static AOENGINE::Pipeline* GetLastUsedPipeline();
	static AOENGINE::Pipeline* GetLastUsedPipelineCS();

	static void SetSkinning(AOENGINE::Skinning* skinning);

	static AOENGINE::Pipeline* SetPipelineCS(const std::string& jsonFile);

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Getter/Setter
	/////////////////////////////////////////////////////////////////////////////////////////////

	static Audio* GetAudio();

	static AOENGINE::Canvas2d* GetCanvas2d();

	static PostProcess* GetPostProcess();
};