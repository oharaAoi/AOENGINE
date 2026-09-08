#pragma once

/// stl
#include <string>

/// engine
#include "Engine/System/Manager/CollisionManager.h"
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"

class BlockGroupLauncherManager;

/// <summary>
/// ボスが落とした足止め(Stopper)に、ブロックの塊(BlockGroup)が当たった時のコールバック。
/// 当たった音を鳴らすだけで、押し戻しや破棄などは行わない
/// </summary>
class StopperToBlockGroupCallBacks :
	public AOENGINE::BaseCollisionCallBacks {
public: // constructor

	StopperToBlockGroupCallBacks(AOENGINE::CollisionManager* _collisionManager) : pCollisionManager_(_collisionManager) {};
	~StopperToBlockGroupCallBacks() override = default;

public: // public method

	void Init() override;
	void Update() override {};

	void CollisionEnter(AOENGINE::BaseCollider* const stopper, AOENGINE::BaseCollider* const block) override;
	void CollisionStay(AOENGINE::BaseCollider* const stopper, AOENGINE::BaseCollider* const block) override;
	void CollisionExit(AOENGINE::BaseCollider* const stopper, AOENGINE::BaseCollider* const block) override;

public: // accessor

	// 当たったブロックが塊として動いているかを調べるために使う
	void SetLauncherManager(BlockGroupLauncherManager* manager) { pLauncherManager_ = manager; }

private: // private variable

	AOENGINE::CollisionManager* pCollisionManager_ = nullptr;
	BlockGroupLauncherManager* pLauncherManager_ = nullptr;

	// 最後に音を鳴らした時刻。まだ一度も鳴らしていないことが分かるように、
	// 開始時刻(0秒)から見ても間隔が空いている値にしておく
	float lastPlayedTime_ = -100.0f;

	// Collider category名(Prefab側の設定と合わせる)
	const std::string kStopperName_ = "Stopper";
	const std::string kBlockName_ = "Block";
	const std::string kLaunchedBlockName_ = "LaunchedBlock";

	// 衝突時に鳴らすSE
	const std::string kSeName_ = "StopperBlocksCollision";

	// 塊のブロックが次々に当たるため、この間隔より短い間は鳴らし直さない
	static constexpr float kPlayInterval_ = 0.15f;

};
