#pragma once
#include <string>
#include <vector>
#include <unordered_map>

bool InputTextWithString(const char* filedName, const char* label, std::string& str, size_t maxLength = 256);

bool ButtonOpenDialog(const char* buttonLabel, const char* dialogKey, const char* windowTitle, const char* filter, std::string& outPath);

int ContainerOfComb(const std::vector<std::string>& items, int& selectedIndex, const char* label = "##combo");