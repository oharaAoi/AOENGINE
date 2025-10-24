#pragma once
#include <memory>
#include "Engine/Core/AoiFramework.h"
#include "Engine/System/Manager/SceneManager.h"

/// <summary>
/// ゲーム全体
/// </summary>
class GameCore : public AoiFramework {
public:

	GameCore(uint32_t _kWindowWidth, uint32_t _kWindowHeight, const char* _windowTitle)
		: AoiFramework(_kWindowWidth, _kWindowHeight, _windowTitle) {}
	~GameCore();

public:

	// 初期化
	void Init() override;
	// 終了
	void Finalize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	// frame開始処理
	void BeginFrame() override;
	// frame終了処理
	void EndFrame() override;

private:

	GameTimer gameTimer_ = GameTimer(60);

	std::unique_ptr<SceneManager> sceneManger_;

	bool isReset_;

};

