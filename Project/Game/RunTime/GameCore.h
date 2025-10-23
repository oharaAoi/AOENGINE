#pragma once
#include <memory>
#include "Engine/Core/AoiFramework.h"
#include "Engine/System/Manager/SceneManager.h"

/// <summary>
/// ゲーム全体
/// </summary>
class GameCore : public AoiFramework {
public:

	GameCore();
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
	// 編集
	void Debug_Gui();

private:

	GameTimer gameTimer_ = GameTimer(60);

	std::unique_ptr<SceneManager> sceneManger_;

	bool isReset_;

};

