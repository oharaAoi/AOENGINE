#include "IJsonConverter.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "imgui.h"

void IJsonConverter::SaveAndLoad() {
	if (ImGui::Button("Save")) {
		Save();
	}
	if (ImGui::Button("Load")) {
		Load();
	}
}

void IJsonConverter::Save() {
	JsonItems::Save(groupName_, ToJson(name_));
}

void IJsonConverter::Load() {
	FromJson(JsonItems::GetData(groupName_, name_));
}