#include "WorldTransform.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include <Lib/Math/MyMatrix.h>
#include "Engine/Render.h"
#include "Engine/System/Editer/Tool/ManipulateTool.h"
#include "Engine/Lib/GameTimer.h"

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
	srt_.rotate = Math::Quaternion();
	srt_.translate = { 0.0f, 0.0f, 0.0f };
	worldMat_ = Math::Matrix4x4::MakeUnit();

	moveQuaternion_ = Math::Quaternion();
	isBillboard_ = false;

	data_->matWorldPrev = Math::Matrix4x4::MakeUnit();
	data_->matWorld = Math::Matrix4x4::MakeUnit();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void WorldTransform::Update(const Math::Matrix4x4& mat) {
	Math::Vector3 worldTranslate = CVector3::ZERO;
	Math::Quaternion worldRotate = Math::Quaternion();

	srt_.rotate = moveQuaternion_ * srt_.rotate;
	srt_.rotate = srt_.rotate.Normalize();

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

	Math::Matrix4x4 scaleMat = srt_.scale.MakeScaleMat();
	Math::Matrix4x4 rotateMat = worldRotate.MakeMatrix();
	if (isBillboard_) {
		rotateMat = Multiply(rotateMat, AOENGINE::Render::GetBillBordMat());
	}
	Math::Matrix4x4 translateMat = Math::Vector3(worldTranslate + temporaryTranslate_).MakeTranslateMat();

	worldMat_ = mat * Multiply(Multiply(scaleMat, rotateMat), translateMat);
	if (parentWorldMat_ != nullptr) {
		worldMat_ = worldMat_ * *parentWorldMat_;
	}

	// GPUに送るデータを更新
	data_->matWorld = worldMat_;
	data_->worldInverseTranspose = (worldMat_).Inverse().Transpose();

	preTranslate_ = srt_.translate + temporaryTranslate_;
	temporaryTranslate_ = CVector3::ZERO;
	moveQuaternion_ = Math::Quaternion();
}

void WorldTransform::PostUpdate() {
	data_->matWorldPrev = worldMat_;
}

void WorldTransform::MoveVelocity(const Math::Vector3& velocity, float rotationSpeed) {
	srt_.translate += velocity;

	if (velocity.x != 0.0f || velocity.y != 0.0f) {
		Math::Quaternion rotate = Math::Quaternion::LookRotation(velocity.Normalize());
		srt_.rotate = Math::Quaternion::Slerp(srt_.rotate, rotate, rotationSpeed);
	}
}

void WorldTransform::LookAt(const Math::Vector3& target, const Math::Vector3& up) {
	Math::Vector3 direction = target - srt_.translate;
	srt_.rotate = Math::Quaternion::LookRotation(direction.Normalize(), up);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　コマンドリストに送る
//////////////////////////////////////////////////////////////////////////////////////////////////

void WorldTransform::BindCommandList(ID3D12GraphicsCommandList* commandList, UINT index) const {
	commandList->SetGraphicsRootConstantBufferView(index, cBuffer_->GetGPUVirtualAddress());
}

void WorldTransform::Translate(const Math::Vector3& translate, float _deltaTime) {
	srt_.translate += translate * _deltaTime;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集
//////////////////////////////////////////////////////////////////////////////////////////////////

void WorldTransform::Debug_Gui() {
	const std::string id = "Transform##id" + std::to_string(id_);
	if (ImGui::CollapsingHeader(id.c_str())) {
		if (ImGui::TreeNodeEx("scale", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::DragFloat3("scale", &srt_.scale.x, 0.01f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("rotate", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::DragFloat3("rotate", &srt_.rotate.x, 0.1f);
			ImGui::DragFloat3("moveRotate", &moveQuaternion_.x, 0.001f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("translation", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::DragFloat3("translation", &srt_.translate.x, 0.1f);
			ImGui::TreePop();
		}
	}
}

void WorldTransform::Manipulate(const ImVec2& windowSize, const ImVec2& imagePos) {
	ImGuizmo::PushID(id_);
	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList()); // ←画面全体描画リスト
	ImGuizmo::SetRect(imagePos.x, imagePos.y, windowSize.x, windowSize.y);

	Math::Matrix4x4 viewMat = AOENGINE::Render::GetViewport3D();
	Math::Matrix4x4 projectMat = Math::Matrix4x4::MakePerspectiveFov(0.45f, float(windowSize.x) / float(windowSize.y), 0.1f, 100.0f);

	float view[16];
	float proj[16];
	float world[16];

	memcpy(view, &viewMat, sizeof(view));
	memcpy(proj, &projectMat, sizeof(proj));
	memcpy(world, &worldMat_, sizeof(world));

	if (ManipulateTool::type_ == UseManipulate::Scale) {
		ImGuizmo::Manipulate(view, proj, ImGuizmo::SCALE, ImGuizmo::LOCAL, world);
	}

	if (ManipulateTool::type_ == UseManipulate::Rotate) {
		ImGuizmo::Manipulate(view, proj, ImGuizmo::ROTATE, ImGuizmo::LOCAL, world);
	}

	if (ManipulateTool::type_ == UseManipulate::Translate) {
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

void WorldTransform::SetParent(const Math::Matrix4x4& parentMat) {
	parentWorldMat_ = &parentMat;
}

void WorldTransform::SetParentTranslate(const Math::Vector3& parentTranslate) {
	parentTranslate_ = &parentTranslate;
}

void WorldTransform::SetParentRotate(const Math::Quaternion& parentQuaternion) {
	parentRotate_ = &parentQuaternion;
}

void WorldTransform::SetMatrix(const Math::Matrix4x4& mat) {
	data_->matWorld = mat;
	data_->worldInverseTranspose = Inverse(data_->matWorld).Transpose();
}
