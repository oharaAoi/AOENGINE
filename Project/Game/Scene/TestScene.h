#pragma once
#include <memory>
#include "Game/Scene/BaseScene.h"

/// game
#include "Game/EventHandlers/PlayerBlockCollisionCallBacks.h"
#include "Game/Stage/StageBlockField.h"
#include "Game/Stage/StageSegment.h"

class Player;

class TestScene
	: public BaseScene
{
public:
public:
	TestScene();
	~TestScene() override;

	void Finalize() override;
	void Init() override;
	void Update() override;
	void OnPlayStart() override;

private:
	/// <summary>
	/// Playerの本体となるGameObjectを用意する。
	/// Sceneに"Player"が置かれていればそれを使い、無ければPrefabから生成する。
	/// </summary>
	AOENGINE::BaseGameObject* ResolvePlayerBody();

	/// <summary>ステージのブロックを生成し、Colliderから引ける表へ登録する</summary>
	void SetupStage();

	/// <summary>生成済みのステージを片付ける(Playを押し直した時の作り直しにも使う)</summary>
	void ClearStage();

private:
	std::unique_ptr<Player> player_;
	/// PlayerとBlockの衝突コールバック
	PlayerBlockCollisionCallBacks playerBlockCallBacks_;
	/// セグメントを跨いでブロックの連結グループを管理する表
	StageBlockField stageBlockField_;
	/// CSVから読み込んだステージセグメント
	StageSegment stageSegment_;
};
