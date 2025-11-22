#pragma once
#include <memory>
#include <functional>
#include <utility>
#include "Engine/System/Editer/Window/GameObjectWindow.h"
#include "Engine/System/ParticleSystem/Tool/ParticleSystemEditor.h"
#include "Engine/Module/Components/ProcessedSceneFrame.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/DirectX/RTV/RenderTarget.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/Module/Components/2d/Canvas2d.h"
#include "Engine/System/Editer/Tool/ManipulateTool.h"
#include "Engine/System/ShaderGraph/Editor/ShaderGraphEditor.h"

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

	static void AddObjectWindow([[maybe_unused]] AttributeGui* attribute, [[maybe_unused]] const std::string& label);

	template<typename Ptr, typename Func>
	void SetFocusedInspector(Ptr ptr, Func&& func) {
		auto ptrHandle = ptr;

		auto& update = GetWindowUpdate();
		update = [ptrHandle, f = std::forward<Func>(func)]() mutable {
			f();   // ★ ptrHandle を渡さない
			};
	}

#ifdef _DEBUG

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="device"></param>
	/// <param name="commandList"></param>
	/// <param name="renderTarget"></param>
	/// <param name="descriptorHeaps"></param>
	void Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, RenderTarget* renderTarget, DescriptorHeap* descriptorHeaps);

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

private:

	/// <summary>
	/// ゲーム画面の描画
	/// </summary>
	void GameWindow();

	/// <summary>
	/// Debug機能を描画するWindow
	/// </summary>
	void DebugItemWindow();

	bool PushStyleColor(bool _flag, const Vector4& color);

	void PopStyleColor(bool _flag);

#endif 

public:

	void SetProcessedSceneFrame(ProcessedSceneFrame* sceneFrame) { processedSceneFrame_ = sceneFrame; }

	void SetRenderTarget(RenderTarget* _renderTarget) { renderTarget_ = _renderTarget; }

	void Reset();

	bool GetSceneReset() const { return sceneReset_; }

	bool GetColliderDraw() const { return colliderDraw_; };

	bool GetGridDraw() const { return gridDraw_; }

	void SetSceneRenderer(SceneRenderer* _renderer) { sceneRenderer_ = _renderer; }
	void SetCanvas2d(Canvas2d* _canvas) { canvas2d_ = _canvas; }

	std::function<void()>& GetWindowUpdate() { return windowUpdate_; }

private:

	GameObjectWindow* GetObjectWindow() { return gameObjectWindow_.get(); }

private:

	// ----------------------
	// ↓ 編集項目
	// ----------------------
	std::unique_ptr<GameObjectWindow> gameObjectWindow_;

	std::unique_ptr<ParticleSystemEditor> particleSystemEditor_;

	std::unique_ptr<ManipulateTool> manipulateTool_;

	std::unique_ptr<ShaderGraphEditor> shaderGraphEditor_;

	// ----------------------
	// ↓ 他ポインタ
	// ----------------------
	ProcessedSceneFrame* processedSceneFrame_ = nullptr;
	RenderTarget* renderTarget_;
	SceneRenderer* sceneRenderer_;
	Canvas2d* canvas2d_;

	std::function<void()> windowUpdate_;

	// editorで使用するフラグ
	bool sceneReset_;
	bool colliderDraw_;
	bool gridDraw_;
	bool isSkip_;
};