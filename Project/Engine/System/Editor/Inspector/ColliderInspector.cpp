#include "ColliderInspector.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include <string>
#include <variant>

namespace {

/// <summary>
/// Colliderのvariant形状から表示名を取得する。
/// </summary>
const char* GetColliderShapeName(const AOENGINE::BaseCollider& collider) {
	const auto& shape = collider.GetShape();
	if (std::holds_alternative<Math::Sphere>(shape)) {
		return "Sphere";
	}
	if (std::holds_alternative<Math::AABB>(shape)) {
		return "AABB";
	}
	if (std::holds_alternative<Math::OBB>(shape)) {
		return "OBB";
	}
	if (std::holds_alternative<Math::Line>(shape)) {
		return "Line";
	}
	return "Unknown";
}

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Collider一覧のInspectorを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::ColliderInspector::Draw(BaseGameObject& object) {
	const auto& colliders = object.GetColliders();
	if (colliders.empty()) {
		return;
	}

	int index = 0;
	for (BaseCollider* collider : colliders) {
		ImGui::PushID(index);

		if (!collider) {
			if (ImGui::TreeNode("null")) {
				ImGui::TreePop();
			}
			ImGui::PopID();
			++index;
			continue;
		}

		std::string label = collider->GetCategoryName();
		if (label.empty()) {
			label = "Collider";
		}
		label += " (";
		label += GetColliderShapeName(*collider);
		label += ")";

		if (ImGui::TreeNode(label.c_str())) {
			collider->Debug_Gui();
			ImGui::TreePop();
		}

		ImGui::PopID();
		++index;
	}
}
