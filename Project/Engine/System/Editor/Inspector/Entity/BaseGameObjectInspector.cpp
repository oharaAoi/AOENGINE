#include "BaseGameObjectInspector.h"
#include "Engine/System/Editor/Inspector/Component/ModelInspector.h"
#include "Engine/System/Editor/Inspector/Component/MaterialInspector.h"
#include "Engine/System/Editor/Inspector/Component/ColliderInspector.h"
#include "Engine/System/Editor/Inspector/Component/RigidBodyInspector.h"
#include "Engine/System/Manager/ImGuiManager.h"

namespace {
bool DrawAnimatorRemoveConfirmation() {
	bool openConfirmation = false;
	if (ImGui::BeginPopupContextItem("AnimatorContext")) {
		if (ImGui::MenuItem("Remove Component")) {
			openConfirmation = true;
		}
		ImGui::EndPopup();
	}
	if (openConfirmation) {
		ImGui::OpenPopup("Remove Animator?");
	}

	bool remove = false;
	if (ImGui::BeginPopupModal("Remove Animator?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::TextUnformatted("Remove Animator?");
		ImGui::TextUnformatted("This operation cannot be undone.");
		ImGui::Separator();
		if (ImGui::Button("Remove", ImVec2(120.0f, 0.0f))) {
			remove = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
	return remove;
}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// BaseGameObject全体のInspectorを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::BaseGameObjectInspector::Draw(BaseGameObject& object) {
	bool enableShadow = object.GetEnableShadow();
	if (ImGui::Checkbox("enableShadow", &enableShadow)) {
		object.SetEnableShadow(enableShadow);
	}

	if (object.GetTransform()) {
		object.GetTransform()->Debug_Gui(); // 最初は暫定でOK
	}

	ModelInspector::Draw(object);
	MaterialInspector::Draw(object);
	ColliderInspector::Draw(object);
	RigidBodyInspector::Draw(object);

	if (object.GetAnimator()) {
		const bool isOpen = ImGui::CollapsingHeader("Animator");
		const bool remove = DrawAnimatorRemoveConfirmation();
		if (isOpen) {
			object.GetAnimator()->Debug_Gui();
		}
		if (remove) {
			object.RemoveAnimator();
		}
	}

	ImGui::Separator();
	DrawAddComponent(object);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Component追加メニューを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::BaseGameObjectInspector::DrawAddComponent(BaseGameObject& object) {
	if (ImGui::Button("Add Component")) {
		ImGui::OpenPopup("AddComponentPopup");
	}

	if (ImGui::BeginPopup("AddComponentPopup")) {
		// 物理挙動
		if (ImGui::BeginMenu("Physics")) {
			if (ImGui::MenuItem("Box Collider")) {
				object.SetCollider("Default", ColliderShape::AABB);
			}
			if (ImGui::MenuItem("Sphere Collider")) {
				object.SetCollider("Default", ColliderShape::Sphere);
			}
			if (ImGui::MenuItem("Rigid Body")) {
				object.SetPhysics();
			}
			ImGui::EndMenu();
		}

		// アニメーション
		const bool canAddAnimator = object.GetModel() != nullptr && object.GetAnimator() == nullptr;
		if (ImGui::MenuItem("Animator", nullptr, false, canAddAnimator)) {
			object.SetAnimator();
		}

		ImGui::EndPopup();
	}
}
