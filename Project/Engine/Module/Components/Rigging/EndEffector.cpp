#include "EndEffector.h"
#include "Engine/Module/Components/Rigging/InverseKinematics.h"

void EndEffector::Update(const Math::Matrix4x4& _worldMat) {
	// 現在のworld座標を配列に格納
	positions_.clear();
	jointsData_.clear();
	for (int32_t idx : chain_) {
		Math::Matrix4x4 jointWorldMat = Multiply(pSkeleton_->GetJoint(idx).skeltonSpaceMat, _worldMat);
		auto& pos = positions_.emplace_back(jointWorldMat.GetPosition());
		jointsData_.push_back(JointsData(pos, Math::Quaternion(), idx));
	}

	// IKを適応した際の座標を計算
	FABRIK(positions_, targetPos_);

	// 骨に代入
	for (int32_t i = 0; i < jointsData_.size() - 1; ++i) {
		for (Skeleton::Joint& joint : pSkeleton_->GetJoints()) {
			if (jointsData_[i].index == joint.index) {
				joint.transform.rotate = jointsData_[i].rotate;
			}
		}
	}
}