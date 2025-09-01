#pragma once
#include <vector>
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Module/Components/Rigging/Skeleton.h"

class EndEffector {
public:

	struct JointsData {
		Vector3& pos;
		Quaternion rotate;
		int32_t index;
	};

public:

	EndEffector() = default;
	~EndEffector() = default;

	void Update(const Matrix4x4& _worldMat);

public:

	void SetChain(const std::vector<int32_t>& _chain) { chain_ = _chain; }

	void SetSkelton(Skeleton* _skelton) { pSkeleton_ = _skelton; }

	void SetTargetPosition(const Vector3& _pos) { targetPos_ = _pos; }

private:

	Skeleton* pSkeleton_;
	std::vector<int32_t> chain_;
	std::vector<Vector3> positions_;
	std::vector<JointsData> jointsData_;

	Vector3 targetPos_;

};

