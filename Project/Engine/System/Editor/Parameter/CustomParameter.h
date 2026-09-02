#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"

namespace AOENGINE {

enum class CustomParameterType {
	Bool,
	Int,
	Float,
	Vector2,
	Vector3,
	Vector4,
	Color,
	String,
};

struct CustomParameterField {
	std::string label;
	CustomParameterType type = CustomParameterType::Float;
	void* value = nullptr;
	float speed = 0.1f;
	float minValue = 0.0f;
	float maxValue = 0.0f;
};

/// <summary>
/// ユーザー定義パラメータ構造体の基底クラス。
/// 生成時にCustomParameterManagerへ自動登録される。
/// </summary>
class CustomParameterSet {
public:
	explicit CustomParameterSet(std::string displayName);
	virtual ~CustomParameterSet();

	CustomParameterSet(const CustomParameterSet&) = delete;
	CustomParameterSet& operator=(const CustomParameterSet&) = delete;
	CustomParameterSet(CustomParameterSet&&) = delete;
	CustomParameterSet& operator=(CustomParameterSet&&) = delete;

	const std::string& GetDisplayName() const { return displayName_; }
	const std::vector<CustomParameterField>& GetFields() const { return fields_; }

protected:
	void AddParameter(const std::string& label, bool& value);
	void AddParameter(const std::string& label, int32_t& value, float speed = 1.0f,
		float minValue = 0.0f, float maxValue = 0.0f);
	void AddParameter(const std::string& label, float& value, float speed = 0.1f,
		float minValue = 0.0f, float maxValue = 0.0f);
	void AddParameter(const std::string& label, Math::Vector2& value, float speed = 0.1f,
		float minValue = 0.0f, float maxValue = 0.0f);
	void AddParameter(const std::string& label, Math::Vector3& value, float speed = 0.1f,
		float minValue = 0.0f, float maxValue = 0.0f);
	void AddParameter(const std::string& label, Math::Vector4& value, float speed = 0.1f,
		float minValue = 0.0f, float maxValue = 0.0f);
	void AddParameter(const std::string& label, Color& value);
	void AddParameter(const std::string& label, std::string& value);

private:
	void AddField(const std::string& label, CustomParameterType type, void* value,
		float speed = 0.1f, float minValue = 0.0f, float maxValue = 0.0f);

	std::string displayName_;
	std::vector<CustomParameterField> fields_;
};

}
