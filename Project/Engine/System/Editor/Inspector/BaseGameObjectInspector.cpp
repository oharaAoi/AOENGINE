#include "BaseGameObjectInspector.h"
#include "ModelInspector.h"
#include "MaterialInspector.h"
#include "ColliderInspector.h"
#include "RigidBodyInspector.h"
#include "Engine/System/Manager/ImGuiManager.h"

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
		if (ImGui::CollapsingHeader("Animator")) {
			object.GetAnimator()->Debug_Gui();
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
		ImGui::EndPopup();
	}
}
