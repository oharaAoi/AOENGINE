#include "ScreenTransform.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Render.h"
#include "Engine/System/Editer/Tool/ManipulateTool.h"

int ScreenTransform::nextId_ = 0;

ScreenTransform::ScreenTransform() {
	id_ = nextId_;
	nextId_++;
}

ScreenTransform::~ScreenTransform() {
	transformBuffer_.Reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ScreenTransform::Init(ID3D12Device* _pDevice) {
	transformBuffer_ = CreateBufferResource(_pDevice, sizeof(TransformData));
	transformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&transformData_));

	transform_ = { {1.0f,1.0f,1.0f} , {0.0f, 0.0f, 0.0f}, {0, 0, 1} };
	parentMat_ = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ScreenTransform::Update(const Matrix4x4& _projection) {
	// 行列の作成
	screenMat_ = transform_.MakeAffine();

	// 親がいる場合親を考慮
	Matrix4x4 matrix = screenMat_;
	if (parentMat_ != nullptr) {
		matrix = screenMat_ * *parentMat_;
	}

	// 最終的なスプライトの変換行列
	transformData_->wvp = Matrix4x4(
		matrix *
		_projection
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ バインド処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ScreenTransform::BindCommand(ID3D12GraphicsCommandList* _cmd, uint32_t index) {
	_cmd->SetGraphicsRootConstantBufferView(index, transformBuffer_->GetGPUVirtualAddress());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Guiで操作する
///////////////////////////////////////////////////////////////////////////////////////////////

void ScreenTransform::Manipulate(const ImVec2& windowSize, const ImVec2& imagePos) {
	ImGuizmo::PushID(id_);
	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList()); // ←画面全体描画リスト
	ImGuizmo::SetRect(imagePos.x, imagePos.y, windowSize.x, windowSize.y);

	Matrix4x4 viewMat = Render::GetViewport2D();
	Matrix4x4 projectMat = Render::GetProjection2D();

	float view[16];
	float proj[16];
	float world[16];

	memcpy(view, &viewMat, sizeof(view));
	memcpy(proj, &projectMat, sizeof(proj));
	memcpy(world, &screenMat_, sizeof(world));

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
		memcpy(&screenMat_, world, sizeof(world));
		Vector3 scale = screenMat_.GetScale();
		Vector3 translate = screenMat_.GetPosition();
		transform_.scale = scale;
		transform_.translate = translate;
	}

	ImGuizmo::PopID();
}

void ScreenTransform::Debug_Gui() {
	if (ImGui::TreeNode("transform")) {
		ImGui::DragFloat3("translation", &transform_.translate.x, 0.1f);
		ImGui::DragFloat2("scale", &transform_.scale.x, 0.01f);
		ImGui::SliderAngle("rotation", &transform_.rotate.z);
		ImGui::TreePop();
	}
}

void ScreenTransform::SetParent(const Matrix4x4& _parentMat) {
	parentMat_ = &_parentMat;
}
