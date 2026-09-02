#include "BossParameter.h"

#include <string>

#include "Engine/System/Manager/ImGuiManager.h"

void BossParameter::Debug_Gui() {
	ImGui::DragFloat2("Screen Pos(px)", &screenPos.x, 1.0f);
	ImGui::DragFloat("World Z", &worldZ, 0.1f);

	ImGui::DragFloat("Max HP", &hp, 1.0f);
	ImGui::DragFloat3("Hit Size (half)", &hitSize.x, 0.1f);

	for (std::size_t i = 0; i < phaseSwitchRatio.size(); ++i) {
		const std::string label = "Phase Switch Ratio " + std::to_string(i);
		ImGui::DragFloat(label.c_str(), &phaseSwitchRatio[i], 0.01f, 0.0f, 1.0f);
	}

	// Save / Load ボタン
	SaveAndLoad();
}
