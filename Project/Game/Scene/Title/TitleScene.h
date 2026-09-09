#pragma once

#include "Engine/System/Audio/SoundManager.h"

// game
#include "Game/Scene/BaseScene.h"
#include "Game/Actor/Common/ScreenWorldPlaneAnchor.h"
#include "Game/UI/TitleEarthParameter.h"
#include "Game/UI/TitleMeteoEffect.h"
#include "Game/UI/TitleUI.h"

class TitleScene :
	public BaseScene {
public:

	TitleScene() = default;
	~TitleScene() override;

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

private:

	/// <summary>
	/// 背景の地球を、画面上の決まった位置に見えるところへ置き直す
	/// </summary>
	void UpdateEarth();

private: // private variables

	AOENGINE::SoundHandle bgmHandle_;

	TitleUI titleUI_;

	// 背景に流す隕石
	TitleMeteoEffect meteoEffect_;

	// 背景の地球。画面上の位置で置き場所を決める
	TitleEarthParameter earthParameter_;
	ScreenWorldPlaneAnchor earthAnchor_;

	// シーン上での名前
	const std::string kEarthName = "Earth";

};

