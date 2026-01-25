#pragma once
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace AOENGINE {
namespace PackagesWindowSerializer {
void Save(const std::unordered_map<std::string, bool>& stateMap, const std::string& currentPath);
void Load(std::unordered_map<std::string, bool>& map, std::string& currentPath);
}
}