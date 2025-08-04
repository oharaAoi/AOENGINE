#include "ParticlesData.h"
#include "imgui.h"

void ParticleEmit::Attribute_Gui() {
	if (ImGui::CollapsingHeader("Emitter Settings")) {
		ImGui::Checkbox("IsLoop", &isLoop);
		ImGui::DragFloat("duration", &duration);
		ImGui::Checkbox("isBillBord", &isBillBord);
		ImGui::BulletText("Emitter Settings");
		ImGui::DragFloat4("Rotate (Quaternion)", (float*)&rotate, 0.01f);
		ImGui::DragFloat("rotateAngle", (float*)&rotateAngle, 0.01f);
		ImGui::DragFloat3("Translate", (float*)&translate, 0.1f);
		ImGui::DragFloat3("Direction", (float*)&direction, 0.1f);
		ImGui::DragScalar("Shape", ImGuiDataType_U32, &shape, 1.0f);
		ImGui::DragScalar("RateOverTimeCout", ImGuiDataType_U32, &rateOverTimeCout, 1.0f);
		
		ImGui::Combo("emitType##type", &emitType, "UP\0ALLDIRE\0CENTER");
		ImGui::Combo("emitOrigin##emitOrigin", &emitOrigin, "CENTER\0RANGE\0OUTSIDE");
		
		ImGui::DragFloat("radius", &radius, 0.1f);

		direction.x = std::clamp(direction.x, -1.0f, 1.0f);
		direction.y = std::clamp(direction.y, -1.0f, 1.0f);
		direction.z = std::clamp(direction.z, -1.0f, 1.0f);
		ImGui::Separator();

		ImGui::BulletText("Particle Parameters");
		ImGui::ColorEdit4("Color", (float*)&color); // 色はColorEditの方が視覚的に便利
		ImGui::Text("2way Random");
		ImGui::DragFloat3("Min Scale", (float*)&minScale, 0.01f);
		ImGui::DragFloat3("Max Scale", (float*)&maxScale, 0.01f);
		ImGui::Text("Parameter");
		ImGui::DragFloat("Speed", &speed, 0.01f);
		ImGui::DragFloat("Lifetime", &lifeTime, 0.01f);
		ImGui::DragFloat("Gravity", &gravity, 0.01f);
		ImGui::DragFloat("Damping", &dampig, 0.01f);
		ImGui::Text("First Angle");
		ImGui::DragFloat("angleMin", &angleMin, 0.01f);
		ImGui::DragFloat("angleMax", &angleMax, 0.01f);
		ImGui::Text("Life of");
		ImGui::Checkbox("isDirectionRotate", &isDirectionRotate);
		ImGui::Checkbox("isLifeOfScale", &isLifeOfScale);
		ImGui::Checkbox("isLifeOfAlpha", &isLifeOfAlpha);

		ImGui::Checkbox("StrechBillbard", &stretchBillboard);
		if (stretchBillboard) {
			ImGui::DragFloat("stretchScale", &stretchScale, 0.1f);
		}

		ImGui::Checkbox("isParticleAddBlend", &isParticleAddBlend);
		ImGui::Checkbox("isScaleUp", &isScaleUp);
		ImGui::DragFloat3("scaleUpScale", (float*)&scaleUpScale, 0.01f);

		minScale.Clamp(minScale,maxScale );
	}
}