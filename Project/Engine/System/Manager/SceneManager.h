#pragma once
#include "Engine/Core/Engine.h"
#include "Game/Scene/SceneFactory.h"
#include "Game/Scene/BaseScene.h"
#include "Engine/System/Manager/SystemManager.h"
#include "Game/Scene/Transition/SceneTransition.h"

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
	bool Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// EditorのPlay開始を現在のシーンへ通知する
	/// </summary>
	void OnPlayStart();
	SceneType GetCurrentSceneType() const { return nowScene_; }

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
	bool LoadScene();

private:
	enum class TransitionState {
		Idle,
		Covering,
		Revealing
	};

	void BeginSceneTransition(SceneType type);
	/// trueならこのフレームにSceneを置き換えた。
	bool UpdateSceneTransition();

	/// <summary>
	/// Manager関連のリセット
	/// </summary>
	void ResetManager();

private:

	std::unique_ptr<SceneFactory> sceneFactory_;
	std::unique_ptr<SystemManager> systemManager_;

	std::unique_ptr<BaseScene> scene_ = nullptr;
	std::unique_ptr<BaseScene> nextScene_ = nullptr;
	std::unique_ptr<SceneTransition> sceneTransition_;
	std::optional<SceneType> pendingSceneType_;
	std::optional<SceneType> editorSceneChangeRequest_;
	TransitionState transitionState_ = TransitionState::Idle;

	SceneType changeScene_;
	SceneType nowScene_;

	bool reset_;

#ifdef _DEVELOPMENT
	int selectSceneNum_ = 0;
	bool isSceneChange_ = 0;
#endif // _DEVELOPMENT

};

}
