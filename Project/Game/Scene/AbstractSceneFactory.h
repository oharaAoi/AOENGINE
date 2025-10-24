#pragma once
#include "Game/Scene/BaseScene.h"
#include "Game/Scene/SceneType.h"

/// <summary>
/// シーン工場
/// </summary>
class AbstractSceneFactory {
public:
	/// <summary>
	/// 仮想デストラクタ
	/// </summary>
	virtual ~AbstractSceneFactory() = default;

	/// <summary>
	/// シーン生成
	/// </summary>
	/// <param name="sceneName">: sceneの名前</param>
	/// <returns>: scene</returns>
	virtual std::unique_ptr<BaseScene> CreateScene(SceneType _sceneType) = 0;
};

