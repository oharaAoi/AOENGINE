#include "Skeleton.h"
#include "Engine/Render.h"
#include "Engine/Utilities/DrawUtils.h"
#include "Engine/Lib/Color.h"

using namespace AOENGINE;

Skeleton::Skeleton() {}
Skeleton::~Skeleton() {}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Skeleton::Init() {
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Skeleton::Update() {
	for (Joint& joint : joints_) {
		joint.localMat = Math::Matrix4x4::MakeAffine(joint.transform.scale, joint.transform.rotate.Normalize(), joint.transform.translate);

		if (joint.parent) {
			joint.skeltonSpaceMat = joint.localMat * joints_[*joint.parent].skeltonSpaceMat;
		} else {
			joint.skeltonSpaceMat = joint.localMat;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Skeleton::DrawBone(const Math::Matrix4x4& worldMat) const {
	for (const Joint& joint : joints_) {
		Math::Vector3 pos = (joint.skeltonSpaceMat * worldMat).GetPosition();
		DrawSphere(pos, 0.2f, AOENGINE::Render::GetViewProjectionMat(), Colors::Linear::red);
	}

	DrawNodeHierarchy(worldMat);
}

void Skeleton::DrawNodeHierarchy(const Math::Matrix4x4& parentWorldMatrix) const {
	for (const Joint& joint : joints_) {
		Math::Vector3 parentPos = (joint.skeltonSpaceMat * parentWorldMatrix).GetPosition();

		for (int32_t childIndex : joint.children) {
			const Joint& child = joints_[childIndex];
			Math::Vector3 childPos = (child.skeltonSpaceMat * parentWorldMatrix).GetPosition();
			// 線を引く
			AOENGINE::Render::DrawLine(parentPos, childPos, Colors::Linear::red, AOENGINE::Render::GetViewProjectionMat());
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　skeletonの作成
//////////////////////////////////////////////////////////////////////////////////////////////////

void Skeleton::CreateSkeleton(const AOENGINE::Model::Node& node) {
	root_ = CreateJoint(node, {}, joints_);
	node_ = node;

	// 名前からIndexを検索可能に
	for (const Joint& joint : joints_) {
		jointMap_.emplace(joint.name, joint.index);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　再帰的にbornの情報を取り込む
//////////////////////////////////////////////////////////////////////////////////////////////////

int32_t Skeleton::CreateJoint(const AOENGINE::Model::Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
	Joint joint;
	joint.name = node.name;
	joint.localMat = node.localMatrix;
	joint.skeltonSpaceMat = Math::Matrix4x4::MakeUnit();
	joint.transform = node.transform;
	joint.index = int32_t(joints_.size());	// 登録されている数
	joint.parent = parent;
	joints.push_back(std::move(joint));	// skeltonのjoint列に追加
	for (const AOENGINE::Model::Node& child : node.children) {
		// 子のjointを作成し、そのIndexを登録
		int32_t chileIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(chileIndex);
	}

	return joint.index;
}

