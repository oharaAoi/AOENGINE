#include "RigidBodyInspector.h"

using namespace AOENGINE;

//////////////////////////////////////////////////////////////////////////////////////////////////
// RigidbodyのInspectorを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::RigidBodyInspector::Draw(BaseGameObject& object) {
	if (!ImGui::CollapsingHeader("RigidBody")) {
		return;
	}

	const auto& rigidBody = object.GetRigidbody();
	if (rigidBody) {
		rigidBody->Debug_Gui();
	}
}
