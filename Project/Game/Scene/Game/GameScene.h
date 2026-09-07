#pragma once
#include <memory>
#include "Game/Scene/BaseScene.h"

#include "Engine/System/Audio/SoundManager.h"

/// game
#include "Game/EventHandlers/CollisionCallBacks.h"
#include "Game/EventHandlers/PlayerBlockCollisionCallBacks.h"
#include "Game/EventHandlers/BossBlockLauncherCollisionCallBacks.h"
#include "Game/Stage/StageBackgrounds.h"
#include "Game/Stage/StageBlockField.h"
#include "Game/Stage/StageSegment.h"
#include "Game/UI/RetryUI.h"
#include "Game/UI/PlayerUI.h"
#include "Game/UI/BossUI.h"
#include "Game/UI/IntroUI.h"


class Player;
class FollowCamera;
class Boss;
class DamageFloor;

class GameScene : public BaseScene{
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

protected:

	/// <summary>
	/// アクターを1フレーム進める
	/// </summary>
	/// <summary>
	/// アクターを進める。
	/// isStandby の間はプレイヤーとボスを止めて、カメラと背景だけ動かす
	/// </summary>
	void UpdateActors(float deltaTime, bool isStandby = false);

	/// <summary>
	/// 開始前のカウントダウンを流すシーンか。
	/// チュートリアルのように、始まりの演出が要らないシーンでは false にする
	/// </summary>
	virtual bool UseIntro() const { return true; }

	/// <summary>
	/// Playerの本体となるGameObjectを用意する。
	/// </summary>
	AOENGINE::BaseGameObject* ResolvePlayerBody();

	/// <summary>生成済みのステージを片付ける(Playを押し直した時の作り直しにも使う)</summary>
	void ClearStage();

	/// <summary>
	/// リトライの際の処理
	/// </summary>
	bool RetrySelect(bool isPlayerAlive);

protected:
	std::unique_ptr<Player> player_;

	AOENGINE::SoundHandle bgmHandle_;
	AOENGINE::SoundHandle gameOverBgmHandle_;

	/// CallBack 系 ------------------------------------
	PlayerBlockCollisionCallBacks playerBlockCallBacks_;
	BossBlockLauncherCollisionCallBacks bossBlockLauncherCallBacks_;

	/// Camera ------------------------------------
	std::unique_ptr<FollowCamera> followCamera_;

	// ステージ関連 ------------------------------------

	// ステージの背景
	std::unique_ptr<StageBackgrounds> backgrounds_;
	/// セグメントを跨いでブロックの連結グループを管理する表
	StageBlockField stageBlockField_;

	std::unique_ptr<Boss> boss_;
	std::unique_ptr<DamageFloor> damageFloor_;

	// コールバック関連
	CollisionCallBacks callBacks_;

	// UI  ------------------------------------
	std::unique_ptr<RetryUI> retryUI_;
	std::unique_ptr<PlayerUI> playerUI_;
	std::unique_ptr<BossUI> bossUI_;

	// 開始前のカウントダウン。終わるまでアクターは止めておく
	IntroUI introUI_;

};
