#pragma once
#include <memory>
#include "Game/Scene/BaseScene.h"
// camera
#include "Game/Camera/FollowCamera.h"
#include "Game/Camera/DebugCamera.h"
// actor
#include "Game/WorldObject/Skydome.h"
#include "Game/Actor/Player/PlayerManager.h"

class GameScene 
	: public BaseScene {
public:

	GameScene();
	~GameScene();

	void Finalize() override;
	void Init() override;
	void Update() override;
	void Draw() const override;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif

private:

	// ------------------- camera ------------------- //
	std::unique_ptr<DebugCamera> debugCamera_;
	std::unique_ptr<FollowCamera> followCamera_;

	// ------------------- actor ------------------- //
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<PlayerManager> playerManager_;

};
