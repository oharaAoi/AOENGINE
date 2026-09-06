#pragma once
// game
#include "Game/Scene/Game/GameScene.h"
#include "Game/Tutorial/TutorialContext.h"
#include "Game/Tutorial/TutorialStepController.h"
#include "Game/Tutorial/TutorialTextTable.h"
#include "Game/UI/TutorialTextBoxUI.h"

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

	/// <summary>今のページに合わせて、テキストボックスの中身を差し替える</summary>
	void UpdateTextBox(float deltaTime);

private: // private variables

	// ページの進行管理
	TutorialStepController stepController_;

	// ページが触るものをまとめた入れ物
	TutorialContext context_;

	// 説明文の表と、それを出すテキストボックス
	TutorialTextTable textTable_;
	TutorialTextBoxUI textBox_;

	// ページを送る/戻すために閉じている最中か
	bool isChangingPage_ = false;
	// 閉じきった後、前のページへ戻るのか
	bool isGoingBack_ = false;
};
