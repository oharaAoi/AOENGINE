#include "BossAttackStopper.h"

#include <algorithm>
#include <cfloat>
#include <unordered_map>

#include "Engine/Lib/Math/Easing.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/System/Manager/PrefabManager.h"
#include "Engine/Utilities/SceneObjectFinder.h"

#include "Game/Actor/Boss/Boss.h"
#include "Game/Stage/StageBlockField.h"
#include "Game/WorldObject/Block.h"

namespace {
	// ブロック1個の高さ
	constexpr float kBlockHeight = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  開始
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackStopper::Enter(Boss& boss) {

	(void)boss;

	// 落とすのはアニメーションを見せてから
	stoppers_.clear();
	isFinished_ = false;
	isSpawned_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  終了
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackStopper::Exit(Boss& boss) {
	(void)boss;

	// 途中で行動が切り替わった場合、残っている足止めを片付ける
	for (Stopper& stopper : stoppers_) {
		if (stopper.entity.IsValid()) {
			stopper.entity.Destroy();
		}
	}
	stoppers_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackStopper::Update(Boss& boss, float deltaTime) {

	// アニメーションを見せてから攻撃を始める
	if (WaitStartDelay(deltaTime, boss.GetParameter().stopperStartDelay)) {
		return;
	}

	// 待ちが明けた最初のフレームで、まとめて落とす
	if (!isSpawned_) {
		isSpawned_ = true;
		SpawnStoppers(boss);
	}

	UpdateStoppers(boss, deltaTime);

	// 全部消えたら攻撃終了
	if (stoppers_.empty()) {
		isFinished_ = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  落とす足場を選ぶ
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackStopper::SpawnStoppers(const Boss& boss) {

	if (!pBlockField_) {
		// 候補が引けないので何も落とせない。すぐ終わらせる
		isFinished_ = true;
		return;
	}

	std::vector<LandingCandidate> candidates = CollectCandidates(boss);
	if (candidates.empty()) {
		isFinished_ = true;
		return;
	}

	// プレイヤーが乗っているグループは、隣接するブロックごと候補から外す
	ExcludePlayerGroup(candidates);

	// 残った候補からランダムに選んで落としていく。同じグループには2個落とさない
	const int32_t count = boss.GetParameter().stopperCount;
	for (int32_t i = 0; i < count && !candidates.empty(); ++i) {
		const int index = Random::RandomInt(0, static_cast<int>(candidates.size()) - 1);

		SpawnStopper(boss, candidates[index]);
		candidates.erase(candidates.begin() + index);
	}

	// 1個も落とせなかった場合はここで終わらせる
	if (stoppers_.empty()) {
		isFinished_ = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  落とし先の候補を集める
///////////////////////////////////////////////////////////////////////////////////////////////

std::vector<BossAttackStopper::LandingCandidate> BossAttackStopper::CollectCandidates(const Boss& boss) const {

	std::vector<LandingCandidate> candidates;

	// 上に何も乗っていない=乗れる足場を、連結グループごとにまとめる
	std::unordered_map<int, std::vector<Block*>> landableByGroup;
	for (Block* block : pBlockField_->GetLandableBlocks()) {

		// グループに属していないもの(壁など)は落とし先にしない
		const int groupId = block->GetGroupId();
		if (groupId == StageBlockField::kInvalidGroupId) {
			continue;
		}

		// 画面の外に落としても見えないので、カメラに映っている足場だけを使う
		if (!boss.IsInCameraView(block->GetPosition())) {
			continue;
		}

		landableByGroup[groupId].push_back(block);
	}

	// グループごとに、中心へ一番近い足場を落とし先にする。
	// 中心そのものはブロックの間に来ることがあるため、マスに合う位置へ寄せる
	for (const auto& group : landableByGroup) {

		Math::Vector3 center = CVector3::ZERO;
		if (!pBlockField_->TryGetGroupCenter(group.first, center)) {
			continue;
		}

		Block* landingBlock = nullptr;
		float nearestDistance = FLT_MAX;
		for (Block* block : group.second) {
			const Math::Vector3 diff = block->GetPosition() - center;
			const float distance = diff.x * diff.x + diff.z * diff.z;
			if (distance < nearestDistance) {
				nearestDistance = distance;
				landingBlock = block;
			}
		}

		if (!landingBlock) {
			continue;
		}

		candidates.push_back(LandingCandidate{ group.first, landingBlock, center });
	}

	return candidates;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  プレイヤーがいるグループを候補から外す
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackStopper::ExcludePlayerGroup(std::vector<LandingCandidate>& candidates) const {

	// 候補が1つしか無い時に外すと落とせなくなるので、その時は残す
	if (candidates.size() <= 1) {
		return;
	}

	AOENGINE::BaseGameObject* player = FindSceneObject<AOENGINE::BaseGameObject>("Player");
	if (!player) {
		return;
	}

	AOENGINE::WorldTransform* playerTransform = player->GetTransform();
	if (!playerTransform) {
		return;
	}

	const Math::Vector3 playerPos = playerTransform->GetTranslate();

	// プレイヤーに一番近いブロックを持つグループを探す。
	auto nearest = candidates.end();
	float nearestDistance = FLT_MAX;
	for (auto it = candidates.begin(); it != candidates.end(); ++it) {

		const std::vector<Block*>* members = pBlockField_->GetGroup(it->groupId);
		if (!members) {
			continue;
		}

		for (const Block* member : *members) {
			if (member == nullptr || !member->IsValid()) {
				continue;
			}

			const Math::Vector3 diff = member->GetPosition() - playerPos;
			const float distance = diff.x * diff.x + diff.y * diff.y;
			if (distance < nearestDistance) {
				nearestDistance = distance;
				nearest = it;
			}
		}
	}

	if (nearest != candidates.end()) {
		candidates.erase(nearest);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  足止めの生成
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackStopper::SpawnStopper(const Boss& boss, const LandingCandidate& target) {

	AOENGINE::SceneObject* root = AOENGINE::PrefabManager::GetInstance()->Instantiate(kStopperName_);
	AOENGINE::BaseGameObject* object = dynamic_cast<AOENGINE::BaseGameObject*>(root);
	if (!object) {
		return;
	}

	stoppers_.push_back(Stopper{});
	Stopper& stopper = stoppers_.back();
	stopper.entity.Bind(object);

	const BossParameter& param = boss.GetParameter();

	// X,Zはグループの中心にあたる足場に合わせ、Yはプレイヤーの一定距離上から落とす
	Math::Vector3 spawnPosition = target.landingBlock->GetPosition();
	if (AOENGINE::BaseGameObject* player = FindSceneObject<AOENGINE::BaseGameObject>("Player")) {
		if (AOENGINE::WorldTransform* playerTransform = player->GetTransform()) {
			spawnPosition.y = playerTransform->GetTranslate().y;
		}
	}
	spawnPosition.y += param.stopperSpawnHeight;

	if (AOENGINE::WorldTransform* transform = stopper.entity.GetTransform()) {
		transform->SetTranslate(spawnPosition);
		transform->SetScale(param.stopperSize);
	}

	// 横方向の押し戻しで流されないように、生成時のX,Zを覚えておく
	stopper.spawnX = spawnPosition.x;
	stopper.spawnZ = spawnPosition.z;

	// 真下に落ちるだけなので、乗る高さは最初に決まる
	stopper.startY = spawnPosition.y;
	stopper.restY = target.landingBlock->GetPosition().y + kBlockHeight * 0.5f + param.stopperSize.y * 0.5f;

	// 落ちる距離と速度から、落ちきるまでの時間を出しておく
	stopper.fallTimer = 0.0f;
	const float distance = stopper.startY - stopper.restY;
	stopper.fallDuration = (param.stopperSpeed > 0.0f) ? (distance / param.stopperSpeed) : 0.0f;

	// Colliderのsizeにはtransformのscaleが掛かる

	// 画面外で当たらないように、カメラに映るまで判定そのものを切っておく
	if (AOENGINE::BaseCollider* collider = stopper.entity.GetCollider(kStopperName_)) {
		collider->SetIsActive(false);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  落下・着地・寿命
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackStopper::UpdateStoppers(Boss& boss, float deltaTime) {

	const BossParameter& param = boss.GetParameter();

	for (auto it = stoppers_.begin(); it != stoppers_.end();) {

		// 既に消えていたらリストから外す
		if (!it->entity.IsValid()) {
			it = stoppers_.erase(it);
			continue;
		}

		// X,Zは生成時のまま、着地後はYも固定する
		if (AOENGINE::WorldTransform* transform = it->entity.GetTransform()) {
			Math::Vector3 pos = transform->GetTranslate();
			pos.x = it->spawnX;
			pos.z = it->spawnZ;
			if (it->isLanded) {
				pos.y = it->restY;
			}
			transform->SetTranslate(pos);
		}

		AOENGINE::BaseCollider* collider = it->entity.GetCollider(kStopperName_);

		// 画面に入ってきたら当たり判定を始める
		// キャッシュされた座標ではなくtransformの現在値を使う
		if (collider && !collider->GetIsActive()) {
			if (AOENGINE::WorldTransform* transform = it->entity.GetTransform()) {
				if (boss.IsBelowCameraTop(transform->GetTranslate())) {
					collider->SetIsActive(true);
				}
			}
		}

		if (!it->isLanded) {
			// 落下開始からの進み具合をイージングにかけて高さを決める
			it->fallTimer += deltaTime;

			float t = 1.0f;
			if (it->fallDuration > 0.0f) {
				t = std::clamp(it->fallTimer / it->fallDuration, 0.0f, 1.0f);
			}
			const float easedT = Math::CallEasing(param.stopperEaseKind, t);

			if (AOENGINE::WorldTransform* transform = it->entity.GetTransform()) {
				Math::Vector3 pos = transform->GetTranslate();
				pos.y = it->startY + (it->restY - it->startY) * easedT;
				transform->SetTranslate(pos);
			}

			// 落ちきったら着地。カメラを揺らす
			if (t >= 1.0f) {
				it->isLanded = true;
				it->remainingLifeTime = param.stopperLifeTime;
				boss.ShakeCamera(param.stopperLandShake);
			}

			++it;
			continue;
		}

		// 着地後はしばらく残ってから消える
		it->remainingLifeTime -= deltaTime;
		if (it->remainingLifeTime <= 0.0f) {
			it->entity.Destroy();
			it = stoppers_.erase(it);
			continue;
		}

		++it;
	}
}
