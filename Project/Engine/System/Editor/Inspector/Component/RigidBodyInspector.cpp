#include "RigidBodyInspector.h"
#include "Engine/System/Manager/ImGuiManager.h"

using namespace AOENGINE;

namespace {
bool DrawRemoveConfirmation() {
	bool openConfirmation = false;
	if (ImGui::BeginPopupContextItem("RigidBodyContext")) {
		if (ImGui::MenuItem("Remove Component")) {
			openConfirmation = true;
		}
		ImGui::EndPopup();
	}
	if (openConfirmation) {
		ImGui::OpenPopup("Remove RigidBody?");
	}

	bool remove = false;
	if (ImGui::BeginPopupModal("Remove RigidBody?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::TextUnformatted("Remove RigidBody?");
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
// RigidbodyのInspectorを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::RigidBodyInspector::Draw(BaseGameObject& object) {
	Rigidbody* rigidBody = object.GetRigidbody();
	if (!rigidBody) {
		return;
	}

	const bool isOpen = ImGui::CollapsingHeader("RigidBody");
	const bool remove = DrawRemoveConfirmation();
	if (isOpen) {
		rigidBody->DebugParameters();
	}
	if (remove) {
		object.RemoveRigidbody();
	}
}
