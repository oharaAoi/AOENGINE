#pragma once
#include "Engine.h"
#include "Game/Scene/SceneFactory.h"
#include "Game/Scene/BaseScene.h"
#include "Engine/System/Manager/SystemManager.h"

namespace AOENGINE {

/// <summary>
/// シーンを管理するクラス
/// </summary>
class SceneManager {
public: // コンストラクタ

	SceneManager();
	~SceneManager();

public:

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 編集処理
	/// </summary>
	void Debug_Gui();

public:

	/// <summary>
	/// シーンの変更
	/// </summary>
	/// <param name="type"></param>
	void SetChange(const SceneType& type);

	/// <summary>
	/// シーンを保存する
	/// </summary>
	void SaveScene();

private:

	/// <summary>
	/// Manager関連のリセット
	/// </summary>
	void ResetManager();

private:

	std::unique_ptr<SceneFactory> sceneFactory_;
	std::unique_ptr<SystemManager> systemManager_;

	std::unique_ptr<BaseScene> scene_ = nullptr;
	std::unique_ptr<BaseScene> nextScene_ = nullptr;

	SceneType changeScene_;

	bool reset_;

#ifdef _DEBUG
	int selectSceneNum_ = 0;
	bool isSceneChange_ = 0;
#endif // _DEBUG

};

}