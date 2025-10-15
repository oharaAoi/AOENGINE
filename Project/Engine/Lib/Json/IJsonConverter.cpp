#include "IJsonConverter.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "imgui.h"

void IJsonConverter::SaveAndLoad() {
	std::string saveName = "Save##";
	saveName += name_.c_str();
	if (ImGui::Button(saveName.c_str())) {
		Save();
	}
	std::string loadName = "Load##";
	loadName += name_.c_str();
	if (ImGui::Button(loadName.c_str())) {
		Load();
	}
}

void IJsonConverter::Save() {
	JsonItems::Save(groupName_, ToJson(name_));
}

void IJsonConverter::Load() {
	FromJson(JsonItems::GetData(groupName_, name_));
}