#pragma once
#include <memory>
#include "Game/Scene/BaseScene.h"
// camera
#include "Game/Camera/FollowCamera.h"
#include "Game/Camera/DebugCamera.h"
#include "Game/Camera/Camera2d.h"
// actor
#include "Game/WorldObject/Floor.h"
#include "Game/Actor/Player/PlayerManager.h"
#include "Game/Actor/Boss/BossRoot.h"
// sprite
#include "Game/UI/CanvasUI.h"
#include "Game/UI/FadePanel.h"
// manager
#include "Engine/System/ParticleSystem/Emitter/GpuParticleEmitter.h"
#include "Game/CallBacks/GameCallBacksManager.h"

class GameScene 
	: public BaseScene {
public:

	GameScene();
	~GameScene();

	void Finalize() override;
	void Init() override;
	void Update() override;
	void PostUpdate() override;
	
private:

	// ------------------- camera ------------------- //
	std::unique_ptr<DebugCamera> debugCamera_;
	std::unique_ptr<FollowCamera> followCamera_;
	std::unique_ptr<Camera2d> camera2d_;

	// ------------------- actor ------------------- //
	std::unique_ptr<Floor> floor_;
	Skybox* skybox_;
	std::unique_ptr<PlayerManager> playerManager_;
	std::unique_ptr<BossRoot> bossRoot_;

	// ------------------- collision ------------------- //
	std::unique_ptr<GameCallBacksManager> gameCallBacksManager_;

	// ------------------- sprite ------------------- //
	std::unique_ptr<CanvasUI> canvas_;
	std::unique_ptr<FadePanel> fadePanel_;

	AOENGINE::GpuParticleEmitter* emitter_;
	AOENGINE::GpuParticleField* field_;
};
