#pragma once
// game
#include "Game/Scene/Game/GameScene.h"
#include "Game/Tutorial/TutorialContext.h"
#include "Game/Tutorial/TutorialStepController.h"

/// <summary>
/// 操作方法とできることを伝えるチュートリアルシーン
/// </summary>
class TutorialScene :
	public GameScene {
public:

	TutorialScene() = default;
	~TutorialScene() override;

	/// <summary>
	/// スタート時に呼ばれる処理(パラメータの読み込み、設定などはココで行う)
	/// </summary>
	void OnPlayStart() override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

private:

	/// <summary>今フレームの入力と接続状況を、ページ側へ渡す形にまとめる</summary>
	void UpdateContext();

private: // private variables

	// ページの進行管理
	TutorialStepController stepController_;

	// ページが触るものをまとめた入れ物
	TutorialContext context_;
};
