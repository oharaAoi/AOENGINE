#include "GpuParticlesData.h"
#include "imgui.h"

void GpuParticleEmitterItem::Attribute_Gui() {
	if (ImGui::CollapsingHeader("Emitter Settings")) {
		ImGui::Checkbox("IsLoop", &isLoop);
		ImGui::DragFloat("duration", &duration);
		ImGui::Checkbox("SeparateByAxisScale", &separateByAxisScale);
		if (separateByAxisScale) {
			ImGui::DragFloat3("minScale", &minScale.x, 0.1f);
			ImGui::DragFloat3("maxScale", &maxScale.x, 0.1f);
		} else {
			ImGui::DragFloat("scaleMinScaler", &scaleMinScaler);
			ImGui::DragFloat("scaleMaxScaler", &scaleMaxScaler);
		}
		ImGui::DragFloat3("rotate", &rotate.x, 0.1f);
		ImGui::DragScalar("rateOverTimeCout", ImGuiDataType_U32, &rateOverTimeCout);
		ImGui::DragScalar("emitType", ImGuiDataType_U32, &emitType);
		ImGui::DragScalar("shape", ImGuiDataType_U32, &shape);
		ImGui::ColorEdit4("color", &color.x);
		ImGui::DragFloat("speed", &speed, 0.1f);
		ImGui::DragFloat("lifeTime", &lifeTime, 0.1f);
		ImGui::DragFloat("gravity", &gravity, 0.1f);
		ImGui::DragFloat("damping", &damping, 0.1f);
		ImGui::Checkbox("lifeOfScaleDown", &lifeOfScaleDown);
		ImGui::Checkbox("lifeOfScaleUp", &lifeOfScaleUp);
		if (lifeOfScaleUp) {
			ImGui::DragFloat3("targetScale", &targetScale.x, 0.1f);
		}
		ImGui::Checkbox("lifeOfAlpha", &lifeOfAlpha);


	}
}