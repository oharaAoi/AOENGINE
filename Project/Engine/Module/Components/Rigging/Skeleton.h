#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include "Engine/Lib/Math/MyMatrix.h"
#include "Engine/Lib/Math/MathStructures.h"
#include "Engine/Lib/Math/Matrix4x4.h"
#include "Engine/Module/Components/GameObject/Model.h"

namespace AOENGINE {

/// <summary>
/// アニメーションのNodeを階層構造を管理する機構
/// </summary>
class Skeleton {
public:

	/// <summary>
	/// joint情報
	/// </summary>
	struct Joint {
		Math::QuaternionSRT transform;	// transform
		Math::Matrix4x4 localMat;				// localMatrix
		Math::Matrix4x4 skeletonSpaceMat;		// skeletonSpaceでの変換行列
		std::string name;				// 名前
		std::vector<int32_t> children;	// 子のJointのIndexのリスト
		int32_t index;					// 自身のIndex
		std::optional<int32_t> parent;	// 親のJointのIndex,いないとnll
	};

public:

	Skeleton();
	~Skeleton();

public:

	// 初期化
	void Init();
	// 更新
	void Update();
	// 骨の描画
	void DrawBone(const Math::Matrix4x4& worldMat) const;
	// 小骨の描画
	void DrawNodeHierarchy(const Math::Matrix4x4& parentWorldMatrix) const;
	// skeletonの生成
	void CreateSkeleton(const AOENGINE::Model::Node& node);
	// jointの生成
	int32_t CreateJoint(const AOENGINE::Model::Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

public:

	const std::map<std::string, int32_t>& GetJointMap() const { return jointMap_; }

	std::vector<Joint>& GetJoints() { return joints_; }

	const size_t GetJointsSize() const { return joints_.size(); }

	const Joint& GetJoint(uint32_t index) { return joints_[index]; }

	const Math::Matrix4x4& GetSkeletonSpaceMat(const std::string& boenName) { return joints_[jointMap_[boenName]].skeletonSpaceMat; }

private:

	int32_t root_ = 0;	// RootJointのIndex
	std::map<std::string, int32_t> jointMap_;	// Joint名とIndexの辞書
	std::vector<Joint> joints_;	// 所属しているジョイント

	AOENGINE::Model::Node node_;
};

}