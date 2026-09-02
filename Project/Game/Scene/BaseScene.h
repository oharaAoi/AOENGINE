#pragma once
#include <optional>
#include <string>
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Scene/SceneLoader.h"
#include "Engine/System/Manager/CollisionManager.h"
#include "Game/Scene/SceneType.h"
#include "Game/WorldObject/Skybox.h"
// camera
#include "Engine/Module/Entity/Camera/DebugCamera.h"
#include "Engine/Module/Entity/Camera/Camera2d.h"
#include "Engine/Module/Entity/Camera/Camera3d.h"

/// <summary>
/// BaseとなるScene
/// </summary>
class BaseScene {
public:

	BaseScene() = default;
	virtual ~BaseScene();

public:

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void Finalize() = 0;

	/// <summary>
	/// 共通初期化後に派生SceneのInitを呼ぶ。SceneManagerだけが呼び出す。
	/// </summary>
	void Initialize();

	/// <summary>各シーン固有の初期化処理。</summary>
	virtual void Init() = 0;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// EditorでPlayを開始したときに一度だけ呼ばれる処理
	/// </summary>
	virtual void OnPlayStart() {}

	/// <summary>
	/// Updateより後に呼び出される更新処理
	/// </summary>
	virtual void PostUpdate() {};

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() const;

	/// <summary>
	/// Sceneの読み込み
	/// </summary>
	/// <param name="directory">: ディレクトリパス</param>
	/// <param name="fileName">: sceneが保存されているファイル名</param>
	/// <param name="extension">: 拡張子</param>
	void LoadScene(const std::string& directory, const std::string& fileName, const std::string& extension);

	/// <summary>
	/// 更新処理の流れ
	/// </summary>
	void UpdateProcess();
	/// <summary>Edit/Pause中にゲーム時間を進めず描画とEditor Cameraだけ更新する。</summary>
	void EditorUpdateProcess();

	/// <summary>
	/// シーンの情報を保存する
	/// </summary>
	void SaveSceneEffect();

	/// <summary>
	/// シーンの情報を読み込む
	/// </summary>
	void LoadSceneEffect();

	/// <summary>
	/// Cameraをシーンに登録する
	/// </summary>
	void RegisterCamera();

public:

	const std::optional<SceneType>& GetNextSceneType() const { return nextSceneType_; }
	void SetNextSceneType(const std::optional<SceneType>& type) { nextSceneType_ = type; }

	void SetSceneName(const std::string& sceneName) { sceneName_ = sceneName; }
	const std::string& GetSceneName() const { return sceneName_; }

protected:

	std::optional<SceneType> nextSceneType_ = std::nullopt;

	std::unique_ptr<AOENGINE::CollisionManager> collisionManager_;

	AOENGINE::SceneRenderer* pSceneRenderer_ = nullptr;
	AOENGINE::SceneLoader* pSceneLoader_;

	std::unique_ptr<Skybox> skybox_;

	std::string sceneName_;

	// ------------------- camera ------------------- //
	std::unique_ptr<Camera2d> camera2d_ = nullptr;
	std::unique_ptr<Camera3d> camera3d_ = nullptr;
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;
};

