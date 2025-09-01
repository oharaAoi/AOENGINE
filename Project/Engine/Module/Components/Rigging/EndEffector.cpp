#include "EndEffector.h"
#include "Engine/Module/Components/Rigging/InverseKinematics.h"

void EndEffector::Update(const Matrix4x4& _worldMat) {
	// 現在のworld座標を配列に格納
	positions_.clear();
	jointsData_.clear();
	for (int32_t idx : chain_) {
		Matrix4x4 jointWorldMat = Multiply(pSkeleton_->GetJoint(idx).skeltonSpaceMat, _worldMat);
		auto& pos = positions_.emplace_back(jointWorldMat.GetPosition());
		jointsData_.push_back(JointsData(pos, Quaternion(), idx));
	}

	// IKを適応した際の座標を計算
	FABRIK(positions_, targetPos_);

	// 座標計算後の骨の回転を求める
	//for (int32_t i = 0; i < jointsData_.size() - 1; ++i) {
	//	int32_t jointIdx = jointsData_[i].index;
	//	int parentIdx = pSkeleton_->GetJoint(jointIdx).parent.value();

	//	// 親のワールド回転を取得
	//	Quaternion parentRot;
	//	if (parentIdx >= 0) {
	//		Matrix4x4 parentWorld = Multiply(pSkeleton_->GetJoint(parentIdx).skeltonSpaceMat, _worldMat);
	//		parentRot = Quaternion::FromMatrix(parentWorld);
	//	} else {
	//		parentRot = Quaternion(); // identity
	//	}
	//	Quaternion invParentRot = Quaternion::Inverse(parentRot);

	//	// ワールドで計算した方向ベクトル
	//	Vector3 newDireWorld = Normalize(jointsData_[i + 1].pos - jointsData_[i].pos);

	//	// 親のローカル空間に変換
	//	Vector3 newDireLocal = invParentRot * newDireWorld;

	//	// バインド時のデフォルト方向と比較
	//	Vector3 defaultDire = pSkeleton_->GetJoint(jointIdx).defaultDire;
 //		Quaternion rotDelta = Quaternion::FromToRotation(defaultDire, newDireLocal);

	//	jointsData_[i].rotate = rotDelta * pSkeleton_->GetJoint(jointIdx).bindTransform.rotate;
	//}

	// 骨に代入
	for (int32_t i = 0; i < jointsData_.size() - 1; ++i) {
		for (Skeleton::Joint& joint : pSkeleton_->GetJoints()) {
			if (jointsData_[i].index == joint.index) {
				joint.transform.rotate = jointsData_[i].rotate;
			}
		}
	}
}