#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace AOENGINE {

namespace JsonSerializer {

bool Save(const std::string& folderPath, const std::string& fileName, const nlohmann::json& json);

nlohmann::json Load(const std::string& folderPath, const std::string& fileName);

}

}
