#include "SceneFactory.h"
#include "Game/Scene/TestScene.h"
#include "Game/Scene/Game/GameScene.h"
#include "Game/Scene/Title/TitleScene.h"
#include "Game/Scene/Clear/ClearScene.h"

std::unique_ptr<BaseScene> SceneFactory::CreateScene(SceneType _sceneType) {
	// 次のシーンを作成
	std::unique_ptr<BaseScene> newScene;

	if (_sceneType == SceneType::Test) {
		newScene = std::make_unique<TestScene>();
	} else if (_sceneType == SceneType::Game) {
		newScene = std::make_unique<GameScene>();
	} else if (_sceneType == SceneType::Title) {
		newScene = std::make_unique<TitleScene>();
	} else if (_sceneType == SceneType::Clear) {
		newScene = std::make_unique<ClearScene>();
	}

	return newScene;
}