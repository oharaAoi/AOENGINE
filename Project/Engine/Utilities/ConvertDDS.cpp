#include "ConvertDDS.h"
#include "Engine/Utilities/Logger.h"
#include "Engine/Utilities/Convert.h"

std::wstring JoinArgs(const std::vector<std::wstring>& args) {
	std::wstring result;
	for (auto& a : args) {
		result += L" \"";
		result += a;
		result += L"\"";
	}
	return result;
}


bool RunPowerShellScript(
	const std::wstring& scriptPath,
	const std::vector<std::wstring>& inputPaths) {
	std::wstring cmd = L"powershell -ExecutionPolicy Bypass -File \"";
	cmd += scriptPath;
	cmd += L"\"";
	cmd += JoinArgs(inputPaths);

	std::vector<wchar_t> cmdLine(cmd.begin(), cmd.end());
	cmdLine.push_back(L'\0');

	STARTUPINFOW si{};
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi{};

	BOOL ok = CreateProcessW(
		nullptr,
		cmdLine.data(),
		nullptr, nullptr, FALSE, 0,
		nullptr, nullptr,
		&si, &pi
	);

	if (!ok)
		return false;

	AOENGINE::Logger::Log(ConvertString(cmd));
	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return true;
}
