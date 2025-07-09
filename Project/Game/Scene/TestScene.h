#pragma once
#include <memory>
#include "Engine/Render/SceneRenderer.h"
#include "Engine/Module/Components/GameObject/TestObject.h"
#include "Game/Scene/BaseScene.h"
// camera
#include "Game/Camera/DebugCamera.h"
#include "Game/Camera/Camera2d.h"
#include "Game/Camera/Camera3d.h"
#include "Game/WorldObject/Skybox.h"


class TestScene 
: public BaseScene {
public:


public:

	TestScene();
	~TestScene() override;

	void Finalize() override;
	void Init() override;
	void Update() override;
	void Draw() const override;

private:

	// ------------------- camera ------------------- //
	std::unique_ptr<Camera2d> camera2d_ = nullptr;
	std::unique_ptr<Camera3d> camera3d_ = nullptr;
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

	// ------------------- actor ------------------- //
	std::unique_ptr<Skybox> skybox_;

	std::unique_ptr<TestObject> testObject_;

	SceneRenderer* sceneRenderer_;
};

