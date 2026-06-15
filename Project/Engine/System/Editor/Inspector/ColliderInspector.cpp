#include "ColliderInspector.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include <string>
#include <variant>

namespace {

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

void AOENGINE::ColliderInspector::Draw(BaseGameObject& object) {
	if (!ImGui::CollapsingHeader("Colliders")) {
		return;
	}

	const auto& colliders = object.GetColliders();
	if (colliders.empty()) {
		ImGui::TextUnformatted("No colliders");
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
