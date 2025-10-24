#include "SceneFactory.h"
#include "Game/Scene/GameScene.h"
#include "Game/Scene/TitleScene.h"
#include "Game/Scene/TutorialScene.h"
#include "Game/Scene/TestScene.h"

std::unique_ptr<BaseScene> SceneFactory::CreateScene(SceneType _sceneType) {
	// 次のシーンを作成
	std::unique_ptr<BaseScene> newScene;

	if (_sceneType == SceneType::TITLE) {
		newScene = std::make_unique<TitleScene>();

	} else if (_sceneType == SceneType::GAME) {
		newScene = std::make_unique<GameScene>();

	} else if (_sceneType == SceneType::TUTORIAL) {
		newScene = std::make_unique<TutorialScene>();

	} else if (_sceneType == SceneType::TEST) {
		newScene = std::make_unique<TestScene>();
	}

	return newScene;
}