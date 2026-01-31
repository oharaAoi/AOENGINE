#include "BaseEnemy.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseEnemy::Debug_Gui() {
	ImGui::Checkbox("カメラに写っているかどうか", &isWithinSight_);
	object_->Debug_Gui();
	ImGui::Separator();
}

void BaseEnemy::BaseParameter::Debug_Gui() {
	ImGui::DragFloat("hp", &health, 0.1f);
	ImGui::DragFloat("耐久度", &postureStability, 0.1f);
	ImGui::DragFloat("耐久度を削る割合", &postureStabilityScrapeRaito, 0.1f);
	SaveAndLoad();
}