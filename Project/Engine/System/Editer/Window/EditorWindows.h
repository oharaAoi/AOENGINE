#pragma once
#include <memory>
#include <functional>
#include "Engine/System/Editer/Window/GameObjectWindow.h"
#include "Engine/System/ParticleSystem/Tool/ParticleSystemEditor.h"
#include "Engine/Module/Components/ProcessedSceneFrame.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/DirectX/RTV/RenderTarget.h"


class EditorWindows {
public:

	EditorWindows() = default;
	~EditorWindows();

	EditorWindows(const EditorWindows&) = delete;
	const EditorWindows& operator=(const EditorWindows&) = delete;

	static EditorWindows* GetInstance();

	void Finalize();

#ifdef _DEBUG
	void Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, RenderTarget* renderTarget, DescriptorHeap* descriptorHeaps);

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
	/// ゲーム画面の描画
	/// </summary>
	void GameWindow();

	/// <summary>
	/// ParticleEditorの描画
	/// </summary>
	void ParticleEditorWindow();
#endif 

public:

	static void AddObjectWindow(AttributeGui* attribute, const std::string& label);

	void SetProcessedSceneFrame(ProcessedSceneFrame* sceneFrame) { processedSceneFrame_ = sceneFrame; }

private:

	GameObjectWindow* GetObjectWindow() { return gameObjectWindow_.get(); }

private:

	std::unique_ptr<GameObjectWindow> gameObjectWindow_;

	std::unique_ptr<ParticleSystemEditor> particleSystemEditor_;

	ProcessedSceneFrame* processedSceneFrame_ = nullptr;

	std::function<void()> windowUpdate_;

	bool openParticleEditor_;

};

