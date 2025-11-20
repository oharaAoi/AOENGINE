#include "IWorldState.h"
#include "Engine/System/Manager/ImGuiManager.h"

void IWorldState::Debug_Gui() {
	if (ImGui::BeginTable("WorldStateTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		ImGui::TableSetupColumn("Key");
		ImGui::TableSetupColumn("Value");
		ImGui::TableHeadersRow();

		for (auto& [key, value] : stateMap_) {
			ImGui::TableNextRow();

			// Key
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted(key.c_str());
			ImGui::TableSetColumnIndex(1);
			// Value
			std::visit([&](auto&& v) {
				using T = std::decay_t<decltype(v)>;

				if constexpr (std::is_same_v<T, int32_t*>) {
					ImGui::Text("%d", *v);
				} else if constexpr (std::is_same_v<T, float*>) {
					ImGui::Text("%.3f", *v);
				} else if constexpr (std::is_same_v<T, bool*>) {
					ImGui::Text(*v ? "true" : "false");
				} else if constexpr (std::is_same_v<T, std::string*>) {
					ImGui::TextUnformatted(v->c_str());
				}

			}, value.Get());
		}

		ImGui::EndTable();
	}
}

void IWorldState::KeyCombo(std::string& _key, int32_t& index, const std::string& _label) {
	std::vector<std::string> keys;
	for (auto& [k, _] : stateMap_) keys.push_back(k);

	if (ImGui::BeginCombo(_label.c_str(), _key.c_str())) {
		for (int i = 0; i < keys.size(); i++) {
			bool isSelected = (_key == keys[i]);
			if (ImGui::Selectable(keys[i].c_str(), isSelected)) {
				_key = keys[i];
				index = i;
			}
			if (isSelected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}
