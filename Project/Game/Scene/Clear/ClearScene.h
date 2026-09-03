#pragma once
// game
#include "Game/Scene/BaseScene.h"
#include "Game/UI/GameClearUI.h"

class ClearScene : 
	public BaseScene {
public:

	ClearScene() = default;
	~ClearScene() override;

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

private: // private variables

	GameClearUI clearUI_;

};

