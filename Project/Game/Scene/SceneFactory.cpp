#include "SceneFactory.h"
#include "Game/Scene/GameScene.h"
#include "Game/Scene/TitleScene.h"
#include "Game/Scene/TutorialScene.h"
#include "Game/Scene/TestScene.h"

std::unique_ptr<BaseScene> SceneFactory::CreateScene(SceneType _sceneType) {
	// 次のシーンを作成
	std::unique_ptr<BaseScene> newScene;

	if (_sceneType == SceneType::Title) {
		newScene = std::make_unique<TitleScene>();

	} else if (_sceneType == SceneType::Game) {
		newScene = std::make_unique<GameScene>();

	} else if (_sceneType == SceneType::Tutorial) {
		newScene = std::make_unique<TutorialScene>();

	} else if (_sceneType == SceneType::Test) {
		newScene = std::make_unique<TestScene>();
	}

	return newScene;
}