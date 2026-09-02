#include "CustomParameter.h"

#include "Engine/System/Editor/Parameter/CustomParameterManager.h"

using namespace AOENGINE;

CustomParameterSet::CustomParameterSet(std::string displayName)
	: displayName_(std::move(displayName)) {
	CustomParameterManager::GetInstance()->Register(this);
}

CustomParameterSet::~CustomParameterSet() {
	CustomParameterManager::GetInstance()->Unregister(this);
}

void CustomParameterSet::AddField(const std::string& label, CustomParameterType type,
	void* value, float speed, float minValue, float maxValue) {
	fields_.push_back({ label, type, value, speed, minValue, maxValue });
}

void CustomParameterSet::AddParameter(const std::string& label, bool& value) {
	AddField(label, CustomParameterType::Bool, &value);
}

void CustomParameterSet::AddParameter(const std::string& label, int32_t& value,
	float speed, float minValue, float maxValue) {
	AddField(label, CustomParameterType::Int, &value, speed, minValue, maxValue);
}

void CustomParameterSet::AddParameter(const std::string& label, float& value,
	float speed, float minValue, float maxValue) {
	AddField(label, CustomParameterType::Float, &value, speed, minValue, maxValue);
}

void CustomParameterSet::AddParameter(const std::string& label, Math::Vector2& value,
	float speed, float minValue, float maxValue) {
	AddField(label, CustomParameterType::Vector2, &value, speed, minValue, maxValue);
}

void CustomParameterSet::AddParameter(const std::string& label, Math::Vector3& value,
	float speed, float minValue, float maxValue) {
	AddField(label, CustomParameterType::Vector3, &value, speed, minValue, maxValue);
}

void CustomParameterSet::AddParameter(const std::string& label, Math::Vector4& value,
	float speed, float minValue, float maxValue) {
	AddField(label, CustomParameterType::Vector4, &value, speed, minValue, maxValue);
}

void CustomParameterSet::AddParameter(const std::string& label, Color& value) {
	AddField(label, CustomParameterType::Color, &value);
}

void CustomParameterSet::AddParameter(const std::string& label, std::string& value) {
	AddField(label, CustomParameterType::String, &value);
}
