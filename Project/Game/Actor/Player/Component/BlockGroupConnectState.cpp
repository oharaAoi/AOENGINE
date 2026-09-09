#include "BlockGroupConnectState.h"

#include <algorithm>
#include <array>

#include "Engine/Core/Engine.h"

#include "Engine/System/Input/Input.h"

using namespace AOENGINE;

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
	if (remainingTime_ > 0.0f && !context.launchTriggered) {
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

	// 集合地点は最後に接続したグループへ乗った場所。
	// 手前のグループはそこへ順に集まってきて、最後のグループは動かずにその場で待つ
	gatherPoint_ = connectedGroups_.back().connectPosition;

	launchTimer_ = params_.launchWaitTime;
	phase_ = Phase::Gathering;
	gatherStarted_ = true;
}

void BlockGroupConnectState::UpdateGathering(float deltaTime, const Context& context) {
	launchTimer_ -= deltaTime;

	// 専用タイマーが尽きるか、打ち上げ入力が来たら打ち上げる
	if (launchTimer_ > 0.0f) {
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

bool BlockGroupConnectState::DiscardConnectedGroups(int groupId) {
	// 集合(Gathering)以降は既に段から切り離されているため対象外。
	// 接続受付中(Connecting)だけが、まだ壊される可能性のあるグループを繋いでいる
	if (phase_ != Phase::Connecting) {
		return false;
	}

	const bool contains = std::any_of(connectedGroups_.begin(), connectedGroups_.end(),
		[groupId](const ConnectedGroup& group) { return group.groupId == groupId; });
	if (!contains) {
		return false;
	}

	// remainingTime_ / launchTimer_ / phase_ は動かさない。
	// 同じ受付時間の中でコンボを0から数え直させるための処理なので、受付自体は続ける
	connectedGroups_.clear();
	return true;
}

const std::string& BlockGroupConnectState::GetPhaseName() const {
	static const std::array<std::string, 3> kNames = {
		"Idle", "Connecting", "Gathering",
	};
	return kNames[static_cast<std::size_t>(phase_)];
}
