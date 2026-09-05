#include "BossAttackFallFire.h"

#include <algorithm>
#include <cmath>

#include "Engine/Lib/Math/Easing.h"

#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/System/Manager/PrefabManager.h"
#include "Engine/Utilities/SceneObjectFinder.h"

#include "Game/Actor/Boss/Boss.h"


void BossAttackFallFire::Enter(Boss& boss) {
	(void)boss;

	// 初期化
	fireballs_.clear();
	spawnedCount_ = 0;
	spawnTimer_ = 0.0f;
	isFinished_ = false;
}

void BossAttackFallFire::Exit(Boss& boss) {
	(void)boss;

	// 途中で行動が切り替わった場合、残っている火玉を片付ける
	for (Fireball& fireball : fireballs_) {
		// 消す前にコールバックを外す
		if (AOENGINE::BaseCollider* collider = fireball.entity.GetCollider(kFireBallColliderTag_)) {
			collider->SetOnCollision(nullptr);
		}
		fireball.entity.Destroy();
	}
	fireballs_.clear();
}


void BossAttackFallFire::Update(Boss& boss, float deltaTime) {

	// アニメーションを見せてから攻撃を始める
	if (WaitStartDelay(deltaTime, boss.GetParameter().fireballStartDelay)) {
		return;
	}

	// タイマー更新
	UpdateSpawnTimer(deltaTime, boss);

	// 火玉更新
	UpdateFireBall(boss, deltaTime);

	// 全部落とし終わって、画面上の火球も無くなったら攻撃終了
	if (spawnedCount_ >= boss.GetParameter().fireballDropCount && fireballs_.empty()) {
		isFinished_ = true;
	}
}

void BossAttackFallFire::SpawnFireball(const Boss& boss) {

	// プレイヤー取得
	AOENGINE::BaseGameObject* player = FindSceneObject<AOENGINE::BaseGameObject>("Player");
	if (!player) {
		return;
	}

	// Transform所得
	AOENGINE::WorldTransform* playerTransform = player->GetTransform();
	if (!playerTransform) {
		return;
	}

	// X,Zはプレイヤーに合わせ、Yはプレイヤーの一定距離上から落とす
	Math::Vector3 spawnPosition = playerTransform->GetTranslate();
	spawnPosition.y += boss.GetParameter().fireballSpawnHeight;

	// 火玉のprefab用意
	AOENGINE::SceneObject* root = AOENGINE::PrefabManager::GetInstance()->Instantiate("Fireball");
	AOENGINE::BaseGameObject* object = dynamic_cast<AOENGINE::BaseGameObject*>(root);
	if (!object) {
		return;
	}

	
	Fireball fireball{};
	fireball.entity.Bind(object);

	// 落ち始めてからの経過時間。これを元に速度を補間する
	fireball.fallTimer = 0.0f;

	// スタート位置の適用
	if (AOENGINE::WorldTransform* transform = fireball.entity.GetTransform()) {
		transform->SetTranslate(spawnPosition);
	}

	fireballs_.push_back(fireball);

	// 何かに当たったらフラグだけ立てる
	Fireball* spawned = &fireballs_.back();
	if (AOENGINE::BaseCollider* collider = spawned->entity.GetCollider(kFireBallColliderTag_)) {
		collider->SetOnCollision([spawned](AOENGINE::BaseCollider*) { spawned->isHit = true; });

		// 画面外で当たらないように、カメラに映るまで判定を切っておく
		collider->SetIsActive(false);
	}
}

void BossAttackFallFire::UpdateSpawnTimer(float deltaTime, const Boss& boss) {

	const BossParameter& param = boss.GetParameter();

	// 発射カウントの上限
	if (spawnedCount_ >= param.fireballDropCount) {
		return;
	}

	// タイマー更新
	spawnTimer_ -= deltaTime;

	// タイマー0で火玉を生成
	if (spawnTimer_ <= 0.0f) {
		SpawnFireball(boss);
		++spawnedCount_;
		spawnTimer_ = param.fireballDropInterval;
	}

}

void  BossAttackFallFire::UpdateFireBall(const Boss& boss, float deltaTime) {

	const BossParameter& param = boss.GetParameter();

	// 落下中の火玉を進める
	for (auto it = fireballs_.begin(); it != fireballs_.end();) {

		// 画面上にいなかったら消す
		if (!it->entity.IsValid()) {
			it = fireballs_.erase(it);
			continue;
		}

		// 落ち始めの速度から上限の速度へ、イージングで繋いでいく
		it->fallTimer += deltaTime;

		float t = 1.0f;
		if (param.fireballFallSpeedUpTime > 0.0f) {
			t = std::clamp(it->fallTimer / param.fireballFallSpeedUpTime, 0.0f, 1.0f);
		}
		const float easedT = Math::CallEasing(param.fireballFallEaseKind, t);
		const float fallSpeed = param.fireballFallStartSpeed
			+ (param.fireballMaxFallSpeed - param.fireballFallStartSpeed) * easedT;

		// 真下に落とす
		if (AOENGINE::WorldTransform* transform = it->entity.GetTransform()) {
			Math::Vector3 pos = transform->GetTranslate();
			pos.y -= fallSpeed * deltaTime;
			transform->SetTranslate(pos);

			// 画面に入ってきたら当たり判定を始める
			if (AOENGINE::BaseCollider* collider = it->entity.GetCollider(kFireBallColliderTag_)) {
				if (!collider->GetIsActive() && boss.IsBelowCameraTop(pos)) {
					collider->SetIsActive(true);
				}
			}
		}

		// 何かしらに着弾したら消す
		if (it->isHit) {
			// 消す前にコールバックを外す
			if (AOENGINE::BaseCollider* collider = it->entity.GetCollider(kFireBallColliderTag_)) {
				collider->SetOnCollision(nullptr);
			}
			it->entity.Destroy();
			it = fireballs_.erase(it);
			continue;
		}
		++it;
	}
}
