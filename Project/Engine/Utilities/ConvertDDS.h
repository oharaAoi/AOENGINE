#pragma once
#include <vector>
#include <stack>
#include <format>
#include <Windows.h>

std::wstring JoinArgs(const std::vector<std::wstring>& args);

bool RunPowerShellScript(
	const std::wstring& scriptPath,
	const std::vector<std::wstring>& inputPaths);
