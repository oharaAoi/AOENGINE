#include "BlockGroupConnectState.h"

#include <array>

void BlockGroupConnectState::Begin() {
	// 集合中に再びジャンプしても受付は始めない(打ち上げが終わるまで1セットとして扱う)
	if (phase_ != Phase::Idle) {
		return;
	}

	connectedGroups_.clear();

	remainingTime_ = params_.connectableTime;
	launchTimer_ = 0.0f;
	phase_ = Phase::Connecting;
}

void BlockGroupConnectState::Update(float deltaTime, const Context& context, const Params& params) {
	params_ = params;

	// 1フレームだけ立てる通知はここで落とす
	gatherStarted_ = false;
	launchRequested_ = false;

	// TryAdd()は衝突判定側から呼ばれ位置を持たないため、ここで拾っておく
	lastPlayerPosition_ = context.playerPosition;

	// 足場に乗っている間は集合地点の候補を更新し続ける
	if (context.isGrounded) {
		lastGroundedPosition_ = context.playerPosition;
		hasGroundedPosition_ = true;
	}

	switch (phase_) {
	case Phase::Connecting:
		UpdateConnecting(deltaTime, context);
		break;
	case Phase::Gathering:
		UpdateGathering(deltaTime, context);
		break;
	case Phase::Idle:
	default:
		break;
	}
}

void BlockGroupConnectState::UpdateConnecting(float deltaTime, const Context& context) {
	remainingTime_ -= deltaTime;
	if (remainingTime_ > 0.0f) {
		return;
	}

	remainingTime_ = 0.0f;
	BeginGather(context);
}

void BlockGroupConnectState::BeginGather(const Context& context) {
	// 1グループも接続できていない場合は集めるものが無いので待機に戻す
	if (connectedGroups_.empty()) {
		Clear();
		return;
	}

	// 集合地点はプレイヤーが最後に乗っていた場所。一度も接地していなければ現在地で代用する
	gatherPoint_ = hasGroundedPosition_ ? lastGroundedPosition_ : context.playerPosition;

	launchTimer_ = params_.launchWaitTime;
	phase_ = Phase::Gathering;
	gatherStarted_ = true;
}

void BlockGroupConnectState::UpdateGathering(float deltaTime, const Context& context) {
	launchTimer_ -= deltaTime;

	// 専用タイマーが尽きるか、打ち上げ入力が来たら打ち上げる
	if (!context.launchTriggered && launchTimer_ > 0.0f) {
		return;
	}

	launchTimer_ = 0.0f;
	launchRequested_ = true;

	// 打ち上げ後は次のジャンプからまた接続を受け付ける
	Clear();
}

bool BlockGroupConnectState::TryAdd(int groupId) {
	if (!CanConnect()) {
		return false;
	}
	if (groupId < 0) {
		return false;
	}
	for (const ConnectedGroup& group : connectedGroups_) {
		if (group.groupId == groupId) {
			return false;
		}
	}

	// このグループへ乗った位置を覚えておく。集合時はこの点を順に結んだ道を通らせる
	ConnectedGroup connected{};
	connected.groupId = groupId;
	connected.connectPosition = lastPlayerPosition_;
	connectedGroups_.push_back(connected);
	return true;
}

void BlockGroupConnectState::Clear() {
	phase_ = Phase::Idle;
	remainingTime_ = 0.0f;
	launchTimer_ = 0.0f;
	connectedGroups_.clear();
}

const std::string& BlockGroupConnectState::GetPhaseName() const {
	static const std::array<std::string, 3> kNames = {
		"Idle", "Connecting", "Gathering",
	};
	return kNames[static_cast<std::size_t>(phase_)];
}
