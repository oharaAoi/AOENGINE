#include "IWorldState.h"
#include "Engine/System/AI/State/WorldStateSerializer.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/Utilities/FileDialogFunc.h"

void IWorldState::Debug_Gui() {
	// 値の追加
	CreateValue();

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

				TemplateValueText(v);

			}, value.Get());
		}

		ImGui::EndTable();
	}

	for (auto& [key, value] : stateMap_) {
		value.DebugValue(key, value);
	}

	if (ImGui::Button("Save##worldStateSave")) {
		Save();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load##worldStateLoad")) {
		path_ = FileOpenDialogFunc();
		Load(path_);
	}

}

void IWorldState::CreateValue() {
	static std::string newKey = "";
	static int combIndex = 0;
	InputTextWithString("key", "##worldStateKey", newKey, 16, 100);
	ImGui::SameLine();
	std::vector<std::string> valueArray = { "int", "float", "bool", "string" };
	combIndex =	ContainerOfComb(valueArray, combIndex, "##worldStateComb", 100);
	ImGui::SameLine();
	if (ImGui::Button(" + ")) {
		switch (combIndex) {
		case 0: // int型
			return this->Set(newKey, int32_t(0));
		case 1: // float型
			return this->Set(newKey, float(0));
		case 2: // bool型
			return this->Set(newKey, bool(false));
		case 3: // string型
			return this->Set(newKey, std::string(""));
		default:
			break;
		}
	}
}

void IWorldState::KeyCombo(std::string& _key, int32_t& index, const std::string& _label) {
	std::vector<std::string> keys;
	for (auto& [k, _] : stateMap_) keys.push_back(k);

	ImGui::SetNextItemWidth(80);
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

void IWorldState::Load(const std::string& _filePath) {
	if (_filePath != "") {
		WorldStateSerializer::Load(_filePath, stateMap_);
		path_ = _filePath;
	}
}

void IWorldState::Save() {
	path_ = FileSaveDialogFunc();
	if (path_ != "") {
		WorldStateSerializer::Save(path_, stateMap_);
	}
}