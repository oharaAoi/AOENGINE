#pragma once
#include <memory>
#include "Game/Scene/BaseScene.h"
// camera
#include "Game/Camera/FollowCamera.h"
#include "Game/Camera/DebugCamera.h"
#include "Game/Camera/Camera2d.h"
// actor
#include "Game/WorldObject/Skydome.h"
#include "Game/Actor/Player/PlayerManager.h"
#include "Game/Actor/Boss/Boss.h"
// Sprite
#include "Game/UI/Reticle.h"
#include "Engine/System/Manager/ParticleManager.h"

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
	std::unique_ptr<Camera2d> camera2d_;

	// ------------------- actor ------------------- //
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<PlayerManager> playerManager_;
	std::unique_ptr<Boss> boss_;

	// ------------------- sprite ------------------- //
	std::unique_ptr<Reticle> reticle_;

	ParticleManager* particleManager_;

	std::unique_ptr<GeometryObject> sphere_;
	std::unique_ptr<GeometryObject> cylinder_;
};
