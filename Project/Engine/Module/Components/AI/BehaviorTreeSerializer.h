#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"

class BehaviorTreeSerializer {
public:

	static void Save(const std::string& filePath, const json& _json);

	static json LoadToJson(const std::string& filePath);

private:


};

