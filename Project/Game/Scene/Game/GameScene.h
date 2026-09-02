#pragma once
#include <memory>
// game
#include "Game/Scene/BaseScene.h"
#include "Game/Stage/StageBackgrounds.h"

class Player;
class FollowCamera;

class GameScene :
	public BaseScene {
public:

	GameScene();
	~GameScene() override;

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 初期化処理(インスタンスの宣言など)
	/// </summary>
	void Init() override;

	/// <summary>
	/// スタート時に呼ばれる処理(パラメータの読み込み、設定などはココで行う)
	/// </summary>
	void OnPlayStart() override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() const override;

private:

	std::unique_ptr<Player> player_;
	std::unique_ptr<FollowCamera> followCamera_;

	std::unique_ptr<StageBackgrounds> backgrounds_;
};
