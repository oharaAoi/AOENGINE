#pragma once
#include <memory>
#include "Game/Scene/BaseScene.h"
// camera
#include "Game/Camera/FollowCamera.h"
#include "Game/Camera/DebugCamera.h"
#include "Game/Camera/Camera2d.h"
// actor
#include "Game/WorldObject/Skydome.h"
#include "Game/WorldObject/Floor.h"
#include "Game/WorldObject/Skybox.h"
#include "Game/Actor/Player/PlayerManager.h"
#include "Game/Actor/Boss/BossRoot.h"
// sprite
#include "Game/UI/CanvasUI.h"
#include "Game/UI/FadePanel.h"
// manager
#include "Engine/System/Manager/CollisionManager.h"
#include "Engine/System/ParticleSystem/Emitter/GpuParticleEmitter.h"
#include "Game/CallBacks/GameCallBacksManager.h"

#include "Engine/Module/Components/GameObject/GeometryObject.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Scene/SceneLoader.h"


class GameScene 
	: public BaseScene {
public:

	GameScene();
	~GameScene();

	void Finalize() override;
	void Init() override;
	void Update() override;
	void Draw() const override;

private:

	// ------------------- camera ------------------- //
	std::unique_ptr<DebugCamera> debugCamera_;
	std::unique_ptr<FollowCamera> followCamera_;
	std::unique_ptr<Camera2d> camera2d_;

	// ------------------- actor ------------------- //
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Floor> floor_;
	std::unique_ptr<Skybox> skybox_;
	std::unique_ptr<PlayerManager> playerManager_;
	std::unique_ptr<BossRoot> bossRoot_;

	// ------------------- collision ------------------- //
	std::unique_ptr<CollisionManager> collisionManager_;

	std::unique_ptr<GameCallBacksManager> gameCallBacksManager_;

	// ------------------- sprite ------------------- //
	std::unique_ptr<CanvasUI> canvas_;
	std::unique_ptr<FadePanel> fadePanel_;

	SceneLoader* sceneLoader_;
	SceneRenderer* sceneRenderer_;

	GpuParticleEmitter* emitter_;
	GpuParticleField* field_;
};
