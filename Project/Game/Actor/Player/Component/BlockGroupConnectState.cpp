#include "BlockGroupConnectState.h"

void BlockGroupConnectState::Begin(float connectableTime) {
	groupIds_.clear();
	remainingTime_ = connectableTime;
}

void BlockGroupConnectState::Update(float deltaTime) {
	remainingTime_ -= deltaTime;
	if (remainingTime_ < 0.0f) {
		remainingTime_ = 0.0f;
	}
	// TODO: 受付終了時に groupIds_ を StageBlockField へ渡してグループを統合する
}

bool BlockGroupConnectState::TryAdd(int groupId) {
	if (!CanConnect()) {
		return false;
	}
	if (groupId < 0) {
		return false;
	}
	for (int id : groupIds_) {
		if (id == groupId) {
			return false;
		}
	}
	groupIds_.push_back(groupId);
	return true;
}

void BlockGroupConnectState::Clear() {
	remainingTime_ = 0.0f;
	groupIds_.clear();
}
