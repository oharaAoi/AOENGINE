#pragma once
#include <memory>
#include "Game/Scene/BaseScene.h"
// camera
#include "Game/Camera/DebugCamera.h"
#include "Game/Camera/Camera2d.h"
#include "Game/Camera/Camera3d.h"
#include "Game/WorldObject/Skybox.h"
//ui
#include "Game/UI/TitleUIs.h"
#include "Game/UI/FadePanel.h"
#include "Game/UI/Guide/GameModeGuide.h"

class TitleScene : 
	public BaseScene {
public:

	TitleScene();
	~TitleScene();

	void Finalize() override;
	void Init() override;
	void Update() override;

private:

	bool putButton_ = false;

	// ------------------- camera ------------------- //
	std::unique_ptr<Camera2d> camera2d_ = nullptr;
	std::unique_ptr<Camera3d> camera3d_ = nullptr;
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

	std::unique_ptr<GameModeGuide> gameModeGuide_;

	// ------------------- actor ------------------- //
	std::unique_ptr<TitleUIs> titleUIs_;
	std::unique_ptr<FadePanel> fadePanel_;
};

