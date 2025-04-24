#include "ParticlesData.h"
#include "imgui.h"

#ifdef _DEBUG
void ParticleEmit::Attribute_Gui() {
	ImGui::Text("Emitter Settings");
	ImGui::DragFloat4("Rotate (Quaternion)", (float*)&rotate, 0.01f);
	ImGui::DragFloat3("Translate", (float*)&translate, 0.1f);
	ImGui::DragFloat3("Direction", (float*)&direction, 0.1f);
	ImGui::DragScalar("Shape", ImGuiDataType_U32, &shape, 1.0f);
	ImGui::DragScalar("Count", ImGuiDataType_U32, &count, 1.0f);
	ImGui::DragFloat("Frequency", &frequency, 0.01f);
	ImGui::DragFloat("Frequency Time", &frequencyTime, 0.01f);
	ImGui::DragInt("Emit", &emit, 1.0f);

	ImGui::Separator();
	ImGui::Text("Particle Parameters");
	ImGui::ColorEdit4("Color", (float*)&color); // 色はColorEditの方が視覚的に便利
	ImGui::DragFloat3("Min Scale", (float*)&minScale, 0.01f);
	ImGui::DragFloat3("Max Scale", (float*)&maxScale, 0.01f);
	ImGui::DragFloat("Speed", &speed, 0.01f);
	ImGui::DragFloat("Lifetime", &lifeTime, 0.01f);
	ImGui::DragFloat("Gravity", &gravity, 0.01f);
	ImGui::DragFloat("Damping", &dampig, 0.01f); // ← 変数名のスペル注意
}
#endif