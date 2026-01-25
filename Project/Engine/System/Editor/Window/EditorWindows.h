#pragma once
#include <memory>
#include <string>
#include "Engine/System/Editor/Window/GameObjectWindow.h"
#include "Engine/System/ParticleSystem/Tool/ParticleSystemEditor.h"
#include "Engine/System/Editor/Tool/ManipulateTool.h"
#include "Engine/System/Editor/Window/PackagesWindow.h"
#include "Engine/System/Manager/SceneManager.h"
#include "Engine/System/ShaderGraph/Editor/ShaderGraphEditor.h"
#include "Engine/Module/Components/ProcessedSceneFrame.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/Attribute/IEditorWindow.h"
#include "Engine/Module/Components/2d/Canvas2d.h"
#include "Engine/DirectX/RTV/RenderTarget.h"
#include "Engine/Render/SceneRenderer.h"

namespace AOENGINE {

/// <summary>
/// 編集画面の表示を管理する
/// </summary>
class EditorWindows {
public:

	EditorWindows() = default;
	~EditorWindows();

	EditorWindows(const EditorWindows&) = delete;
	const EditorWindows& operator=(const EditorWindows&) = delete;

	static EditorWindows* GetInstance();

public:

	// 終了
	void Finalize();

	static void AddObjectWindow([[maybe_unused]] AOENGINE::AttributeGui* attribute, [[maybe_unused]] const std::string& label);

#ifdef _DEBUG

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="device"></param>
	/// <param name="commandList"></param>
	/// <param name="renderTarget"></param>
	/// <param name="descriptorHeaps"></param>
	void Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, AOENGINE::RenderTarget* renderTarget, AOENGINE::DescriptorHeap* descriptorHeaps);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 開始処理
	/// </summary>
	void Begin();

	/// <summary>
	/// 終了処理
	/// </summary>
	void End();

	/// <summary>
	/// バッファをクリアする
	/// </summary>
	void ClearBuffer();

	/// <summary>
	/// バッファをリサイズする
	/// </summary>
	void ResizeBuffer();

	/// <summary>
	/// Sceneをリセットする
	/// </summary>
	void SceneReset();

private:

	/// <summary>
	/// ゲーム画面の描画
	/// </summary>
	void GameWindow();

	/// <summary>
	/// Debug機能を描画するWindow
	/// </summary>
	void DebugItemWindow();

	/// <summary>
	/// ImGuiのカラーをセットする
	/// </summary>
	/// <param name="flag"></param>
	/// <param name="color"></param>
	/// <returns></returns>
	bool PushStyleColor(bool flag, const Math::Vector4& color);

	/// <summary>
	/// ImGuiのカラーを解放する
	/// </summary>
	/// <param name="flag"></param>
	void PopStyleColor(bool flag);

#endif 

public:

	void SetProcessedSceneFrame(AOENGINE::ProcessedSceneFrame* sceneFrame) { processedSceneFrame_ = sceneFrame; }

	void SetRenderTarget(AOENGINE::RenderTarget* renderTarget) { renderTarget_ = renderTarget; }


	bool GetSceneReset() const { return sceneReset_; }

	bool GetColliderDraw() const { return colliderDraw_; };

	bool GetGridDraw() const { return gridDraw_; }

	void SetSceneRenderer(AOENGINE::SceneRenderer* renderer) { sceneRenderer_ = renderer; }
	void SetCanvas2d(AOENGINE::Canvas2d* canvas) { canvas2d_ = canvas; }
	void SetSelectWindow(IEditorWindow* ptr) { pSelectWindow_ = ptr; };
	void SetSceneManager(AOENGINE::SceneManager* sceneManager) { pSceneManager_ = sceneManager; }

private:

	GameObjectWindow* GetObjectWindow() { return gameObjectWindow_.get(); }

private:

	// ----------------------
	// ↓ 編集項目
	// ----------------------
	std::unique_ptr<AOENGINE::GameObjectWindow> gameObjectWindow_;

	std::unique_ptr<AOENGINE::ParticleSystemEditor> particleSystemEditor_;

	std::unique_ptr<AOENGINE::ManipulateTool> manipulateTool_;

	std::unique_ptr<AOENGINE::ShaderGraphEditor> shaderGraphEditor_;

	std::unique_ptr<AOENGINE::PackagesWindow> packagesWindow_;

	// ----------------------
	// ↓ 他ポインタ
	// ----------------------
	AOENGINE::ProcessedSceneFrame* processedSceneFrame_ = nullptr;
	AOENGINE::RenderTarget* renderTarget_;
	AOENGINE::SceneRenderer* sceneRenderer_;
	AOENGINE::Canvas2d* canvas2d_;
	AOENGINE::SceneManager* pSceneManager_;

	AOENGINE::IEditorWindow* pSelectWindow_;

	// editorで使用するフラグ
	bool sceneReset_;
	bool colliderDraw_;
	bool gridDraw_;
	bool isSkip_;
	bool isFullScreen_;
};
}