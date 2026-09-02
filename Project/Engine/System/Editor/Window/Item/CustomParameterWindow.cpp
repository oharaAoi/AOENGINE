#include "CustomParameterWindow.h"

#include <array>
#include <algorithm>
#include <cstring>

#include "Engine/System/Editor/Parameter/CustomParameter.h"
#include "Engine/System/Editor/Parameter/CustomParameterManager.h"
#include "Engine/System/Manager/ImGuiManager.h"

using namespace AOENGINE;

void CustomParameterWindow::Init() {
	name_ = "Custom Parameters";
	isActive_ = true;
}

void CustomParameterWindow::Edit() {
	if (!ImGui::Begin(name_.c_str(), &isActive_)) {
		ImGui::End();
		return;
	}

	const auto& parameterSets = CustomParameterManager::GetInstance()->GetParameterSets();
	if (parameterSets.empty()) {
		ImGui::TextDisabled("No custom parameters registered.");
	}

	for (CustomParameterSet* parameters : parameterSets) {
		if (!parameters) { continue; }
		ImGui::PushID(parameters);
		if (ImGui::CollapsingHeader(parameters->GetDisplayName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
			for (const CustomParameterField& field : parameters->GetFields()) {
				if (!field.value) { continue; }
				switch (field.type) {
				case CustomParameterType::Bool:
					ImGui::Checkbox(field.label.c_str(), static_cast<bool*>(field.value));
					break;
				case CustomParameterType::Int:
					ImGui::DragInt(field.label.c_str(), static_cast<int32_t*>(field.value), field.speed,
						static_cast<int>(field.minValue), static_cast<int>(field.maxValue));
					break;
				case CustomParameterType::Float:
					ImGui::DragFloat(field.label.c_str(), static_cast<float*>(field.value), field.speed,
						field.minValue, field.maxValue);
					break;
				case CustomParameterType::Vector2:
					ImGui::DragFloat2(field.label.c_str(), &static_cast<Math::Vector2*>(field.value)->x,
						field.speed, field.minValue, field.maxValue);
					break;
				case CustomParameterType::Vector3:
					ImGui::DragFloat3(field.label.c_str(), &static_cast<Math::Vector3*>(field.value)->x,
						field.speed, field.minValue, field.maxValue);
					break;
				case CustomParameterType::Vector4:
					ImGui::DragFloat4(field.label.c_str(), &static_cast<Math::Vector4*>(field.value)->x,
						field.speed, field.minValue, field.maxValue);
					break;
				case CustomParameterType::Color:
					ImGui::ColorEdit4(field.label.c_str(), &static_cast<Color*>(field.value)->r);
					break;
				case CustomParameterType::String: {
					auto* value = static_cast<std::string*>(field.value);
					std::array<char, 512> buffer{};
					const size_t count = (std::min)(value->size(), buffer.size() - 1);
					std::memcpy(buffer.data(), value->data(), count);
					if (ImGui::InputText(field.label.c_str(), buffer.data(), buffer.size())) {
						*value = buffer.data();
					}
					break;
				}
				}
			}
		}
		ImGui::PopID();
	}

	ImGui::End();
}
