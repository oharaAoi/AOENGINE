#include "WorldTransform.h"
#include "ImGuizmo.h"
#include <Lib/Math/MyMatrix.h>
#include "Engine/Render.h"
#include "Engine/System/Editer/Tool/ManipulateTool.h"

int WorldTransform::nextId_ = 0;

WorldTransform::WorldTransform() {
	id_ = nextId_;
	nextId_++;
}
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
	srt_.scale = { 1.0f, 1.0f, 1.0f };
	srt_.rotate = Quaternion();
	srt_.translate = { 0.0f, 0.0f, 0.0f };
	worldMat_ = Matrix4x4::MakeUnit();

	moveQuaternion_ = Quaternion();
	isBillboard_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void WorldTransform::Update(const Matrix4x4& mat) {
	Vector3 worldTranslate = CVector3::ZERO;
	Quaternion worldRotate = Quaternion();

	srt_.rotate = moveQuaternion_ * srt_.rotate;
	srt_.rotate = srt_.rotate.Normalize();

	data_->matWorldPrev = mat * Matrix4x4::MakeAffine(srt_.scale, srt_.rotate, preTranslate_ + temporaryTranslate_);
	if (parentWorldMat_ != nullptr) {
		data_->matWorldPrev = data_->matWorldPrev * *parentWorldMat_;
	}

	// -------------------------------------------------
	// ↓ 平行成分の親子関係があるかを確認
	// -------------------------------------------------
	if (parentTranslate_ != nullptr) {
		worldTranslate = srt_.translate + *parentTranslate_;
	} else {
		worldTranslate = srt_.translate;
	}

	// -------------------------------------------------
	// ↓ 回転成分の親子関係があるかを確認
	// -------------------------------------------------
	if (parentRotate_ != nullptr) {
		worldRotate = *parentRotate_ * srt_.rotate;
	} else {
		worldRotate = srt_.rotate;
	}

	// -------------------------------------------------
	// ↓ world行列を生成
	// -------------------------------------------------

	Matrix4x4 scaleMat = srt_.scale.MakeScaleMat();
	Matrix4x4 rotateMat = worldRotate.MakeMatrix();
	if (isBillboard_) {
		rotateMat = Multiply(rotateMat, Render::GetBillBordMat());
	}
	Matrix4x4 translateMat = Vector3(worldTranslate + temporaryTranslate_).MakeTranslateMat();

	worldMat_ = mat * Multiply(Multiply(scaleMat, rotateMat), translateMat);
	if (parentWorldMat_ != nullptr) {
		worldMat_ = worldMat_ * *parentWorldMat_;
	}

	// GPUに送るデータを更新
	data_->matWorld = worldMat_;
	data_->worldInverseTranspose = (worldMat_).Inverse().Transpose();

	preTranslate_ = srt_.translate + temporaryTranslate_;
	temporaryTranslate_ = CVector3::ZERO;
	moveQuaternion_ = Quaternion();
}

void WorldTransform::MoveVelocity(const Vector3& velocity, float rotationSpeed) {
	srt_.translate += velocity;

	if (velocity.x != 0.0f || velocity.y != 0.0f) {
		Quaternion rotate = Quaternion::LookRotation(velocity.Normalize());
		srt_.rotate = Quaternion::Slerp(srt_.rotate, rotate, rotationSpeed);
	}
}

void WorldTransform::LookAt(const Vector3& target, const Vector3& up) {
	Vector3 direction = target - srt_.translate;
	srt_.rotate = Quaternion::LookRotation(direction.Normalize(), up);
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

void WorldTransform::Debug_Gui() {
	if (ImGui::CollapsingHeader("Transform")) {
		if (ImGui::TreeNode("scale")) {
			ImGui::DragFloat3("scale", &srt_.scale.x, 0.01f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("rotate")) {
			ImGui::DragFloat3("rotate", &srt_.rotate.x, 0.1f);
			ImGui::DragFloat3("moveRotate", &moveQuaternion_.x, 0.001f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("translation")) {
			ImGui::DragFloat3("translation", &srt_.translate.x, 0.1f);
			ImGui::TreePop();
		}
	}
}

void WorldTransform::Manipulate(const ImVec2& windowSize, const ImVec2& imagePos) {
	ImGuizmo::PushID(id_);
	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList()); // ←画面全体描画リスト
	ImGuizmo::SetRect(imagePos.x, imagePos.y, windowSize.x, windowSize.y);

	Matrix4x4 viewMat = Render::GetViewport3D();
	Matrix4x4 projectMat = Matrix4x4::MakePerspectiveFov(0.45f, float(windowSize.x) / float(windowSize.y), 0.1f, 100.0f);

	float view[16];
	float proj[16];
	float world[16];

	memcpy(view, &viewMat, sizeof(view));
	memcpy(proj, &projectMat, sizeof(proj));
	memcpy(world, &worldMat_, sizeof(world));

	if (ManipulateTool::type_ == UseManipulate::SCALE) {
		ImGuizmo::Manipulate(view, proj, ImGuizmo::SCALE, ImGuizmo::LOCAL, world);
	}

	if (ManipulateTool::type_ == UseManipulate::ROTATE) {
		ImGuizmo::Manipulate(view, proj, ImGuizmo::ROTATE, ImGuizmo::LOCAL, world);
	}

	if (ManipulateTool::type_ == UseManipulate::TRANSLATE) {
		ImGuizmo::Manipulate(view, proj, ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, world);
	}

	if (ImGuizmo::IsUsing()) {
		memcpy(&worldMat_, world, sizeof(world));
		srt_.scale = worldMat_.GetScale();
		srt_.rotate = worldMat_.GetRotate();
		//srt_.translate = worldMat_.GetPosition();
	}

	ImGuizmo::PopID();
}

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
