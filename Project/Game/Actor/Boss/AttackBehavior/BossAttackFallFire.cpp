#include "BossAttackFallFire.h"

#include <cmath>

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
	bounceTimer_ = 0.0f;
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

	// タイマー更新
	UpdateSpawnTimer(deltaTime, boss);

	// 火玉更新
	UpdateFireBall(deltaTime);

	// この行動中はボスを上下に跳ねさせる
	UpdateBossBounce(boss, deltaTime);

	// 全部落とし終わって、画面上の火球も無くなったら攻撃終了
	if (spawnedCount_ >= dropCount_ && fireballs_.empty()) {
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

	// X,Zはプレイヤーに合わせ、Yはボスの高さ + オフセットから落とす
	Math::Vector3 spawnPosition = playerTransform->GetTranslate();
	spawnPosition.y = boss.GetPosition().y + boss.GetParameter().fireballSpawnOffsetY;

	// 火玉のprefab用意
	AOENGINE::SceneObject* root = AOENGINE::PrefabManager::GetInstance()->Instantiate("Fireball");
	AOENGINE::BaseGameObject* object = dynamic_cast<AOENGINE::BaseGameObject*>(root);
	if (!object) {
		return;
	}

	
	Fireball fireball{};
	fireball.entity.Bind(object);

	// スタート位置の適用
	if (AOENGINE::WorldTransform* transform = fireball.entity.GetTransform()) {
		transform->SetTranslate(spawnPosition);
	}

	fireballs_.push_back(fireball);

	// 何かに当たったらフラグだけ立てる
	Fireball* spawned = &fireballs_.back();
	if (AOENGINE::BaseCollider* collider = spawned->entity.GetCollider(kFireBallColliderTag_)) {
		collider->SetOnCollision([spawned](AOENGINE::BaseCollider*) { spawned->isHit = true; });
	}
}

void BossAttackFallFire::UpdateSpawnTimer(float deltaTime, const Boss& boss) {

	// 発射カウントの上限
	if (spawnedCount_ >= dropCount_) {
		return;
	}

	// タイマー更新
	spawnTimer_ -= deltaTime;

	// タイマー0で火玉を生成
	if (spawnTimer_ <= 0.0f) {
		SpawnFireball(boss);
		++spawnedCount_;
		spawnTimer_ = launchInterval_;
	}

}

void  BossAttackFallFire::UpdateFireBall(float deltaTime) {

	// 落下中の火玉を進める
	for (auto it = fireballs_.begin(); it != fireballs_.end();) {

		// 画面上にいなかったら消す
		if (!it->entity.IsValid()) {
			it = fireballs_.erase(it);
			continue;
		}

		// 真下に落とす
		if (AOENGINE::WorldTransform* transform = it->entity.GetTransform()) {
			Math::Vector3 pos = transform->GetTranslate();
			pos.y -= fallSpeed_ * deltaTime;
			transform->SetTranslate(pos);
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


void  BossAttackFallFire::UpdateBossBounce(const Boss& boss, float deltaTime) {
	bounceTimer_ += deltaTime;

	// sin関数でポンポン跳ねる
	const BossParameter& param = boss.GetParameter();
	const float offsetY = std::abs(std::sin(bounceTimer_ * param.bounceSpeed)) * param.bounceHeight;

	// 基準位置へのオフセットとして毎フレーム乗せる
	if (AOENGINE::WorldTransform* transform = boss.GetTransform()) {
		Math::Vector3 pos = transform->GetTranslate();
		pos.y += offsetY;
		transform->SetTranslate(pos);
	}
}