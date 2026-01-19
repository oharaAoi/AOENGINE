#pragma once
#include <optional>
#include <string>
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Scene/SceneLoader.h"
#include "Engine/System/Manager/CollisionManager.h"
#include "Game/WorldObject/Skybox.h"
#include "Game/Scene/SceneType.h"

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
	/// 初期化処理
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

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

	/// <summary>
	/// シーンの情報を保存する
	/// </summary>
	void SaveScene();

public:

	const std::optional<SceneType>& GetNextSceneType() const { return nextSceneType_; }
	void SetNextSceneType(const std::optional<SceneType>& type) { nextSceneType_ = type; }

	void SetSceneName(const std::string& sceneName) { sceneName_ = sceneName; }

protected:

	std::optional<SceneType> nextSceneType_ = std::nullopt;

	std::unique_ptr<AOENGINE::CollisionManager> collisionManager_;

	AOENGINE::SceneRenderer* pSceneRenderer_ = nullptr;
	AOENGINE::SceneLoader* pSceneLoader_;
	Skybox* skybox_;

	std::string sceneName_;
};

