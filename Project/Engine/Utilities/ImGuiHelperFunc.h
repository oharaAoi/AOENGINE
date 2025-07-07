#pragma once
#include <string>
#include <vector>
#include <unordered_map>

bool InputTextWithString(const char* label, std::string& str, size_t maxLength = 256);

int ContainerOfComb(const std::vector<std::string>& items, int& selectedIndex, const char* label = "##combo");