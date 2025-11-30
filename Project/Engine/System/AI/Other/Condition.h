#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "Engine/System/AI/State/WorldStateValue.h"
#include "Engine/System/AI/State/IWorldState.h"

/// <summary>
/// if文となるクラス
/// </summary>
class Condition {
public:

	Condition() = default;
	~Condition() = default;

public:

	bool Execute(IWorldState* _state);

	/// <summary>
	/// 判断する
	/// </summary>
	bool Compare(const WorldStateValue& lhs,
				 const WorldStateValue& rhs,
				 const std::string& op);

	void Debug_Gui(IWorldState* _state);

	nlohmann::json ToJson();

	void FromJson(const nlohmann::json& _jsonData);

private:

	static const int32_t kOperatorCount_ = 6;
	const char* conditionOps[kOperatorCount_] = {
	"==", "!=", ">", "<", ">=", "<="
	};
	int32_t opIndex_ = 0;
	int32_t leftKeyIndex_ = 0;
	int32_t rightKeyIndex_ = 0;

	std::string leftKey_ = "";
	std::string rightKey_ = "";

};

