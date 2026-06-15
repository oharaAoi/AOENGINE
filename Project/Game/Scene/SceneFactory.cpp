#include "SceneFactory.h"
#include "Game/Scene/TestScene.h"

std::unique_ptr<BaseScene> SceneFactory::CreateScene(SceneType _sceneType) {
	// 次のシーンを作成
	std::unique_ptr<BaseScene> newScene;

	if (_sceneType == SceneType::Test) {
		newScene = std::make_unique<TestScene>();
	}

	return newScene;
}