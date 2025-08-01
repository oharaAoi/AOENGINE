#pragma once
#include "Engine.h"
#include "Engine/Utilities/Loader.h"
#include "Game/Scene/SceneFactory.h"
#include "Game/Scene/BaseScene.h"
#include "Game/Scene/TestScene.h"
#include "Game/Scene/GameScene.h"
#include "Engine/System/Manager/ModelManager.h"
#include "Engine/System/Input/Input.h"

/// <summary>
/// シーンを管理するクラス
/// </summary>
class SceneManager {
public:

	SceneManager();
	~SceneManager();

	void Finalize();
	void Init();
	void Update();
	void Draw();

	void Debug_Gui();

public:

	void SetChange(const SceneType& type);

	void Reset();

	void Free();

	bool CheckReset();

private:

	std::unique_ptr<SceneFactory> sceneFactory_;

	std::unique_ptr<BaseScene> scene_ = nullptr;
	std::unique_ptr<BaseScene> nextScene_ = nullptr;

	SceneType changeScene_;

	bool reset_;

#ifdef _DEBUG
	int selectSceneNum_ = 0;
	bool isSceneChange_ = 0;
#endif // _DEBUG

};

