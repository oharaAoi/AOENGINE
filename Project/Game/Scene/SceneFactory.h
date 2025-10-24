#pragma once
#include "Game/Scene/AbstractSceneFactory.h"
#include "Game/Scene/SceneType.h"

/// <summary>
/// シーン工場
/// </summary>
class SceneFactory : public AbstractSceneFactory {
public:

	SceneFactory() {};
	~SceneFactory() {};

	/// <summary>
	/// シーンの生成
	/// </summary>
	/// <param name="sceneName">: sceneの名前</param>
	/// <returns>: 生成したシーン</returns>
	std::unique_ptr<BaseScene> CreateScene(SceneType _sceneType) override;
};

