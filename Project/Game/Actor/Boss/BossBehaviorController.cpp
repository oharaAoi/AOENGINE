#include "BossBehaviorController.h"

#include <limits>
#include <vector>

#include "Engine/Lib/Math/MyRandom.h"
#include "Engine/System/Manager/ImGuiManager.h"

#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/AttackBehavior/BossAttackFallFire.h"
#include "Game/Actor/Boss/AttackBehavior/BossAttackBeam.h"
#include "Game/Actor/Boss/AttackBehavior/BossAttackIdle.h"
#include "Game/Actor/Boss/AttackBehavior/BossAttackStopper.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//  初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void BossBehaviorController::Init(Boss& boss) {

	attackUseCounts_.fill(0);
	hasPreviousAttack_ = false;

	// 開始直後にいきなり殴らないよう、まずIdleから入る
	nextIsIdle_ = true;

	ChangeBehavior(boss, SelectNextBehavior(boss));
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void BossBehaviorController::Update(Boss& boss, float deltaTime) {
	// 行動がまだ無ければここで用意する
	if (!currentBehavior_) {
		ChangeBehavior(boss, SelectNextBehavior(boss));
		if (!currentBehavior_) {
			return;
		}
	}

	currentBehavior_->Update(boss, deltaTime);

	// 行動が終わったら次の行動へ切り替える
	if (currentBehavior_->IsFinished()) {
		ChangeBehavior(boss, SelectNextBehavior(boss));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  行動の切り替え
///////////////////////////////////////////////////////////////////////////////////////////////

void BossBehaviorController::ChangeBehavior(Boss& boss, std::unique_ptr<BaseBossAttackBehavior> next) {
	// 今の行動を終わらせる
	if (currentBehavior_) {
		currentBehavior_->Exit(boss);
	}

	currentBehavior_ = std::move(next);

	// 新しい行動を始める
	if (currentBehavior_) {
		currentBehavior_->Enter(boss);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  次の行動を決める
///////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<BaseBossAttackBehavior> BossBehaviorController::SelectNextBehavior(const Boss& boss) {

	// 攻撃の後は必ずIdleを挟む
	if (nextIsIdle_) {
		nextIsIdle_ = false;
		return std::make_unique<BossAttackIdle>();
	}

	const AttackKind kind = SelectNextAttack(boss);

	// 選んだものを覚えて、次はIdleへ回す
	++attackUseCounts_[ToIndex(kind)];
	previousAttack_ = kind;
	hasPreviousAttack_ = true;
	nextIsIdle_ = true;

	return CreateAttack(boss, kind);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  次の攻撃を決める
///////////////////////////////////////////////////////////////////////////////////////////////

BossBehaviorController::AttackKind BossBehaviorController::SelectNextAttack(const Boss& boss) const {

	const BossParameter& parameter = boss.GetParameter();
	const int32_t phase = boss.GetPhaseIndex();

	// HPが減ってフェーズが進むほど、使える攻撃が増えていく
	std::vector<AttackKind> unlocked;
	for (std::size_t i = 0; i < kAttackKindCount; ++i) {
		const AttackKind kind = static_cast<AttackKind>(i);
		if (GetUnlockPhase(parameter, kind) <= phase) {
			unlocked.push_back(kind);
		}
	}

	// 解禁設定を全部1以上にされた場合の保険
	if (unlocked.empty()) {
		return AttackKind::FallFire;
	}

	// 直前と同じ攻撃は繰り返さない。使える攻撃が1種類しか無いフェーズの時だけ許す
	const bool canAvoidRepeat = unlocked.size() > 1;

	// 残った中から使用回数が一番少ないものを候補にする
	std::vector<AttackKind> candidates;
	int32_t fewestCount = (std::numeric_limits<int32_t>::max)();

	for (const AttackKind kind : unlocked) {
		if (canAvoidRepeat && hasPreviousAttack_ && kind == previousAttack_) {
			continue;
		}

		const int32_t count = attackUseCounts_[ToIndex(kind)];
		if (count < fewestCount) {
			// より少ないものが見つかったら候補を入れ替える
			fewestCount = count;
			candidates.clear();
		}
		if (count == fewestCount) {
			candidates.push_back(kind);
		}
	}

	// 使用回数が並んだものの中からランダムに決める
	const int index = Random::RandomInt(0, static_cast<int>(candidates.size()) - 1);
	return candidates[index];
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  攻撃が解禁されるフェーズ
///////////////////////////////////////////////////////////////////////////////////////////////

int32_t BossBehaviorController::GetUnlockPhase(const BossParameter& parameter, AttackKind kind) {

	// 攻撃を増やしたらここにも追加する
	if (kind == AttackKind::Beam) {
		return parameter.beamUnlockPhase;
	}

	if (kind == AttackKind::Stopper) {
		return parameter.stopperUnlockPhase;
	}

	return parameter.fallFireUnlockPhase;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  攻撃の生成
///////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<BaseBossAttackBehavior> BossBehaviorController::CreateAttack(const Boss& boss, AttackKind kind) {

	// 攻撃を増やしたらここにも追加する
	if (kind == AttackKind::Beam) {
		return std::make_unique<BossAttackBeam>();
	}

	if (kind == AttackKind::Stopper) {
		auto stopper = std::make_unique<BossAttackStopper>();
		stopper->SetBlockField(boss.GetBlockField());
		return stopper;
	}

	return std::make_unique<BossAttackFallFire>();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  デバッグ表示
///////////////////////////////////////////////////////////////////////////////////////////////

void BossBehaviorController::Debug_Gui(Boss& boss) {
	ImGui::TextUnformatted("behavior:");
	ImGui::SameLine();
	ImGui::TextUnformatted(GetCurrentName().c_str());

	ImGui::Text("phase: %d", boss.GetPhaseIndex());

	// どの攻撃が何回出たか。偏っていないかの確認用
	ImGui::Text("count: FallFire %d / Beam %d / Stopper %d",
		attackUseCounts_[ToIndex(AttackKind::FallFire)],
		attackUseCounts_[ToIndex(AttackKind::Beam)],
		attackUseCounts_[ToIndex(AttackKind::Stopper)]);

	// 攻撃を単体で確認するために、ここから強制的に切り替えられるようにする
	// 攻撃が増えたらボタンもここに増やす
	if (ImGui::Button("FallFire")) {
		ChangeBehavior(boss, std::make_unique<BossAttackFallFire>());
	}
	ImGui::SameLine();
	if (ImGui::Button("Beam")) {
		ChangeBehavior(boss, std::make_unique<BossAttackBeam>());
	}
	ImGui::SameLine();
	if (ImGui::Button("Stopper")) {
		auto stopper = std::make_unique<BossAttackStopper>();
		stopper->SetBlockField(boss.GetBlockField());
		ChangeBehavior(boss, std::move(stopper));
	}
	ImGui::SameLine();
	if (ImGui::Button("Idle")) {
		ChangeBehavior(boss, std::make_unique<BossAttackIdle>());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  現在の行動名
///////////////////////////////////////////////////////////////////////////////////////////////

const std::string& BossBehaviorController::GetCurrentName() const {
	if (!currentBehavior_) {
		return kNoneName_;
	}
	return currentBehavior_->GetName();
}
