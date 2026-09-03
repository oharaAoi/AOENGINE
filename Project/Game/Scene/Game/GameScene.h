#pragma once
#include <memory>
#include "Game/Scene/BaseScene.h"
/// game
#include "Game/EventHandlers/PlayerBlockCollisionCallBacks.h"
#include "Game/Stage/StageBackgrounds.h"
#include "Game/Stage/StageBlockField.h"
#include "Game/Stage/StageSegment.h"

class Player;
class FollowCamera;
class Boss;
class DamageFloor;

class GameScene : public BaseScene
{
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
	/// <summary>
	/// Playerの本体となるGameObjectを用意する。
	/// Sceneに"Player"が置かれていればそれを使い、無ければPrefabから生成する。
	/// </summary>
	AOENGINE::BaseGameObject *ResolvePlayerBody();

	/// <summary>ステージのブロックを生成し、Colliderから引ける表へ登録する</summary>
	void SetupStage();

	/// <summary>生成済みのステージを片付ける(Playを押し直した時の作り直しにも使う)</summary>
	void ClearStage();

private:
	std::unique_ptr<Player> player_;
	/// PlayerとBlockの衝突コールバック
	PlayerBlockCollisionCallBacks playerBlockCallBacks_;

	std::unique_ptr<FollowCamera> followCamera_;

	// ステージ関連 ------------------------------------

	// ステージの背景
	std::unique_ptr<StageBackgrounds> backgrounds_;
	/// セグメントを跨いでブロックの連結グループを管理する表
	StageBlockField stageBlockField_;
	/// CSVから読み込んだステージセグメント
	StageSegment stageSegment_;
	std::unique_ptr<Boss> boss_;
	std::unique_ptr<DamageFloor> damageFloor_;
};
