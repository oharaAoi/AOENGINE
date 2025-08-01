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

	void Init() override;
	void Finalize() override;
	void Update() override;
	void Draw() override;

	void Debug_Gui();

private:

	GameTimer gameTimer_ = GameTimer(60);

	std::unique_ptr<SceneManager> sceneManger_;

	bool isReset_;

};

