#include "WorldTransform.h"
#include <Lib/Math/MyMatrix.h>

WorldTransform::WorldTransform() {}
WorldTransform::~WorldTransform() {
	Finalize();
}

void WorldTransform::Finalize() {
	cBuffer_.Reset();
	data_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void WorldTransform::Init(ID3D12Device* device) {
	cBuffer_ = CreateBufferResource(device, sizeof(WorldTransformData));
	// データをマップ
	cBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

	// 値を初期化しておく
	scale_ = { 1.0f, 1.0f, 1.0f };
	rotation_ = Quaternion();
	translate_ = { 0.0f, 0.0f, 0.0f };
	worldMat_ = Matrix4x4::MakeUnit();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void WorldTransform::Update(const Matrix4x4& mat) {
	Vector3 worldTranslate = CVector3::ZERO;
	Quaternion worldRotate = Quaternion();

	rotation_ = (rotation_.Normalize() * moveQuaternion_.Normalize());
	rotation_ = rotation_.Normalize();
	moveQuaternion_ = Quaternion();

	// -------------------------------------------------
	// ↓ 平行成分の親子関係があるかを確認
	// -------------------------------------------------
	if (parentTranslate_ != nullptr) {
		worldTranslate = translate_ + *parentTranslate_;
	} else {
		worldTranslate = translate_;
	}

	// -------------------------------------------------
	// ↓ 回転成分の親子関係があるかを確認
	// -------------------------------------------------
	if (parentRotate_ != nullptr) {
		worldRotate = *parentRotate_ * rotation_;
	} else {
		worldRotate = rotation_;
	}

	// -------------------------------------------------
	// ↓ world行列を生成
	// -------------------------------------------------

	transform_.scale = scale_;
	transform_.rotate = worldRotate;
	transform_.translate = worldTranslate;

	worldMat_ = mat * Matrix4x4::MakeAffine(scale_, worldRotate, worldTranslate + temporaryTranslate_);
	if (parentWorldMat_ != nullptr) {
		worldMat_ = worldMat_  * *parentWorldMat_;
	}

	// GPUに送るデータを更新
	data_->matWorld = worldMat_;
	data_->worldInverseTranspose = (worldMat_).Inverse().Transpose();

	temporaryTranslate_ = CVector3::ZERO;
}

void WorldTransform::MoveVelocity(const Vector3& velocity, float rotationSpeed) {
	translate_ += velocity;

	if (velocity.x != 0.0f || velocity.y != 0.0f) {
		Quaternion rotate = Quaternion::LookRotation(velocity.Normalize());
		rotation_ = Quaternion::Slerp(rotation_, rotate, rotationSpeed);
	}
}

void WorldTransform::LookAt(const Vector3& target, const Vector3& up) {
	Vector3 direction = target - translate_;
	rotation_ = Quaternion::LookRotation(direction.Normalize(), up);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　コマンドリストに送る
//////////////////////////////////////////////////////////////////////////////////////////////////

void WorldTransform::BindCommandList(ID3D12GraphicsCommandList* commandList, UINT index) const {
	commandList->SetGraphicsRootConstantBufferView(index, cBuffer_->GetGPUVirtualAddress());
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void WorldTransform::Debug_Gui() {
	if (ImGui::TreeNode("Transform")) {
		if (ImGui::TreeNode("scale")) {
			ImGui::DragFloat3("scale", &scale_.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("rotate")) {
			Debug_Quaternion();
			float norm = std::sqrtf(Quaternion::Dot(rotation_, rotation_));
			ImGui::Text("norm: %f", norm);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("translation")) {
			ImGui::DragFloat3("translation", &translate_.x, 0.1f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

void WorldTransform::Debug_Quaternion() {
	ImGui::DragFloat4("rotation", &rotation_.x, 0.1f);
	ImGui::DragFloat4("moveQuaternion", &moveQuaternion_.x, 0.1f);
	if (ImGui::Button("Reset")) {
		rotation_ = Quaternion();
	}
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Setter系
//////////////////////////////////////////////////////////////////////////////////////////////////

void WorldTransform::SetParent(const Matrix4x4& parentMat) {
	parentWorldMat_ = &parentMat;
}

void WorldTransform::SetParentTranslate(const Vector3& parentTranslate) {
	parentTranslate_ = &parentTranslate;
}

void WorldTransform::SetParentRotate(const Quaternion& parentQuaternion) {
	parentRotate_ = &parentQuaternion;
}

void WorldTransform::SetMatrix(const Matrix4x4& mat) {
	data_->matWorld = mat;
	data_->worldInverseTranspose = Inverse(data_->matWorld).Transpose();
}
