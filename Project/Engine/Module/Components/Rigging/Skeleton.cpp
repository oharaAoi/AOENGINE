#include "Skeleton.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Render.h"
#include "Engine/Utilities/DrawUtils.h"
#ifdef DEBUG
#include "Engine/Manager/ImGuiManager.h"
#endif

Skeleton::Skeleton() {}
Skeleton::~Skeleton() {}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Skeleton::Init() {
	//for (size_t oi = 0; oi < joints_.size(); ++oi) {
	//	auto& instance = obj_.emplace_back(std::make_unique<BaseGameObject>());
	//	instance->Init();
	//	instance->SetObject("SquarePyramid.obj");
	//}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Skeleton::Update() {
	for (Joint& joint : joints_) {
		joint.localMat = Matrix4x4::MakeAffine(joint.transform.scale, joint.transform.rotate.Normalize(), joint.transform.translate);

		if (joint.parent) {
			joint.skeltonSpaceMat = joint.localMat * joints_[*joint.parent].skeltonSpaceMat;
		} else {
			joint.skeltonSpaceMat = joint.localMat;
		}
	}
}

void Skeleton::DrawBone(const Matrix4x4& worldMat) const {
	for (const Joint& joint : joints_) {
		Vector3 pos = (joint.skeltonSpaceMat * worldMat).GetPosition();
		DrawSphere(pos, 0.2f, Render::GetViewProjectionMat(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	DrawNodeHierarchy(worldMat);
}

void Skeleton::DrawNodeHierarchy(const Matrix4x4& parentWorldMatrix) const {
	for (const Joint& joint : joints_) {
		Vector3 parentPos = (joint.skeltonSpaceMat * parentWorldMatrix).GetPosition();

		for (int32_t childIndex : joint.children) {
			const Joint& child = joints_[childIndex];
			Vector3 childPos = (child.skeltonSpaceMat * parentWorldMatrix).GetPosition();
			// 線を引く
			Render::DrawLine(parentPos, childPos, Vector4(1.0f, 0.0f, 0.0f, 1.0f), Render::GetViewProjectionMat());
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　skeletonの作成
//////////////////////////////////////////////////////////////////////////////////////////////////

void Skeleton::CreateSkeleton(const Model::Node& node) {
	root_ = CreateJoint(node, {}, joints_);
	node_ = node;

	// 名前からIndexを検索可能に
	for (const Joint& joint : joints_) {
		jointMap_.emplace(joint.name, joint.index);
	}

	//Update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　再帰的にbornの情報を取り込む
//////////////////////////////////////////////////////////////////////////////////////////////////

int32_t Skeleton::CreateJoint(const Model::Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
	Joint joint;
	joint.name = node.name;
	joint.localMat = node.localMatrix;
	joint.skeltonSpaceMat = Matrix4x4::MakeUnit();
	joint.transform = node.transform;
	joint.index = int32_t(joints_.size());	// 登録されている数
	joint.parent = parent;
	joints.push_back(std::move(joint));	// skeltonのjoint列に追加
	for (const Model::Node& child : node.children) {
		// 子のjointを作成し、そのIndexを登録
		int32_t chileIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(chileIndex);
	}

	return joint.index;
}

