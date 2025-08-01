#include "GpuParticlesData.h"
#include "imgui.h"

void GpuParticleEmitterItem::Attribute_Gui() {
	if (ImGui::CollapsingHeader("Emitter Settings")) {
		ImGui::BulletText("Emitter Setting");
		ImGui::Checkbox("IsLoop", &isLoop);
		ImGui::DragFloat("duration", &duration);
		ImGui::DragFloat3("rotate", &rotate.x, 0.1f);
		ImGui::DragScalar("rateOverTimeCout", ImGuiDataType_U32, &rateOverTimeCout);
		ImGui::DragScalar("emitType", ImGuiDataType_U32, &emitType);
		ImGui::Combo("shape##", &shape, "SPHERE\0BOX\0CONE");
		ImGui::Combo("emitType##type", &emitType, "UP\0ALLDIRE\0OUTSIDE\0CENTER");
		ImGui::Combo("emitOrigin##emitOrigin", &emitOrigin, "CENTER\0RANGE\0EDGE\0OUTSIDE");
		ImGui::DragScalar("shape", ImGuiDataType_U32, &emitOrigin);

		if (shape == GpuEmitterShape::SPHERE) {
			ImGui::DragFloat("radius", &radius, 0.1f);
		} else if (shape == GpuEmitterShape::BOX) {
			ImGui::DragFloat3("size", &size.x, 0.1f);
		} else if (shape == GpuEmitterShape::CONE) {
			ImGui::DragFloat("radius", &radius, 0.1f);
			ImGui::DragFloat("angle", &angle, 0.1f);
			ImGui::DragFloat("height", &height, 0.1f);
		}

		ImGui::Separator();
		ImGui::BulletText("Particle Setting");
		ImGui::ColorEdit4("color", &color.x);
		ImGui::Checkbox("SeparateByAxisScale", &separateByAxisScale);
		if (separateByAxisScale) {
			ImGui::DragFloat3("minScale", &minScale.x, 0.1f);
			ImGui::DragFloat3("maxScale", &maxScale.x, 0.1f);
		} else {
			ImGui::DragFloat("scaleMinScaler", &scaleMinScaler);
			ImGui::DragFloat("scaleMaxScaler", &scaleMaxScaler);
		}
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